######################################
# 智能停车场系统 Makefile
# 基于 STM32F103C8T6
######################################

# 目标名称
TARGET = parking_system

# 工具链定义
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

# MCU定义
CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# 宏定义
C_DEFS = \
-DSTM32F10X_MD \
-DUSE_STDPERIPH_DRIVER

# 头文件路径
C_INCLUDES = \
-IUSER \
-ICORE \
-ISYSTEM/sys \
-ISYSTEM/delay \
-ISYSTEM/usart \
-IHARDWARE/GPIO \
-IHARDWARE/KEY \
-IHARDWARE/MOTOR_DUOJI \
-IHARDWARE/OLED \
-IHARDWARE/ALARM \
-ISTM32F10x_FWLib/inc

# 编译选项
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) -Wall -fdata-sections -ffunction-sections
CFLAGS += -g -gdwarf-2

# 链接选项
LDSCRIPT = STM32F103C8Tx_FLASH.ld
LIBS = -lc -lm -lnosys
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) -Wl,-Map=$(TARGET).map,--cref -Wl,--gc-sections

# 源文件
C_SOURCES = \
USER/main.c \
USER/stm32f10x_it.c \
USER/system_stm32f10x.c \
SYSTEM/sys/sys.c \
SYSTEM/delay/delay.c \
SYSTEM/usart/usart.c \
HARDWARE/GPIO/gpio.c \
HARDWARE/KEY/key.c \
HARDWARE/MOTOR_DUOJI/motor_duoji.c \
HARDWARE/OLED/oled.c \
HARDWARE/ALARM/alarm.c \
STM32F10x_FWLib/src/stm32f10x_gpio.c \
STM32F10x_FWLib/src/stm32f10x_rcc.c \
STM32F10x_FWLib/src/stm32f10x_tim.c \
STM32F10x_FWLib/src/stm32f10x_usart.c \
STM32F10x_FWLib/src/misc.c

# 启动文件
ASM_SOURCES = CORE/startup_stm32f10x_md.s

# 构建目录
BUILD_DIR = build

# 对象文件
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# 默认目标
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# 编译规则
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(CP) -O ihex $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(CP) -O binary -S $< $@

$(BUILD_DIR):
	mkdir -p $@

# 清理
clean:
	-rm -fR $(BUILD_DIR)

# 依赖
-include $(wildcard $(BUILD_DIR)/*.d)
