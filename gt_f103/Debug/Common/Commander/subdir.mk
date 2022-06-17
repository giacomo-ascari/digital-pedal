################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/asky/Desktop/digital-pedal/gt_common/Commander/commander.c 

OBJS += \
./Common/Commander/commander.o 

C_DEPS += \
./Common/Commander/commander.d 


# Each subdirectory must supply rules for building sources it contributes
Common/Commander/commander.o: /home/asky/Desktop/digital-pedal/gt_common/Commander/commander.c Common/Commander/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Common/Menu -I../Common/Painter2 -I../Common/RotaryEncoder -I../Common/EPDDriver -I../Common/Painter -I../Common/Commander -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Common-2f-Commander

clean-Common-2f-Commander:
	-$(RM) ./Common/Commander/commander.d ./Common/Commander/commander.o ./Common/Commander/commander.su

.PHONY: clean-Common-2f-Commander

