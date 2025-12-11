################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../peripherie/netx90/DPMCommunication/src/PacketCommunication.c \
../peripherie/netx90/DPMCommunication/src/SystemPackets.c 

OBJS += \
./peripherie/netx90/DPMCommunication/src/PacketCommunication.o \
./peripherie/netx90/DPMCommunication/src/SystemPackets.o 

C_DEPS += \
./peripherie/netx90/DPMCommunication/src/PacketCommunication.d \
./peripherie/netx90/DPMCommunication/src/SystemPackets.d 


# Each subdirectory must supply rules for building sources it contributes
peripherie/netx90/DPMCommunication/src/%.o peripherie/netx90/DPMCommunication/src/%.su peripherie/netx90/DPMCommunication/src/%.cyclo: ../peripherie/netx90/DPMCommunication/src/%.c peripherie/netx90/DPMCommunication/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -DCIFX_TOOLKIT_HWIF -c -I../Core/Inc -I"C:/Projekte/stm32/STXBoard/peripherie/neopixel/inc" -I"C:/Projekte/stm32/STXBoard/app/config/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/DPMCommunication/inc" -I"C:/Projekte/stm32/STXBoard/profinet/inc/PNS_API" -I"C:/Projekte/stm32/STXBoard/profinet/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/lcd/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/led/inc" -I"C:/Projekte/stm32/STXBoard/app/manager/inc" -I"C:/Projekte/stm32/STXBoard/app/workers/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/cifXAPI" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/HilscherDefinitions" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/OSAbstraction" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/SerialDPM" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Source" -I"C:/Projekte/stm32/STXBoard/FreeRTOS/Include" -I"C:/Projekte/stm32/STXBoard/FreeRTOS/portable/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Projekte/stm32/STXBoard/app/workers/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-peripherie-2f-netx90-2f-DPMCommunication-2f-src

clean-peripherie-2f-netx90-2f-DPMCommunication-2f-src:
	-$(RM) ./peripherie/netx90/DPMCommunication/src/PacketCommunication.cyclo ./peripherie/netx90/DPMCommunication/src/PacketCommunication.d ./peripherie/netx90/DPMCommunication/src/PacketCommunication.o ./peripherie/netx90/DPMCommunication/src/PacketCommunication.su ./peripherie/netx90/DPMCommunication/src/SystemPackets.cyclo ./peripherie/netx90/DPMCommunication/src/SystemPackets.d ./peripherie/netx90/DPMCommunication/src/SystemPackets.o ./peripherie/netx90/DPMCommunication/src/SystemPackets.su

.PHONY: clean-peripherie-2f-netx90-2f-DPMCommunication-2f-src

