################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../peripherie/netx90/cifXToolkit/SerialDPM/SerialDPMInterface.c 

OBJS += \
./peripherie/netx90/cifXToolkit/SerialDPM/SerialDPMInterface.o 

C_DEPS += \
./peripherie/netx90/cifXToolkit/SerialDPM/SerialDPMInterface.d 


# Each subdirectory must supply rules for building sources it contributes
peripherie/netx90/cifXToolkit/SerialDPM/%.o peripherie/netx90/cifXToolkit/SerialDPM/%.su peripherie/netx90/cifXToolkit/SerialDPM/%.cyclo: ../peripherie/netx90/cifXToolkit/SerialDPM/%.c peripherie/netx90/cifXToolkit/SerialDPM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -DCIFX_TOOLKIT_HWIF -c -I../Core/Inc -I"D:/Workspace/stm32/STXBoard/peripherie/lcd/inc" -I"D:/Workspace/stm32/STXBoard/peripherie/led/inc" -I"D:/Workspace/stm32/STXBoard/app/manager/inc" -I"D:/Workspace/stm32/STXBoard/app/workers/inc" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/cifXAPI" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/HilscherDefinitions" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/OSAbstraction" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/SerialDPM" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Source" -I"D:/Workspace/stm32/STXBoard/peripherie/lcd/inc" -I"D:/Workspace/stm32/STXBoard/FreeRTOS/Include" -I"D:/Workspace/stm32/STXBoard/FreeRTOS/portable/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Workspace/stm32/STXBoard/app/workers/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-peripherie-2f-netx90-2f-cifXToolkit-2f-SerialDPM

clean-peripherie-2f-netx90-2f-cifXToolkit-2f-SerialDPM:
	-$(RM) ./peripherie/netx90/cifXToolkit/SerialDPM/SerialDPMInterface.cyclo ./peripherie/netx90/cifXToolkit/SerialDPM/SerialDPMInterface.d ./peripherie/netx90/cifXToolkit/SerialDPM/SerialDPMInterface.o ./peripherie/netx90/cifXToolkit/SerialDPM/SerialDPMInterface.su

.PHONY: clean-peripherie-2f-netx90-2f-cifXToolkit-2f-SerialDPM

