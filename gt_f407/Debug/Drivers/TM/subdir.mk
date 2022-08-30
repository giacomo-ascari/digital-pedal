################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/TM/tm_stm32f4_fft.c 

OBJS += \
./Drivers/TM/tm_stm32f4_fft.o 

C_DEPS += \
./Drivers/TM/tm_stm32f4_fft.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/TM/%.o Drivers/TM/%.su: ../Drivers/TM/%.c Drivers/TM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DF407 -c -I../Core/Inc -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I"C:/Users/giaco/Desktop/digital-pedal/gt_common/Inc" -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Drivers/TM -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-TM

clean-Drivers-2f-TM:
	-$(RM) ./Drivers/TM/tm_stm32f4_fft.d ./Drivers/TM/tm_stm32f4_fft.o ./Drivers/TM/tm_stm32f4_fft.su

.PHONY: clean-Drivers-2f-TM

