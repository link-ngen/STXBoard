################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../peripherie/netx90/DPMCommunication/src/PacketCommunication.c 

OBJS += \
./peripherie/netx90/DPMCommunication/src/PacketCommunication.o 

C_DEPS += \
./peripherie/netx90/DPMCommunication/src/PacketCommunication.d 


# Each subdirectory must supply rules for building sources it contributes
peripherie/netx90/DPMCommunication/src/%.o peripherie/netx90/DPMCommunication/src/%.su peripherie/netx90/DPMCommunication/src/%.cyclo: ../peripherie/netx90/DPMCommunication/src/%.c peripherie/netx90/DPMCommunication/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -DCIFX_TOOLKIT_HWIF -c -I../Core/Inc -I"D:/Workspace/stm32/STXBoard/peripherie/lcd/inc" -I"D:/Workspace/stm32/STXBoard/peripherie/led/inc" -I"D:/Workspace/stm32/STXBoard/app/manager/inc" -I"D:/Workspace/stm32/STXBoard/app/workers/inc" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/cifXAPI" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/HilscherDefinitions" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/OSAbstraction" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/SerialDPM" -I"D:/Workspace/stm32/STXBoard/peripherie/netx90/cifXToolkit/Source" -I"D:/Workspace/stm32/STXBoard/peripherie/lcd/inc" -I"D:/Workspace/stm32/STXBoard/FreeRTOS/Include" -I"D:/Workspace/stm32/STXBoard/FreeRTOS/portable/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/Workspace/stm32/STXBoard/app/workers/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-peripherie-2f-netx90-2f-DPMCommunication-2f-src

clean-peripherie-2f-netx90-2f-DPMCommunication-2f-src:
	-$(RM) ./peripherie/netx90/DPMCommunication/src/PacketCommunication.cyclo ./peripherie/netx90/DPMCommunication/src/PacketCommunication.d ./peripherie/netx90/DPMCommunication/src/PacketCommunication.o ./peripherie/netx90/DPMCommunication/src/PacketCommunication.su

.PHONY: clean-peripherie-2f-netx90-2f-DPMCommunication-2f-src

