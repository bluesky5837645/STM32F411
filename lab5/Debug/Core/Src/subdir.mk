################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/AUDIO.c \
../Core/Src/AUDIO_LINK.c \
../Core/Src/File_Handling.c \
../Core/Src/cs43l22.c \
../Core/Src/diskio.c \
../Core/Src/fatfs.c \
../Core/Src/ff.c \
../Core/Src/ff_gen_drv.c \
../Core/Src/fops.c \
../Core/Src/main.c \
../Core/Src/mmc_sd.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/user_diskio.c \
../Core/Src/waveplayer.c 

OBJS += \
./Core/Src/AUDIO.o \
./Core/Src/AUDIO_LINK.o \
./Core/Src/File_Handling.o \
./Core/Src/cs43l22.o \
./Core/Src/diskio.o \
./Core/Src/fatfs.o \
./Core/Src/ff.o \
./Core/Src/ff_gen_drv.o \
./Core/Src/fops.o \
./Core/Src/main.o \
./Core/Src/mmc_sd.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/user_diskio.o \
./Core/Src/waveplayer.o 

C_DEPS += \
./Core/Src/AUDIO.d \
./Core/Src/AUDIO_LINK.d \
./Core/Src/File_Handling.d \
./Core/Src/cs43l22.d \
./Core/Src/diskio.d \
./Core/Src/fatfs.d \
./Core/Src/ff.d \
./Core/Src/ff_gen_drv.d \
./Core/Src/fops.d \
./Core/Src/main.d \
./Core/Src/mmc_sd.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/user_diskio.d \
./Core/Src/waveplayer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F407xx -c -I../Core/Inc -I"C:/Users/rong_he/STM32CubeIDE/workspace_1.8.0/fat/FreeRTOS/include" -I"C:/Users/rong_he/STM32CubeIDE/workspace_1.8.0/fat/FreeRTOS/portable/ARM_CM4F" -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/AUDIO.d ./Core/Src/AUDIO.o ./Core/Src/AUDIO_LINK.d ./Core/Src/AUDIO_LINK.o ./Core/Src/File_Handling.d ./Core/Src/File_Handling.o ./Core/Src/cs43l22.d ./Core/Src/cs43l22.o ./Core/Src/diskio.d ./Core/Src/diskio.o ./Core/Src/fatfs.d ./Core/Src/fatfs.o ./Core/Src/ff.d ./Core/Src/ff.o ./Core/Src/ff_gen_drv.d ./Core/Src/ff_gen_drv.o ./Core/Src/fops.d ./Core/Src/fops.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/mmc_sd.d ./Core/Src/mmc_sd.o ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/user_diskio.d ./Core/Src/user_diskio.o ./Core/Src/waveplayer.d ./Core/Src/waveplayer.o

.PHONY: clean-Core-2f-Src

