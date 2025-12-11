################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../peripherie/netx90/cifXToolkit/Source/Hilcrc32.c \
../peripherie/netx90/cifXToolkit/Source/Hilmd5.c \
../peripherie/netx90/cifXToolkit/Source/cifXDownload.c \
../peripherie/netx90/cifXToolkit/Source/cifXEndianess.c \
../peripherie/netx90/cifXToolkit/Source/cifXFunctions.c \
../peripherie/netx90/cifXToolkit/Source/cifXHWFunctions.c \
../peripherie/netx90/cifXToolkit/Source/cifXInit.c \
../peripherie/netx90/cifXToolkit/Source/cifXInterrupt.c \
../peripherie/netx90/cifXToolkit/Source/netX5x_hboot.c \
../peripherie/netx90/cifXToolkit/Source/netX5xx_hboot.c \
../peripherie/netx90/cifXToolkit/Source/netX90_netX4x00.c 

OBJS += \
./peripherie/netx90/cifXToolkit/Source/Hilcrc32.o \
./peripherie/netx90/cifXToolkit/Source/Hilmd5.o \
./peripherie/netx90/cifXToolkit/Source/cifXDownload.o \
./peripherie/netx90/cifXToolkit/Source/cifXEndianess.o \
./peripherie/netx90/cifXToolkit/Source/cifXFunctions.o \
./peripherie/netx90/cifXToolkit/Source/cifXHWFunctions.o \
./peripherie/netx90/cifXToolkit/Source/cifXInit.o \
./peripherie/netx90/cifXToolkit/Source/cifXInterrupt.o \
./peripherie/netx90/cifXToolkit/Source/netX5x_hboot.o \
./peripherie/netx90/cifXToolkit/Source/netX5xx_hboot.o \
./peripherie/netx90/cifXToolkit/Source/netX90_netX4x00.o 

C_DEPS += \
./peripherie/netx90/cifXToolkit/Source/Hilcrc32.d \
./peripherie/netx90/cifXToolkit/Source/Hilmd5.d \
./peripherie/netx90/cifXToolkit/Source/cifXDownload.d \
./peripherie/netx90/cifXToolkit/Source/cifXEndianess.d \
./peripherie/netx90/cifXToolkit/Source/cifXFunctions.d \
./peripherie/netx90/cifXToolkit/Source/cifXHWFunctions.d \
./peripherie/netx90/cifXToolkit/Source/cifXInit.d \
./peripherie/netx90/cifXToolkit/Source/cifXInterrupt.d \
./peripherie/netx90/cifXToolkit/Source/netX5x_hboot.d \
./peripherie/netx90/cifXToolkit/Source/netX5xx_hboot.d \
./peripherie/netx90/cifXToolkit/Source/netX90_netX4x00.d 


# Each subdirectory must supply rules for building sources it contributes
peripherie/netx90/cifXToolkit/Source/%.o peripherie/netx90/cifXToolkit/Source/%.su peripherie/netx90/cifXToolkit/Source/%.cyclo: ../peripherie/netx90/cifXToolkit/Source/%.c peripherie/netx90/cifXToolkit/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -DCIFX_TOOLKIT_HWIF -c -I../Core/Inc -I"C:/Projekte/stm32/STXBoard/peripherie/neopixel/inc" -I"C:/Projekte/stm32/STXBoard/app/config/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/DPMCommunication/inc" -I"C:/Projekte/stm32/STXBoard/profinet/inc/PNS_API" -I"C:/Projekte/stm32/STXBoard/profinet/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/lcd/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/led/inc" -I"C:/Projekte/stm32/STXBoard/app/manager/inc" -I"C:/Projekte/stm32/STXBoard/app/workers/inc" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/cifXAPI" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/HilscherDefinitions" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/OSAbstraction" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/SerialDPM" -I"C:/Projekte/stm32/STXBoard/peripherie/netx90/cifXToolkit/Source" -I"C:/Projekte/stm32/STXBoard/FreeRTOS/Include" -I"C:/Projekte/stm32/STXBoard/FreeRTOS/portable/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Projekte/stm32/STXBoard/app/workers/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-peripherie-2f-netx90-2f-cifXToolkit-2f-Source

clean-peripherie-2f-netx90-2f-cifXToolkit-2f-Source:
	-$(RM) ./peripherie/netx90/cifXToolkit/Source/Hilcrc32.cyclo ./peripherie/netx90/cifXToolkit/Source/Hilcrc32.d ./peripherie/netx90/cifXToolkit/Source/Hilcrc32.o ./peripherie/netx90/cifXToolkit/Source/Hilcrc32.su ./peripherie/netx90/cifXToolkit/Source/Hilmd5.cyclo ./peripherie/netx90/cifXToolkit/Source/Hilmd5.d ./peripherie/netx90/cifXToolkit/Source/Hilmd5.o ./peripherie/netx90/cifXToolkit/Source/Hilmd5.su ./peripherie/netx90/cifXToolkit/Source/cifXDownload.cyclo ./peripherie/netx90/cifXToolkit/Source/cifXDownload.d ./peripherie/netx90/cifXToolkit/Source/cifXDownload.o ./peripherie/netx90/cifXToolkit/Source/cifXDownload.su ./peripherie/netx90/cifXToolkit/Source/cifXEndianess.cyclo ./peripherie/netx90/cifXToolkit/Source/cifXEndianess.d ./peripherie/netx90/cifXToolkit/Source/cifXEndianess.o ./peripherie/netx90/cifXToolkit/Source/cifXEndianess.su ./peripherie/netx90/cifXToolkit/Source/cifXFunctions.cyclo ./peripherie/netx90/cifXToolkit/Source/cifXFunctions.d ./peripherie/netx90/cifXToolkit/Source/cifXFunctions.o ./peripherie/netx90/cifXToolkit/Source/cifXFunctions.su ./peripherie/netx90/cifXToolkit/Source/cifXHWFunctions.cyclo ./peripherie/netx90/cifXToolkit/Source/cifXHWFunctions.d ./peripherie/netx90/cifXToolkit/Source/cifXHWFunctions.o ./peripherie/netx90/cifXToolkit/Source/cifXHWFunctions.su ./peripherie/netx90/cifXToolkit/Source/cifXInit.cyclo ./peripherie/netx90/cifXToolkit/Source/cifXInit.d ./peripherie/netx90/cifXToolkit/Source/cifXInit.o ./peripherie/netx90/cifXToolkit/Source/cifXInit.su ./peripherie/netx90/cifXToolkit/Source/cifXInterrupt.cyclo ./peripherie/netx90/cifXToolkit/Source/cifXInterrupt.d ./peripherie/netx90/cifXToolkit/Source/cifXInterrupt.o ./peripherie/netx90/cifXToolkit/Source/cifXInterrupt.su ./peripherie/netx90/cifXToolkit/Source/netX5x_hboot.cyclo ./peripherie/netx90/cifXToolkit/Source/netX5x_hboot.d ./peripherie/netx90/cifXToolkit/Source/netX5x_hboot.o ./peripherie/netx90/cifXToolkit/Source/netX5x_hboot.su ./peripherie/netx90/cifXToolkit/Source/netX5xx_hboot.cyclo ./peripherie/netx90/cifXToolkit/Source/netX5xx_hboot.d ./peripherie/netx90/cifXToolkit/Source/netX5xx_hboot.o ./peripherie/netx90/cifXToolkit/Source/netX5xx_hboot.su ./peripherie/netx90/cifXToolkit/Source/netX90_netX4x00.cyclo ./peripherie/netx90/cifXToolkit/Source/netX90_netX4x00.d ./peripherie/netx90/cifXToolkit/Source/netX90_netX4x00.o ./peripherie/netx90/cifXToolkit/Source/netX90_netX4x00.su

.PHONY: clean-peripherie-2f-netx90-2f-cifXToolkit-2f-Source

