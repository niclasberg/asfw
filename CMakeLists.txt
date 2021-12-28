cmake_minimum_required(VERSION 3.0.0)
project(stm32synth VERSION 0.1.0)

enable_language(CXX C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(lib)

set(EXECUTABLE ${PROJECT_NAME}.out)

add_executable(${EXECUTABLE} 
    projects/adc_read/main.cpp
    stm32/syscalls.c
    stm32/interrupts.cpp
    stm32/startup_stm32f407vgtx.s)

target_link_libraries(${EXECUTABLE} lib)

target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -fdata-sections
        -ffunction-sections)

target_link_options(${EXECUTABLE} PRIVATE
    -T${CMAKE_SOURCE_DIR}/stm32/STM32F407VGTX_FLASH.ld
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -specs=nano.specs 
    -specs=nosys.specs
    -lc
    -lm
    -lnosys
    -fno-exceptions
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
    )

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND arm-none-eabi-size ${EXECUTABLE})

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)