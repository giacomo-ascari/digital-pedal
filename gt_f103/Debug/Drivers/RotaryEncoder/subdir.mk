################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/RotaryEncoder/reconder.c 

OBJS += \
./Drivers/RotaryEncoder/reconder.o 

C_DEPS += \
./Drivers/RotaryEncoder/reconder.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/RotaryEncoder/%.o Drivers/RotaryEncoder/%.su: ../Drivers/RotaryEncoder/%.c Drivers/RotaryEncoder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/Commander -I../Drivers/Painter -I../Drivers/EPDDriver -I../Drivers/RotaryEncoder -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-RotaryEncoder

clean-Drivers-2f-RotaryEncoder:
	-$(RM) ./Drivers/RotaryEncoder/reconder.d ./Drivers/RotaryEncoder/reconder.o ./Drivers/RotaryEncoder/reconder.su

.PHONY: clean-Drivers-2f-RotaryEncoder

