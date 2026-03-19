#include "sys.h"
#include "delay.h"
#include "gpio.h"
#include "key.h"
#include "motor_duoji.h"
#include "oled.h"
#include "alarm.h"

/************************************************************
 * 智能停车场系统 - 主程序
 * 基于 STM32F103C8T6
 *
 * 功能说明:
 * 1. 车位检测与计数统计
 * 2. 车辆进出识别与道闸控制
 * 3. 手动与自动模式切换
 * 4. 异常报警功能
 * 5. 本地数据显示与参数设置
 ************************************************************/

/*==========================================================
 * 常量定义
 *==========================================================*/
#define TOTAL_SPACES_DEFAULT    3       // 默认总车位数
#define TOTAL_SPACES_MAX        8       // 最大车位数
#define TOTAL_SPACES_MIN        1       // 最小车位数

#define GATE_AUTO_CLOSE_TIME    5       // 道闸自动关闭时间(秒)
#define DEBOUNCE_COUNT          5       // 防抖计数阈值
#define SENSOR_POLL_INTERVAL    200     // 传感器轮询间隔(ms)

/*==========================================================
 * 系统状态变量
 *==========================================================*/
uint8_t  system_mode = 1;               // 0=手动模式, 1=自动模式
uint8_t  total_spaces = TOTAL_SPACES_DEFAULT;   // 总车位数
uint8_t  free_spaces = TOTAL_SPACES_DEFAULT;    // 空余车位数
uint8_t  entrance_gate_state = 0;       // 0=关闭, 1=开启
uint8_t  exit_gate_state = 0;           // 0=关闭, 1=开启
uint8_t  emergency_mode = 0;            // 0=正常, 1=紧急放行
uint8_t  menu_mode = 0;                 // 0=正常显示, 1=设置菜单
uint8_t  alarm_active = 0;              // 报警激活标志

// 车位占用状态 (0=空闲, 1=占用)
uint8_t space_occupied[3] = {0, 0, 0};
uint8_t space_occupied_prev[3] = {0, 0, 0};

// 入口/出口传感器防抖计数
uint8_t entrance_debounce = 0;
uint8_t exit_debounce = 0;
uint8_t entrance_triggered = 0;
uint8_t exit_triggered = 0;

// 道闸自动关闭计时
uint16_t entrance_close_timer = 0;
uint16_t exit_close_timer = 0;

// 系统时钟计数
uint32_t system_tick = 0;

/*==========================================================
 * 函数声明
 *==========================================================*/
void System_Init(void);
void Key_Process(void);
void Sensor_Process(void);
void Gate_Process(void);
void Display_Update(void);
void Timer_1s_Process(void);

/*==========================================================
 * 主函数
 *==========================================================*/
int main(void)
{
    System_Init();

    while(1)
    {
        system_tick++;

        // 按键处理
        Key_Process();

        // 传感器处理 (每200ms执行一次)
        if(system_tick % 4 == 0)
        {
            Sensor_Process();
        }

        // 道闸控制处理
        if(system_tick % 2 == 0)
        {
            Gate_Process();
        }

        // 1秒定时处理
        if(system_tick % 20 == 0)
        {
            Timer_1s_Process();
        }

        // 显示更新
        Display_Update();

        Delay_ms(50);  // 主循环周期50ms
    }
}

/*==========================================================
 * 功能: 系统初始化
 *==========================================================*/
void System_Init(void)
{
    // 延时初始化
    Delay_Init();

    // GPIO初始化
    GPIO_Config();

    // 按键初始化
    KEY_Init();

    // 舵机初始化
    Servo_Init();

    // OLED初始化
    OLED_Init();

    // 报警系统初始化
    Alarm_Init();

    // 初始状态：两个道闸都关闭
    Servo_Entrance_Close();
    Servo_Exit_Close();

    // 显示欢迎信息
    OLED_Clear();
    OLED_ShowString(0, 0, (u8*)"Smart Parking", 16, 1);
    OLED_ShowString(0, 16, (u8*)"System", 16, 1);
    OLED_ShowString(0, 32, (u8*)"Initializing...", 16, 1);
    OLED_Refresh();
    Delay_ms(2000);

    // 短促提示音
    Alarm_Beep_Short();
}

/*==========================================================
 * 功能: 按键处理
 *==========================================================*/
void Key_Process(void)
{
    uint8_t key = KEY_Scan(0);

    if(key == KEY_NONE)
        return;

    Alarm_Beep_Short();  // 按键提示音

    switch(key)
    {
        case KEY_MODE_SWITCH:  // SW2: 自动/手动模式切换
            system_mode = !system_mode;
            if(system_mode == 0)
            {
                // 切换到手动模式，关闭所有道闸
                if(entrance_gate_state == 1)
                {
                    Servo_Entrance_Close();
                    entrance_gate_state = 0;
                }
                if(exit_gate_state == 1)
                {
                    Servo_Exit_Close();
                    exit_gate_state = 0;
                }
            }
            break;

        case KEY_ENTRANCE_GATE:  // SW3: 入口道闸控制
            if(system_mode == 0)  // 仅手动模式有效
            {
                if(entrance_gate_state == 0)
                {
                    Servo_Entrance_Open();
                    entrance_gate_state = 1;
                    entrance_close_timer = GATE_AUTO_CLOSE_TIME;
                }
                else
                {
                    Servo_Entrance_Close();
                    entrance_gate_state = 0;
                    entrance_close_timer = 0;
                }
            }
            break;

        case KEY_EXIT_GATE:  // SW4: 出口道闸控制
            if(system_mode == 0)  // 仅手动模式有效
            {
                if(exit_gate_state == 0)
                {
                    Servo_Exit_Open();
                    exit_gate_state = 1;
                    exit_close_timer = GATE_AUTO_CLOSE_TIME;
                }
                else
                {
                    Servo_Exit_Close();
                    exit_gate_state = 0;
                    exit_close_timer = 0;
                }
            }
            break;

        case KEY_EMERGENCY:  // SW5: 紧急放行
            emergency_mode = !emergency_mode;
            if(emergency_mode)
            {
                // 紧急模式：打开所有道闸
                Servo_Entrance_Open();
                Servo_Exit_Open();
                entrance_gate_state = 1;
                exit_gate_state = 1;
                LED_ALARM_ON();
            }
            else
            {
                // 退出紧急模式
                LED_ALARM_OFF();
            }
            break;

        case KEY_MENU:  // SW6: 菜单/确认
            menu_mode = !menu_mode;
            if(menu_mode == 0)
            {
                // 退出菜单，保存设置
                // 更新空余车位数
                uint8_t occupied_count = 0;
                for(uint8_t i = 0; i < 3; i++)
                {
                    if(space_occupied[i])
                        occupied_count++;
                }
                free_spaces = total_spaces - occupied_count;
            }
            break;

        case KEY_SPACE_ADD:  // SW7: 增加车位
            if(menu_mode && total_spaces < TOTAL_SPACES_MAX)
            {
                total_spaces++;
                free_spaces++;
            }
            break;

        case KEY_SPACE_SUB:  // SW8: 减少车位
            if(menu_mode && total_spaces > TOTAL_SPACES_MIN)
            {
                total_spaces--;
                if(free_spaces > 0)
                    free_spaces--;
            }
            break;
    }
}

/*==========================================================
 * 功能: 传感器处理 (车位检测 + 入口/出口检测)
 *==========================================================*/
void Sensor_Process(void)
{
    uint8_t i;
    uint8_t occupied_count = 0;

    // 读取车位传感器状态
    // 空闲=高电平(1), 占用=低电平(0)
    space_occupied[0] = (IR_SPACE1 == 0) ? 1 : 0;
    space_occupied[1] = (IR_SPACE2 == 0) ? 1 : 0;
    space_occupied[2] = (IR_SPACE3 == 0) ? 1 : 0;

    // 统计占用车位数
    for(i = 0; i < 3; i++)
    {
        if(space_occupied[i])
            occupied_count++;
    }

    // 更新空余车位数
    if(occupied_count <= total_spaces)
        free_spaces = total_spaces - occupied_count;
    else
        free_spaces = 0;

    // 车位已满报警
    if(free_spaces == 0 && alarm_active == 0)
    {
        alarm_active = 1;
        Alarm_Full_Warning();
    }
    else if(free_spaces > 0)
    {
        alarm_active = 0;
        Alarm_Off();
    }

    // 自动模式下的入口/出口检测
    if(system_mode == 1 && emergency_mode == 0)
    {
        // 入口检测 (车辆靠近时传感器输出低电平)
        if(IR_ENTRANCE == 0)
        {
            entrance_debounce++;
            if(entrance_debounce >= DEBOUNCE_COUNT && entrance_triggered == 0)
            {
                entrance_triggered = 1;

                // 有空余车位才开启道闸
                if(free_spaces > 0 && entrance_gate_state == 0)
                {
                    Servo_Entrance_Open();
                    entrance_gate_state = 1;
                    entrance_close_timer = GATE_AUTO_CLOSE_TIME;
                    Alarm_Beep_Short();
                }
                else if(free_spaces == 0)
                {
                    // 车位已满，不开启道闸
                    Alarm_Full_Warning();
                }
            }
        }
        else
        {
            entrance_debounce = 0;
            entrance_triggered = 0;
        }

        // 出口检测
        if(IR_EXIT == 0)
        {
            exit_debounce++;
            if(exit_debounce >= DEBOUNCE_COUNT && exit_triggered == 0)
            {
                exit_triggered = 1;

                // 开启出口道闸
                if(exit_gate_state == 0)
                {
                    Servo_Exit_Open();
                    exit_gate_state = 1;
                    exit_close_timer = GATE_AUTO_CLOSE_TIME;
                    Alarm_Beep_Short();
                }
            }
        }
        else
        {
            exit_debounce = 0;
            exit_triggered = 0;
        }
    }
}

/*==========================================================
 * 功能: 道闸控制处理
 *==========================================================*/
void Gate_Process(void)
{
    // 紧急模式下，道闸保持开启
    if(emergency_mode)
        return;

    // 入口道闸自动关闭逻辑
    if(entrance_gate_state == 1 && entrance_close_timer == 0)
    {
        Servo_Entrance_Close();
        entrance_gate_state = 0;
    }

    // 出口道闸自动关闭逻辑
    if(exit_gate_state == 1 && exit_close_timer == 0)
    {
        Servo_Exit_Close();
        exit_gate_state = 0;
    }
}

/*==========================================================
 * 功能: 显示更新
 *==========================================================*/
void Display_Update(void)
{
    static uint32_t last_update = 0;

    // 每500ms更新一次显示
    if(system_tick - last_update < 10)
        return;

    last_update = system_tick;

    OLED_Clear();

    if(menu_mode)
    {
        // 设置菜单显示
        OLED_ShowString(0, 0, (u8*)"=== SETTING ===", 16, 1);
        OLED_ShowString(0, 16, (u8*)"Total Space:", 16, 1);
        OLED_ShowNum(96, 16, total_spaces, 2, 16, 1);
        OLED_ShowString(0, 32, (u8*)"SW6:Save & Exit", 16, 1);
        OLED_ShowString(0, 48, (u8*)"SW7:+ SW8:-", 16, 1);
    }
    else
    {
        // 主界面显示
        // 第一行：模式和紧急状态
        if(emergency_mode)
        {
            OLED_ShowString(0, 0, (u8*)"EMERGENCY MODE!", 16, 1);
        }
        else
        {
            OLED_ShowString(0, 0, system_mode ? (u8*)"Mode:Auto" : (u8*)"Mode:Manual", 16, 1);
        }

        // 第二行：总车位和空余车位
        OLED_ShowString(0, 16, (u8*)"Total:", 16, 1);
        OLED_ShowNum(48, 16, total_spaces, 2, 16, 1);
        OLED_ShowString(72, 16, (u8*)"Free:", 16, 1);
        OLED_ShowNum(112, 16, free_spaces, 1, 16, 1);

        // 第三行：入口道闸状态
        OLED_ShowString(0, 32, (u8*)"IN:", 16, 1);
        OLED_ShowString(32, 32, entrance_gate_state ? (u8*)"OPEN " : (u8*)"CLOSE", 16, 1);

        // 第四行：出口道闸状态
        OLED_ShowString(0, 48, (u8*)"OUT:", 16, 1);
        OLED_ShowString(32, 48, exit_gate_state ? (u8*)"OPEN " : (u8*)"CLOSE", 16, 1);

        // 车位已满提示
        if(free_spaces == 0)
        {
            OLED_ShowString(80, 32, (u8*)"FULL!", 16, 1);
        }
    }

    OLED_Refresh();
}

/*==========================================================
 * 功能: 1秒定时处理
 *==========================================================*/
void Timer_1s_Process(void)
{
    // 入口道闸自动关闭倒计时
    if(entrance_close_timer > 0)
    {
        entrance_close_timer--;
    }

    // 出口道闸自动关闭倒计时
    if(exit_close_timer > 0)
    {
        exit_close_timer--;
    }
}
