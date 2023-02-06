cmake_minimum_required(VERSION 3.15)

include(CTest)

# Build tests
add_executable(run_tests)
target_compile_options(run_tests PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
target_compile_options(run_tests PRIVATE -fsanitize=address,undefined)
target_link_options(run_tests PRIVATE -fsanitize=address,undefined)

target_sources(run_tests
    PRIVATE
    main.cpp
    test_delegate.cpp
    test_rational.cpp
    async/test_and_then.cpp
    async/test_bind_back.cpp
    #async/test_conditional.cpp
    async/test_emit.cpp
    async/test_event.cpp
    async/test_inline_scheduler.cpp
    async/test_map.cpp
    async/test_outcome.cpp
    async/test_just.cpp
    async/test_pollable.cpp
    async/test_sequence.cpp
    async/test_use_state.cpp
    async/test_when_all.cpp
    #async/test_when_any.cpp
    board/test_clock_config.cpp
    cont/test_box.cpp
    drivers/test_adc.cpp
    #drivers/test_cs43l22.cpp
    drivers/test_dma.cpp
    drivers/test_gpio.cpp
    drivers/test_i2c.cpp
    #drivers/test_i2c_memory.cpp
    drivers/test_i2s.cpp
    drivers/test_spi.cpp
    drivers/test_uart.cpp
    reg/test_clear.cpp
    reg/test_combine.cpp
    reg/test_set.cpp
    reg/test_toggle.cpp
    reg/test_peripheral_operations.cpp
    reg/test_write.cpp
    schedulers/test_cooperative_scheduler.cpp
    tmp/test_type_list.cpp)

target_include_directories(run_tests 
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/include
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/hana/include)
target_compile_features(run_tests PUBLIC cxx_std_20)

enable_testing()
add_test(RunAllTests run_tests)