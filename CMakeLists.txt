cmake_minimum_required(VERSION 3.10)

project(emulator LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_library(Registers SHARED
    src/emulator/registers/registers.c
)
add_library(Memory SHARED
    src/emulator/memory/memutils.c
)
add_library(RAM SHARED
    src/emulator/memory/RAM/RAM.c
)
add_library(Cache SHARED
    src/emulator/memory/cache/cache.c
)
add_library(ROM SHARED
    src/emulator/memory/ROM/ROM.c
)
add_library(CPUUtils SHARED
    src/emulator/CPU/CPUutils.c
)
add_library(Interrupts SHARED
    src/emulator/CPU/interrupts/Interrupts.c
)
add_library(CPU SHARED
    src/emulator/CPU/CPU.c
)

target_link_libraries(Memory m)
target_link_libraries(Cache Memory)
target_link_libraries(RAM Memory Cache)
target_link_libraries(Interrupts CPUUtils)
target_link_libraries(CPU RAM ROM Cache Registers CPUUtils Interrupts)

target_compile_options(Registers PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)
target_compile_options(Memory PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)
target_compile_options(RAM PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)
target_compile_options(Cache PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)
target_compile_options(ROM PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)
target_compile_options(CPUUtils PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)
target_compile_options(Interrupts PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)
target_compile_options(CPU PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)

add_executable(${CMAKE_PROJECT_NAME}
    src/main.c
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -g
)

target_link_libraries(${CMAKE_PROJECT_NAME} Registers RAM Cache ROM CPU)