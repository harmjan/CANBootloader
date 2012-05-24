################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Programmer.axf

dependents:
	-cd /home/chiel/Documents/BachelorProject/LPCXpresso/Bootloaderlib/Release && $(MAKE) clean all
	-cd /home/chiel/Documents/BachelorProject/LPCXpresso/CMSISv2.10_LPC17xx/Debug && $(MAKE) clean all
	-cd /home/chiel/Documents/BachelorProject/LPCXpresso/LPC17xx-Drivers/Debug && $(MAKE) clean all

# Tool invocations
Programmer.axf: $(OBJS) $(USER_OBJS) /home/chiel/Documents/BachelorProject/LPCXpresso/Bootloaderlib/Release/libBootloaderlib.a /home/chiel/Documents/BachelorProject/LPCXpresso/CMSISv2.10_LPC17xx/Debug/libCMSISv2.10_LPC17xx.a /home/chiel/Documents/BachelorProject/LPCXpresso/LPC17xx-Drivers/Debug/libLPC17xx-Drivers.a
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/chiel/Documents/BachelorProject/LPCXpresso/LPC17xx-Drivers/Debug" -L"/home/chiel/Documents/BachelorProject/LPCXpresso/Bootloaderlib/Release" -L"/home/chiel/Documents/BachelorProject/LPCXpresso/CMSISv2.10_LPC17xx/Debug" -L"/home/chiel/Documents/BachelorProject/LPCXpresso/Bootloaderlib/Debug" -Xlinker -Map="Programmer.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "Programmer_Debug.ld" -o"Programmer.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) Programmer.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Programmer.axf"; # arm-none-eabi-objcopy -O binary "Programmer.axf" "Programmer.bin" ; checksum -p LPC1769 -d "Programmer.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
/home/chiel/Documents/BachelorProject/LPCXpresso/Bootloaderlib/Release/libBootloaderlib.a:
/home/chiel/Documents/BachelorProject/LPCXpresso/CMSISv2.10_LPC17xx/Debug/libCMSISv2.10_LPC17xx.a:
/home/chiel/Documents/BachelorProject/LPCXpresso/LPC17xx-Drivers/Debug/libLPC17xx-Drivers.a:

-include ../makefile.targets
