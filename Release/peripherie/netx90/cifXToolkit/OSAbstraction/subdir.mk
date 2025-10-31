################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../peripherie/netx90/cifXToolkit/OSAbstraction/OS_Custom.c \
../peripherie/netx90/cifXToolkit/OSAbstraction/OS_SPICustom.c 

OBJS += \
./peripherie/netx90/cifXToolkit/OSAbstraction/OS_Custom.o \
./peripherie/netx90/cifXToolkit/OSAbstraction/OS_SPICustom.o 

C_DEPS += \
./peripherie/netx90/cifXToolkit/OSAbstraction/OS_Custom.d \
./peripherie/netx90/cifXToolkit/OSAbstraction/OS_SPICustom.d 


# Each subdirectory must supply rules for building sources it contributes
peripherie/netx90/cifXToolkit/OSAbstraction/%.o peripherie/netx90/cifXToolkit/OSAbstraction/%.su peripherie/netx90/cifXToolkit/OSAbstraction/%.cyclo: ../peripherie/netx90/cifXToolkit/OSAbstraction/%.c peripherie/netx90/cifXToolkit/OSAbstraction/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -DCIFX_TOOLKIT_HWIF -c -I../Core/Inc -I"C:/Users/nnguyen/Projects/stm32/STXBoard/app/config/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/DPMCommunication/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/profinet/inc/PNS_API" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/profinet/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/lcd/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/led/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/app/manager/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/app/workers/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/cifXAPI" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/HilscherDefinitions" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/OSAbstraction" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/SerialDPM" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/Source" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/FreeRTOS/Include" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/FreeRTOS/portable/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/nnguyen/Projects/stm32/STXBoard/app/workers/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-peripherie-2f-netx90-2f-cifXToolkit-2f-OSAbstraction

clean-peripherie-2f-netx90-2f-cifXToolkit-2f-OSAbstraction:
	-$(RM) ./peripherie/netx90/cifXToolkit/OSAbstraction/OS_Custom.cyclo ./peripherie/netx90/cifXToolkit/OSAbstraction/OS_Custom.d ./peripherie/netx90/cifXToolkit/OSAbstraction/OS_Custom.o ./peripherie/netx90/cifXToolkit/OSAbstraction/OS_Custom.su ./peripherie/netx90/cifXToolkit/OSAbstraction/OS_SPICustom.cyclo ./peripherie/netx90/cifXToolkit/OSAbstraction/OS_SPICustom.d ./peripherie/netx90/cifXToolkit/OSAbstraction/OS_SPICustom.o ./peripherie/netx90/cifXToolkit/OSAbstraction/OS_SPICustom.su

.PHONY: clean-peripherie-2f-netx90-2f-cifXToolkit-2f-OSAbstraction

