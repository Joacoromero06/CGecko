cmake_minimum_required(VERSION 3.15)
project(Gecko-Language LANGUAGES C)

# Set C standard (e.g., C11)
set(CMAKE_C_STANDARD 11)

# Add executable target
add_executable(Gecko
            src/main.c
            src/chunk.c
            src/debug.c
            src/memory.c
            src/value.c
            src/vm.c
            src/compiler.c
            src/scanner.c
            src/object.c
            src/table.c
            )   