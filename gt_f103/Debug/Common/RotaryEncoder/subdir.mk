################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/asky/Desktop/digital-pedal/gt_common/RotaryEncoder/reconder.c 

OBJS += \
./Common/RotaryEncoder/reconder.o 

C_DEPS += \
./Common/RotaryEncoder/reconder.d 


# Each subdirectory must supply rules for building sources it contributes
Common/RotaryEncoder/reconder.o: /home/asky/Desktop/digital-pedal/gt_common/RotaryEncoder/reconder.c Common/RotaryEncoder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Common/Menu -I../Common/Painter2 -I../Common/RotaryEncoder -I../Common/EPDDriver -I../Common/Painter -I../Common/Commander -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Common-2f-RotaryEncoder

clean-Common-2f-RotaryEncoder:
	-$(RM) ./Common/RotaryEncoder/reconder.d ./Common/RotaryEncoder/reconder.o ./Common/RotaryEncoder/reconder.su

.PHONY: clean-Common-2f-RotaryEncoder

