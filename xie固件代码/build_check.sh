#!/bin/bash

###############################################
# 智能停车场系统 - 编译验证脚本
###############################################

echo "======================================"
echo "  智能停车场系统 - 项目检查"
echo "======================================"
echo ""

# 检查项目结构
echo "1. 检查项目结构..."
REQUIRED_DIRS=(
    "USER"
    "HARDWARE/GPIO"
    "HARDWARE/KEY"
    "HARDWARE/MOTOR_DUOJI"
    "HARDWARE/OLED"
    "HARDWARE/ALARM"
    "SYSTEM/sys"
    "SYSTEM/delay"
    "SYSTEM/usart"
    "STM32F10x_FWLib/inc"
    "STM32F10x_FWLib/src"
    "CORE"
)

for dir in "${REQUIRED_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        echo "  ✓ $dir"
    else
        echo "  ✗ $dir - 缺失"
        exit 1
    fi
done

echo ""
echo "2. 检查关键源文件..."
REQUIRED_FILES=(
    "USER/main.c"
    "USER/stm32f10x_it.c"
    "USER/system_stm32f10x.c"
    "HARDWARE/GPIO/gpio.c"
    "HARDWARE/KEY/key.c"
    "HARDWARE/MOTOR_DUOJI/motor_duoji.c"
    "HARDWARE/OLED/oled.c"
    "HARDWARE/ALARM/alarm.c"
    "SYSTEM/sys/sys.c"
    "SYSTEM/delay/delay.c"
)

for file in "${REQUIRED_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file - 缺失"
        exit 1
    fi
done

echo ""
echo "3. 统计代码行数..."
echo "  C源文件数量: $(find . -name "*.c" | wc -l)"
echo "  H头文件数量: $(find . -name "*.h" | wc -l)"
echo "  总代码行数: $(find . -name "*.c" -o -name "*.h" | xargs wc -l | tail -1)"

echo ""
echo "4. 检查编译配置文件..."
if [ -f "Makefile" ]; then
    echo "  ✓ Makefile 存在"
else
    echo "  ✗ Makefile - 缺失"
fi

echo ""
echo "======================================"
echo "  项目检查完成！"
echo "======================================"
echo ""
echo "注意事项："
echo "1. 本项目需要 Keil MDK 或 ARM GCC 工具链进行编译"
echo "2. 确保已安装 STM32F10x 系列芯片支持包"
echo "3. 编译前请检查 Makefile 中的工具链路径"
echo "4. 如使用 Keil，请创建新工程并添加所有源文件"
echo ""
echo "快速开始："
echo "  - 使用 Keil: 打开 Keil uVision，创建新工程并添加源文件"
echo "  - 使用 Make: 运行 'make' 命令（需要 ARM GCC 工具链）"
echo ""
