################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/manager/src/app_manager.c 

OBJS += \
./app/manager/src/app_manager.o 

C_DEPS += \
./app/manager/src/app_manager.d 


# Each subdirectory must supply rules for building sources it contributes
app/manager/src/%.o app/manager/src/%.su app/manager/src/%.cyclo: ../app/manager/src/%.c app/manager/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -DCIFX_TOOLKIT_HWIF -c -I../Core/Inc -I"C:/Projekte/stm32/STXBoard/peripherie/neopixel/inc" -I"C:/Projekte/stm32/STXBoard/app/config/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/DPMCommunication/inc" -I"C:/Projekte/stm32/STXBoard/profinet/inc/PNS_API" -I"C:/Projekte/stm32/STXBoard/profinet/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/lcd/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/led/inc" -I"C:/Projekte/stm32/STXBoard/app/manager/inc" -I"C:/Projekte/stm32/STXBoard/app/workers/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/cifXAPI" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/HilscherDefinitions" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/OSAbstraction" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/SerialDPM" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Source" -I"C:/Projekte/stm32/STXBoard/FreeRTOS/Include" -I"C:/Projekte/stm32/STXBoard/FreeRTOS/portable/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Projekte/stm32/STXBoard/app/workers/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-manager-2f-src

clean-app-2f-manager-2f-src:
	-$(RM) ./app/manager/src/app_manager.cyclo ./app/manager/src/app_manager.d ./app/manager/src/app_manager.o ./app/manager/src/app_manager.su

.PHONY: clean-app-2f-manager-2f-src

