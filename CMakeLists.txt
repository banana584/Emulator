cmake_minimum_required(VERSION 3.10)

project(emulator LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(Registers SHARED
    src/registers/registers.c
)
add_library(RAM SHARED
    src/memory/RAM/RAM.c
)
add_library(Cache SHARED
    src/memory/cache/cache.c
)
add_library(Memory SHARED
    src/memory/memutils.c
)

target_link_libraries(Memory RAM Cache)

add_executable(${CMAKE_PROJECT_NAME}
    src/main.c
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
)

target_link_libraries(${CMAKE_PROJECT_NAME} Registers Memory)