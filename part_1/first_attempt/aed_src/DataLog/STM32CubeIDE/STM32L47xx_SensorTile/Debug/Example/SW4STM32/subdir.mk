################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU ARM Embedded (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/home/pranay/iitd/sem7/col788/STSW-STLKT01_V2.5.0/Projects/SensorTile/Applications/DataLog/STM32CubeIDE/startup_stm32l476xx.s 

S_DEPS += \
./Example/SW4STM32/startup_stm32l476xx.d 

OBJS += \
./Example/SW4STM32/startup_stm32l476xx.o 


# Each subdirectory must supply rules for building sources it contributes
Example/SW4STM32/startup_stm32l476xx.o: /home/pranay/iitd/sem7/col788/STSW-STLKT01_V2.5.0/Projects/SensorTile/Applications/DataLog/STM32CubeIDE/startup_stm32l476xx.s Example/SW4STM32/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Example-2f-SW4STM32

clean-Example-2f-SW4STM32:
	-$(RM) ./Example/SW4STM32/startup_stm32l476xx.d ./Example/SW4STM32/startup_stm32l476xx.o

.PHONY: clean-Example-2f-SW4STM32

