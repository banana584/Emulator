# Overview
A custom ISA emulator. ISA is found in ISA.txt\
Source files are found in src/\
Include files are found in include/\
Cmake is used as the build system\

## Input / Output
### Inputs
Custom assembly compiler / linker. Emulator should provide support for running these files
### Outputs
Console output\
Window output may be supported later

## Specifications
### Instructions
Instructions are found in ISA.txt\
### RAM
This CPU will have RAM on the smaller side\
### Cache
This CPU will have quite small caches\
### Registers
The accumulator will store results from operations\
This can be swapped to and from the X/Y registers\
The program counter stores where the CPU is executing currently\
The stack pointer stores where the stack is in RAM, starting from the top and growing downwards\
The status will store information about the state of the CPU or the last operation\