################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS/MemMang/heap_4.c 

OBJS += \
./FreeRTOS/MemMang/heap_4.o 

C_DEPS += \
./FreeRTOS/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/MemMang/%.o: ../FreeRTOS/MemMang/%.c FreeRTOS/MemMang/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F407xx -c -I../Core/Inc -I"C:/Users/rong_he/STM32CubeIDE/workspace_1.8.0/fat/FreeRTOS/include" -I"C:/Users/rong_he/STM32CubeIDE/workspace_1.8.0/fat/FreeRTOS/portable/ARM_CM4F" -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FreeRTOS-2f-MemMang

clean-FreeRTOS-2f-MemMang:
	-$(RM) ./FreeRTOS/MemMang/heap_4.d ./FreeRTOS/MemMang/heap_4.o

.PHONY: clean-FreeRTOS-2f-MemMang

