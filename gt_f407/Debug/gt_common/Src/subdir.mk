################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/giaco/Desktop/digital-pedal/gt_common/Src/commander.c \
C:/Users/giaco/Desktop/digital-pedal/gt_common/Src/pedalboard.c 

OBJS += \
./gt_common/Src/commander.o \
./gt_common/Src/pedalboard.o 

C_DEPS += \
./gt_common/Src/commander.d \
./gt_common/Src/pedalboard.d 


# Each subdirectory must supply rules for building sources it contributes
gt_common/Src/commander.o: C:/Users/giaco/Desktop/digital-pedal/gt_common/Src/commander.c gt_common/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DF407 -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I"C:/Users/giaco/Desktop/digital-pedal/gt_common/Inc" -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
gt_common/Src/pedalboard.o: C:/Users/giaco/Desktop/digital-pedal/gt_common/Src/pedalboard.c gt_common/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DF407 -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I"C:/Users/giaco/Desktop/digital-pedal/gt_common/Inc" -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-gt_common-2f-Src

clean-gt_common-2f-Src:
	-$(RM) ./gt_common/Src/commander.d ./gt_common/Src/commander.o ./gt_common/Src/commander.su ./gt_common/Src/pedalboard.d ./gt_common/Src/pedalboard.o ./gt_common/Src/pedalboard.su

.PHONY: clean-gt_common-2f-Src

