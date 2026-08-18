cmake_minimum_required(VERSION 3.16)

project(CPUSchedulingSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(scheduler_core
    src/scheduler.cpp
)

target_include_directories(scheduler_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(scheduler_core PRIVATE /W4)
else()
    target_compile_options(scheduler_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(scheduler
    src/main.cpp
)
target_link_libraries(scheduler PRIVATE scheduler_core)

enable_testing()

add_executable(scheduler_tests
    tests/test_scheduler.cpp
)
target_link_libraries(scheduler_tests PRIVATE scheduler_core)

add_test(NAME scheduler_tests COMMAND scheduler_tests)
