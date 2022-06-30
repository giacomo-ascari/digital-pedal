################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/asky/Desktop/digital-pedal/gt_common/Src/commander.c \
/home/asky/Desktop/digital-pedal/gt_common/Src/mode.c \
/home/asky/Desktop/digital-pedal/gt_common/Src/pedalboard.c 

OBJS += \
./gt_common/Src/commander.o \
./gt_common/Src/mode.o \
./gt_common/Src/pedalboard.o 

C_DEPS += \
./gt_common/Src/commander.d \
./gt_common/Src/mode.d \
./gt_common/Src/pedalboard.d 


# Each subdirectory must supply rules for building sources it contributes
gt_common/Src/commander.o: /home/asky/Desktop/digital-pedal/gt_common/Src/commander.c gt_common/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -DF103 -c -I../Core/Inc -I"/home/asky/Desktop/digital-pedal/gt_common/Inc" -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
gt_common/Src/mode.o: /home/asky/Desktop/digital-pedal/gt_common/Src/mode.c gt_common/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -DF103 -c -I../Core/Inc -I"/home/asky/Desktop/digital-pedal/gt_common/Inc" -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
gt_common/Src/pedalboard.o: /home/asky/Desktop/digital-pedal/gt_common/Src/pedalboard.c gt_common/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -DF103 -c -I../Core/Inc -I"/home/asky/Desktop/digital-pedal/gt_common/Inc" -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-gt_common-2f-Src

clean-gt_common-2f-Src:
	-$(RM) ./gt_common/Src/commander.d ./gt_common/Src/commander.o ./gt_common/Src/commander.su ./gt_common/Src/mode.d ./gt_common/Src/mode.o ./gt_common/Src/mode.su ./gt_common/Src/pedalboard.d ./gt_common/Src/pedalboard.o ./gt_common/Src/pedalboard.su

.PHONY: clean-gt_common-2f-Src

