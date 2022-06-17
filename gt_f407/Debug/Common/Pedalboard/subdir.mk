################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/asky/Desktop/digital-pedal/gt_common/Pedalboard/pedalboard.c 

OBJS += \
./Common/Pedalboard/pedalboard.o 

C_DEPS += \
./Common/Pedalboard/pedalboard.d 


# Each subdirectory must supply rules for building sources it contributes
Common/Pedalboard/pedalboard.o: /home/asky/Desktop/digital-pedal/gt_common/Pedalboard/pedalboard.c Common/Pedalboard/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Common/Commander -I../Common/Pedalboard -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-Pedalboard

clean-Common-2f-Pedalboard:
	-$(RM) ./Common/Pedalboard/pedalboard.d ./Common/Pedalboard/pedalboard.o ./Common/Pedalboard/pedalboard.su

.PHONY: clean-Common-2f-Pedalboard

