################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Pedalboard/pedalboard.c 

OBJS += \
./Drivers/Pedalboard/pedalboard.o 

C_DEPS += \
./Drivers/Pedalboard/pedalboard.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Pedalboard/%.o Drivers/Pedalboard/%.su: ../Drivers/Pedalboard/%.c Drivers/Pedalboard/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/Commander -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Drivers/Pedalboard -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Pedalboard

clean-Drivers-2f-Pedalboard:
	-$(RM) ./Drivers/Pedalboard/pedalboard.d ./Drivers/Pedalboard/pedalboard.o ./Drivers/Pedalboard/pedalboard.su

.PHONY: clean-Drivers-2f-Pedalboard

