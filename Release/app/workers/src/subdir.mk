################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/workers/src/lcd_worker.c \
../app/workers/src/led_worker.c \
../app/workers/src/netx_worker.c 

OBJS += \
./app/workers/src/lcd_worker.o \
./app/workers/src/led_worker.o \
./app/workers/src/netx_worker.o 

C_DEPS += \
./app/workers/src/lcd_worker.d \
./app/workers/src/led_worker.d \
./app/workers/src/netx_worker.d 


# Each subdirectory must supply rules for building sources it contributes
app/workers/src/%.o app/workers/src/%.su app/workers/src/%.cyclo: ../app/workers/src/%.c app/workers/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -DCIFX_TOOLKIT_HWIF -c -I../Core/Inc -I"D:/Workspace/stm32/STXBoard/peripherie/lcd/inc" -I"D:/Workspace/stm32/STXBoard/peripherie/led/inc" -I"D:/Workspace/stm32/STXBoard/app/manager/inc" -I"D:/Workspace/stm32/STXBoard/app/workers/inc" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/cifXAPI" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/HilscherDefinitions" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/OSAbstraction" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/SerialDPM" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Source" -I"D:/Workspace/stm32/STXBoard/peripherie/lcd/inc" -I"D:/Workspace/stm32/STXBoard/FreeRTOS/Include" -I"D:/Workspace/stm32/STXBoard/FreeRTOS/portable/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Workspace/stm32/STXBoard/app/workers/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app-2f-workers-2f-src

clean-app-2f-workers-2f-src:
	-$(RM) ./app/workers/src/lcd_worker.cyclo ./app/workers/src/lcd_worker.d ./app/workers/src/lcd_worker.o ./app/workers/src/lcd_worker.su ./app/workers/src/led_worker.cyclo ./app/workers/src/led_worker.d ./app/workers/src/led_worker.o ./app/workers/src/led_worker.su ./app/workers/src/netx_worker.cyclo ./app/workers/src/netx_worker.d ./app/workers/src/netx_worker.o ./app/workers/src/netx_worker.su

.PHONY: clean-app-2f-workers-2f-src

