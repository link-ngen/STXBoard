################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include profinet/src/subdir.mk
-include peripherie/netx90/cifXToolkit/User/subdir.mk
-include peripherie/netx90/cifXToolkit/Source/subdir.mk
-include peripherie/netx90/cifXToolkit/SerialDPM/subdir.mk
-include peripherie/netx90/cifXToolkit/OSAbstraction/subdir.mk
-include peripherie/netx90/DPMCommunication/src/subdir.mk
-include peripherie/neopixel/src/subdir.mk
-include peripherie/led/src/subdir.mk
-include peripherie/lcd/src/subdir.mk
-include app/workers/src/subdir.mk
-include app/manager/src/subdir.mk
-include FreeRTOS/portable/ARM_CM4F/subdir.mk
-include FreeRTOS/Source/MemMang/subdir.mk
-include FreeRTOS/Source/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := STXBoard
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STXBoard.elf \

MAP_FILES += \
STXBoard.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STXBoard.list \


# All Target
all: main-build

# Main-build Target
main-build: STXBoard.elf secondary-outputs

# Tool invocations
STXBoard.elf STXBoard.map: $(OBJS) $(USER_OBJS) C:\Users\nnguyen\Projects\stm32\STXBoard\STM32F411CEUX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "STXBoard.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\nnguyen\Projects\stm32\STXBoard\STM32F411CEUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="STXBoard.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STXBoard.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STXBoard.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STXBoard.elf STXBoard.list STXBoard.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
