#include "sys.h"
#include "delay.h"
#include "gpio.h"
#include "key.h"
#include "motor_duoji.h"
#include "oled.h"
#include "RLY.h"
#include "hcsr04.h"

/*==========================================================
 *  常量定义 (Constants Definition)
 *==========================================================*/
// 距离检测相关 (Distance Detection)
#define DIST_THR       20      // 超声波检测阈值 cm (要求: 10-30cm)
#define DIST_DET_CNT   3       // 连续检测次数要求 (防抖)

// 延时相关 (Timing)
#define CLOSE_DLY_DEF  5       // 默认关盖延时 秒 (要求: 3-10s)
#define CLOSE_DLY_MIN  3       // 最小关盖延时
#define CLOSE_DLY_MAX  10      // 最大关盖延时

// 紫外消毒相关 (UV Disinfection)
#define UV_DEF         60      // 默认消毒时长 秒 (要求: 1-5分钟)
#define UV_MIN         60      // 最小消毒时长
#define UV_MAX         300     // 最大消毒时长
#define UV_STEP        30      // 调节步长

// 故障检测相关 (Fault Detection)
#define FAULT_THR      15      // 故障计数阈值
#define STARTUP_SKIP   60      // 启动时忽略满溢检测次数 (60*200ms=12s)
#define OVERFLOW_CNT   15      // 满溢确认计数 (15*200ms=3s连续检测)

// 舵机控制相关 (Servo Control)
#define SERVO_CLS      1850    // 关盖PWM值
#define SERVO_OPN      1950    // 开盖PWM值
#define SERVO_STP      10      // 缓启缓停步进值

// 状态机定义 (State Machine)
#define LID_CLOSED     0       // 盖子关闭
#define LID_OPEN       1       // 盖子打开
#define LID_WAIT       2       // 等待关闭

// 日志与报警 (Logging & Alarm)
#define MAX_FLOG       8       // 最大故障日志条数
#define F_SENSOR       1       // 传感器故障标志
#define F_OVERFLOW     2       // 溢出故障标志

// 主循环时间片 (Main Loop Tick)
#define TICK_MS        50      // 主循环周期 ms

/*==========================================================
 *  全局变量
 *==========================================================*/
uint8_t  mode = 0;               // 0=手动 1=自动
uint8_t  lid_state = LID_CLOSED;
uint8_t  cur_page = 0;           // 0=主页 1=消毒页
uint8_t  uv_on = 0;
uint16_t uv_set = UV_DEF;
uint16_t uv_cnt = 0;
uint8_t  auto_uv_pend = 0;
uint8_t  cdly_set = CLOSE_DLY_DEF;
uint8_t  cdly_cnt = 0;
uint8_t  f_full = 0;
uint8_t  f_fault = 0;
uint8_t  alarm = 0;
uint16_t ferr_cnt = 0;
uint8_t  flog[MAX_FLOG];
uint8_t  flog_n = 0;
uint16_t dist_val = 0;
uint8_t  det_cnt = 0;
uint16_t tick = 0;
uint8_t  disp_dirty = 1;
uint8_t  idle_slow = 0;

/*==========================================================
 *  函数声明
 *==========================================================*/
void Key_func(void);
void Monitor_func(void);
void Manage_func(void);
void Timer_1s(void);
void Alarm_func(void);
void Flow_LED_Off(void);
void Flow_LED_Set(uint8_t idx);
void Refresh(void);
void UV_Start(void);
void UV_Stop(void);
void Stop_UV_Before_Lid_Op(void);  // 新增：开盖前停止消毒的统一处理
void Log_F(uint8_t t);
void Servo_Open(void);
void Servo_Close(void);
void Beep_Init(void);
void Beep_On(void);
void Beep_Off(void);
void Beep_Short(void);

/*==========================================================
 *  主函数
 *==========================================================*/
int main(void)
{
    Delay_Init();
    Gpio_Init();
    Key_Init();
    Motor_Duoji_Init();
    OLED_Init();
    RLY_Init();
    Beep_Init();
    HCSR04_Init();

    Motor_Duoji_SetCompare(SERVO_CLS);
    Delay_ms(100);
    LED1_G = 0;  LED1_R = 1;
    LED2_G = 1;  LED2_R = 1;
    OLED_ShowMainPage();
    disp_dirty = 0;  // ShowMainPage 已绘制，无需主循环立即重绘

    while(1)
    {
        tick++;
        Key_func();

        if(mode == 1)
        {
            uint8_t si = idle_slow ? 20 : 4;
            if(tick % si == 0) Monitor_func();
        }
        if(tick % 4 == 0)  Manage_func();
        if(tick % 20 == 0) Timer_1s();
        if(tick % 2 == 0)  Alarm_func();

        if(disp_dirty) { Refresh(); disp_dirty = 0; }
        Delay_ms(TICK_MS);
    }
}

/*==========================================================
 *  蜂鸣器 (PA0 低电平触发)
 *==========================================================*/
void Beep_Init(void)
{
    GPIO_InitTypeDef s;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    s.GPIO_Pin   = GPIO_Pin_0;
    s.GPIO_Mode  = GPIO_Mode_Out_PP;
    s.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &s);
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
}
void Beep_On(void)  { GPIO_ResetBits(GPIOA, GPIO_Pin_0); }
void Beep_Off(void) { GPIO_SetBits(GPIOA, GPIO_Pin_0); }
void Beep_Short(void) { Beep_On(); Delay_ms(80); Beep_Off(); }

/*==========================================================
 *  舵机缓启缓停
 *==========================================================*/
void Servo_Open(void)
{
    uint16_t c = SERVO_CLS;
    while(c < SERVO_OPN) { c += SERVO_STP; if(c > SERVO_OPN) c = SERVO_OPN; Motor_Duoji_SetCompare(c); Delay_ms(20); }
    Delay_ms(300);
    Motor_Duoji_SetCompare(0);  // 到位后停止 PWM，释放 3.3V 压力
}
void Servo_Close(void)
{
    uint16_t c = SERVO_OPN;
    while(c > SERVO_CLS) { c -= SERVO_STP; if(c < SERVO_CLS) c = SERVO_CLS; Motor_Duoji_SetCompare(c); Delay_ms(20); }
    Delay_ms(300);
    Motor_Duoji_SetCompare(0);  // 到位后停止 PWM，释放 3.3V 压力
}

/*==========================================================
 *  异常记录 / 消毒启停 / 开盖前消毒停止处理
 *==========================================================*/
void Log_F(uint8_t t)
{
    if(flog_n < MAX_FLOG) flog[flog_n++] = t;
}

/* 开盖前停止消毒的统一处理函数
 * 根据要求文档：开盖优先级高于消毒，开盖前必须停止消毒
 * 集中处理逻辑，避免代码重复
 */
void Stop_UV_Before_Lid_Op(void)
{
    if(uv_on)
    {
        UV_Stop();
        auto_uv_pend = 0;  // 取消待触发的自动消毒
    }
}

void UV_Start(void)
{
    if(uv_on) return;
    uv_on = 1; uv_cnt = uv_set;
    RLY_On(); LED2_G = 0;
    cur_page = 1; disp_dirty = 1;
}

void UV_Stop(void)
{
    uv_on = 0; uv_cnt = 0;
    RLY_Off(); LED2_G = 1;
    cur_page = 0; disp_dirty = 1;
}

/*==========================================================
 *  按键函数 (Key Control Function)
 *  优化点：
 *  1. 使用统一的Stop_UV_Before_Lid_Op()避免代码重复
 *  2. 增加详细中文注释说明按键功能
 *  3. KEY4报警解除在key.c中已优先处理
 *==========================================================*/
void Key_func(void)
{
    uint8_t k = Chiclet_Keyboard_Scan(0);
    if(k == 0) return;

    switch(k)
    {
    case 1: /* KEY1: 手动模式=开关盖切换; 自动模式=手动触发开盖 */
        if(mode == 0)  // 手动模式
        {
            Stop_UV_Before_Lid_Op();  // 开盖前停止消毒
            if(lid_state == LID_CLOSED)
            {
                Beep_Short();
                Servo_Open();
                lid_state = LID_OPEN;
            }
            else
            {
                Servo_Close();
                lid_state = LID_CLOSED;
            }
            disp_dirty = 1;
        }
        else if(lid_state == LID_CLOSED)  // 自动模式下手动触发开盖
        {
            Stop_UV_Before_Lid_Op();  // 开盖前停止消毒
            Beep_Short();
            Servo_Open();
            lid_state = LID_OPEN;
            cdly_cnt = cdly_set;
            det_cnt = 0;
            disp_dirty = 1;
        }
        break;

    case 2: /* KEY2: 消毒时长+30s */
        if(uv_set < UV_MAX)
        {
            uv_set += UV_STEP;
            Beep_Short();
            disp_dirty = 1;
        }
        break;

    case 3: /* KEY3: 消毒时长-30s */
        if(uv_set > UV_MIN)
        {
            uv_set -= UV_STEP;
            Beep_Short();
            disp_dirty = 1;
        }
        break;

    case 4: /* KEY4: 报警解除 (已在key.c中优先处理) */
        if(alarm)
        {
            alarm = 0;
            f_fault = 0;
            ferr_cnt = 0;
            Beep_Off();
            Flow_LED_Off();
            // 恢复正常LED状态
            if(!f_full) { LED1_G = 0; LED1_R = 1; }
            disp_dirty = 1;
        }
        break;

    case 5: /* KEY5: 手动消毒启停 */
        if(uv_on)
            UV_Stop();
        else
            UV_Start();
        break;

    case 6: /* KEY6: 模式切换 (手动<->自动) */
        // 切换模式前先停止消毒和关闭盖子
        if(cur_page == 1) UV_Stop();
        if(lid_state != LID_CLOSED)
        {
            Servo_Close();
            lid_state = LID_CLOSED;
        }
        // 切换模式
        mode = !mode;
        det_cnt = 0;
        cdly_cnt = 0;
        idle_slow = 0;
        if(mode == 0) auto_uv_pend = 0;  // 手动模式禁用自动消毒触发
        Beep_Short();
        disp_dirty = 1;
        break;
    }
}

/*==========================================================
 *  监测函数 - 超声波自动开盖 (自动模式)
 *  优化点：
 *  1. 使用DIST_DET_CNT常量替代硬编码3
 *  2. 简化idle_slow逻辑
 *  3. 使用统一的Stop_UV_Before_Lid_Op()
 *  4. 增加注释说明状态机转换
 *==========================================================*/
void Monitor_func(void)
{
    dist_val = HCSR04_GetDistance();

    // 999表示无目标(超出测距范围)，属正常情况，不是故障
    if(dist_val == 999u)
    {
        det_cnt = 0;
        idle_slow = 1;  // 无人时降低扫描频率节能
        return;
    }

    // 有效距离读取，清除故障计数
    ferr_cnt = 0;

    switch(lid_state)
    {
    case LID_CLOSED:  // 盖子关闭状态
        if(dist_val < DIST_THR)
        {
            det_cnt++;
            // 连续检测到人靠近(防抖处理)
            if(det_cnt >= DIST_DET_CNT)
            {
                Stop_UV_Before_Lid_Op();  // 开盖前停止消毒
                Beep_Short();
                Servo_Open();
                lid_state = LID_OPEN;
                cdly_cnt = cdly_set;
                idle_slow = 0;  // 有人活动，提高扫描频率
                disp_dirty = 1;
                det_cnt = 0;
            }
        }
        else
        {
            det_cnt = 0;
            idle_slow = 1;  // 无人靠近，降低扫描频率
        }
        break;

    case LID_OPEN:  // 盖子打开状态
        if(dist_val < DIST_THR)
        {
            cdly_cnt = cdly_set;  // 检测到人，重置关盖倒计时
        }
        else
        {
            lid_state = LID_WAIT;  // 人离开，进入等待关闭状态
        }
        break;

    case LID_WAIT:  // 等待关闭状态
        if(dist_val < DIST_THR)
        {
            // 人又靠近，返回打开状态
            lid_state = LID_OPEN;
            cdly_cnt = cdly_set;
        }
        // 否则保持等待，由Timer_1s倒计时关闭
        break;
    }
}

/*==========================================================
 *  管理函数 - 满溢检测 (Overflow Detection)
 *  优化点：
 *  1. 使用STARTUP_SKIP和OVERFLOW_CNT常量替代魔数
 *  2. 改进注释说明检测逻辑
 *  3. 优化变量命名和代码结构
 *==========================================================*/
void Manage_func(void)
{
    static uint8_t fc = 0;  // 满溢连续检测计数器
    static uint16_t startup_skip = STARTUP_SKIP;  // 启动忽略计数

    // 启动前12秒忽略传感器数据，等待稳定
    if(startup_skip > 0)
    {
        startup_skip--;
        return;
    }

    // IR_1==0 表示红外对射被遮挡(垃圾满)
    if(IR_1 == 0)
    {
        fc++;
        if(fc > OVERFLOW_CNT) fc = OVERFLOW_CNT;  // 限制最大值

        // 连续3秒检测到满溢才确认(防抖)
        if(fc >= OVERFLOW_CNT)
        {
            // 更新OLED显示
            if(cur_page == 0)
                OLED_ShowChinese(80, 0, 13, 16, 1);  // 显示"满"

            // LED指示：红灯亮，绿灯灭
            LED1_G = 1;
            LED1_R = 0;

            // 首次检测到满溢，触发报警
            if(!f_full)
            {
                f_full = 1;
                alarm = 1;
                // 三次短鸣提示
                Beep_On(); Delay_ms(150); Beep_Off(); Delay_ms(100);
                Beep_On(); Delay_ms(150); Beep_Off(); Delay_ms(100);
                Beep_On(); Delay_ms(150); Beep_Off();
                Log_F(F_OVERFLOW);  // 记录溢出日志
                disp_dirty = 1;
            }
        }
    }
    else  // 传感器未遮挡，垃圾未满
    {
        fc = 0;

        // 更新OLED显示
        if(cur_page == 0)
            OLED_ShowChinese(80, 0, 17, 16, 1);  // 显示"正常"

        // LED指示：绿灯亮，红灯灭(无故障时)
        if(!f_fault)
        {
            LED1_G = 0;
            LED1_R = 1;
        }

        // 清除满溢标志
        f_full = 0;

        // 如果无其他故障，解除报警
        if(!f_fault)
            alarm = 0;
    }
}

/*==========================================================
 *  1秒处理
 *==========================================================*/
void Timer_1s(void)
{
    if(uv_on && uv_cnt > 0)
    {
        uv_cnt--;
        if(cur_page == 1) { OLED_ShowNum(80, 48, uv_cnt, 3, 16, 1); OLED_ShowChar(104, 48, 's', 16, 1); }
        if(uv_cnt == 0) UV_Stop();
    }
    if(mode == 1 && auto_uv_pend && lid_state == LID_CLOSED && !uv_on)
    { auto_uv_pend = 0; UV_Start(); }

    if(mode == 1 && lid_state == LID_WAIT && cdly_cnt > 0)
    {
        cdly_cnt--;
        if(cdly_cnt == 0)
        {
            Servo_Close(); lid_state = LID_CLOSED; disp_dirty = 1;
            if(!uv_on) auto_uv_pend = 1;
        }
    }
}

/*==========================================================
 *  流水灯：关闭所有报警LED
 *==========================================================*/
void Flow_LED_Off(void)
{
    LED1_G = 1; LED1_R = 1;
    LED2_G = 1; LED2_R = 1;
    LED3_G = 1; LED3_R = 1;
    LED4_G = 1; LED4_R = 1;
}

/* 按序号点亮一颗LED (0~7) */
void Flow_LED_Set(uint8_t idx)
{
    Flow_LED_Off();
    switch(idx)
    {
    case 0: LED1_G = 0; break;
    case 1: LED1_R = 0; break;
    case 2: LED2_G = 0; break;
    case 3: LED2_R = 0; break;
    case 4: LED3_G = 0; break;
    case 5: LED3_R = 0; break;
    case 6: LED4_G = 0; break;
    case 7: LED4_R = 0; break;
    }
}

/*==========================================================
 *  报警处理 (Alarm Handler) - 每100ms调用
 *  优化点：
 *  1. 简化蜂鸣器切换逻辑
 *  2. 添加详细注释说明流水灯和蜂鸣器模式
 *  3. 改进可读性
 *==========================================================*/
void Alarm_func(void)
{
    static uint8_t flow_idx = 0;   // 流水灯索引 (0-7循环)
    static uint8_t beep_tog = 0;   // 蜂鸣器切换计数

    if(alarm)
    {
        // 流水灯效果：8个LED依次点亮，100ms一个
        Flow_LED_Set(flow_idx);
        flow_idx = (flow_idx + 1) & 7;  // 位运算取模，0~7循环

        // 蜂鸣器模式：400ms周期，响200ms停200ms
        // beep_tog: 0-1响, 2-3停, 然后循环
        beep_tog++;
        if(beep_tog >= 4) beep_tog = 0;

        if(beep_tog < 2)
            Beep_On();   // 前200ms响
        else
            Beep_Off();  // 后200ms停
    }
    else
    {
        // 报警解除，复位状态
        flow_idx = 0;
        beep_tog = 0;
        Beep_Off();

        // 恢复正常LED状态 (LED2-4用于流水灯，需全灭)
        LED2_R = 1;
        LED3_G = 1;
        LED3_R = 1;
        LED4_G = 1;
        LED4_R = 1;
        // LED1和LED2_G由满溢检测和UV状态控制，不在此恢复
    }
}

/*==========================================================
 *  显示刷新
 *==========================================================*/
void Refresh(void)
{
    OLED_BeginBatch();

    if(cur_page == 1)
    {
        OLED_ShowDisinfectPage();
        OLED_ShowNum(80, 48, uv_cnt, 3, 16, 1);
        OLED_ShowChar(104, 48, 's', 16, 1);
        OLED_EndBatch();
        return;
    }

    OLED_Clear();
    OLED_ShowChinese(0, 0, 0, 16, 1);
    OLED_ShowChinese(16, 0, 1, 16, 1);
    OLED_ShowChinese(32, 0, 15, 16, 1);
    OLED_ShowChinese(48, 0, 16, 16, 1);
    OLED_ShowChar(64, 0, ':', 16, 1);
    OLED_ShowChinese(80, 0, f_full ? 13 : 17, 16, 1);
    OLED_ShowChinese(96, 0, 14, 16, 1);

    if(mode == 0)
    {
        OLED_ShowString(0, 16, (u8*)"Mode:Manual     ", 16, 1);
        OLED_ShowString(0, 32, lid_state == LID_CLOSED ? (u8*)"Lid:Closed      " : (u8*)"Lid:Open        ", 16, 1);
        if(alarm)
            OLED_ShowString(0, 48, (u8*)"!ALM  K4:Clear  ", 16, 1);
        else
        {
            OLED_ShowString(0, 48, (u8*)"UV:", 16, 1);
            OLED_ShowNum(24, 48, uv_set, 3, 16, 1);
            OLED_ShowString(48, 48, (u8*)"s K2+K3-", 16, 1);
        }
    }
    else
    {
        OLED_ShowString(0, 16, (u8*)"Mode:Auto       ", 16, 1);
        if(lid_state == LID_CLOSED)
            OLED_ShowString(0, 32, (u8*)"Lid:Closed      ", 16, 1);
        else if(lid_state == LID_OPEN)
            OLED_ShowString(0, 32, (u8*)"Lid:Open        ", 16, 1);
        else
            OLED_ShowString(0, 32, (u8*)"Lid:Closing...  ", 16, 1);

        if(alarm)
            OLED_ShowString(0, 48, (u8*)"!ALM  K4:Clear  ", 16, 1);
        else
        {
            OLED_ShowString(0, 48, (u8*)"UV:", 16, 1);
            OLED_ShowNum(24, 48, uv_set, 3, 16, 1);
            OLED_ShowString(48, 48, (u8*)"s Dly:", 16, 1);
            OLED_ShowNum(96, 48, cdly_set, 1, 16, 1);
            OLED_ShowChar(104, 48, 's', 16, 1);
        }
    }
    OLED_EndBatch();
}
