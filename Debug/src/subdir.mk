################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BilinearMappingHandler.cpp \
../src/EncryptionHandler.cpp \
../src/Eng_final_proj.cpp \
../src/State.cpp \
../src/StateMachine.cpp \
../src/Transition3Tuple.cpp 

OBJS += \
./src/BilinearMappingHandler.o \
./src/EncryptionHandler.o \
./src/Eng_final_proj.o \
./src/State.o \
./src/StateMachine.o \
./src/Transition3Tuple.o 

CPP_DEPS += \
./src/BilinearMappingHandler.d \
./src/EncryptionHandler.d \
./src/Eng_final_proj.d \
./src/State.d \
./src/StateMachine.d \
./src/Transition3Tuple.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/pbc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


