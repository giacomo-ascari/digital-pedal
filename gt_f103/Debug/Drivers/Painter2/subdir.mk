################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Painter2/painter2.c 

OBJS += \
./Drivers/Painter2/painter2.o 

C_DEPS += \
./Drivers/Painter2/painter2.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Painter2/%.o Drivers/Painter2/%.su: ../Drivers/Painter2/%.c Drivers/Painter2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/Commander -I../Drivers/Painter -I../Drivers/EPDDriver -I../Drivers/RotaryEncoder -I../Drivers/Painter2 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Painter2

clean-Drivers-2f-Painter2:
	-$(RM) ./Drivers/Painter2/painter2.d ./Drivers/Painter2/painter2.o ./Drivers/Painter2/painter2.su

.PHONY: clean-Drivers-2f-Painter2

