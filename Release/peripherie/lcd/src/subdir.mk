################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../peripherie/lcd/src/ssd1306.c \
../peripherie/lcd/src/ssd1306_fonts.c \
../peripherie/lcd/src/ssd1306_gfx.c 

OBJS += \
./peripherie/lcd/src/ssd1306.o \
./peripherie/lcd/src/ssd1306_fonts.o \
./peripherie/lcd/src/ssd1306_gfx.o 

C_DEPS += \
./peripherie/lcd/src/ssd1306.d \
./peripherie/lcd/src/ssd1306_fonts.d \
./peripherie/lcd/src/ssd1306_gfx.d 


# Each subdirectory must supply rules for building sources it contributes
peripherie/lcd/src/%.o peripherie/lcd/src/%.su peripherie/lcd/src/%.cyclo: ../peripherie/lcd/src/%.c peripherie/lcd/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -DCIFX_TOOLKIT_HWIF -c -I../Core/Inc -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/neopixel/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/app/config/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/DPMCommunication/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/profinet/inc/PNS_API" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/profinet/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/lcd/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/led/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/app/manager/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/app/workers/inc" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/cifXAPI" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/Common/HilscherDefinitions" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/OSAbstraction" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/SerialDPM" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/peripherie/netx90/cifXToolkit/Source" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/FreeRTOS/Include" -I"C:/Users/nnguyen/Projects/stm32/STXBoard/FreeRTOS/portable/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/nnguyen/Projects/stm32/STXBoard/app/workers/inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-peripherie-2f-lcd-2f-src

clean-peripherie-2f-lcd-2f-src:
	-$(RM) ./peripherie/lcd/src/ssd1306.cyclo ./peripherie/lcd/src/ssd1306.d ./peripherie/lcd/src/ssd1306.o ./peripherie/lcd/src/ssd1306.su ./peripherie/lcd/src/ssd1306_fonts.cyclo ./peripherie/lcd/src/ssd1306_fonts.d ./peripherie/lcd/src/ssd1306_fonts.o ./peripherie/lcd/src/ssd1306_fonts.su ./peripherie/lcd/src/ssd1306_gfx.cyclo ./peripherie/lcd/src/ssd1306_gfx.d ./peripherie/lcd/src/ssd1306_gfx.o ./peripherie/lcd/src/ssd1306_gfx.su

.PHONY: clean-peripherie-2f-lcd-2f-src

