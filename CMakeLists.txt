cmake_minimum_required(VERSION 3.10)
project(BSVM)


set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Og -g -Wall -Wextra -Wunused -Iinclude -fsanitize=address,leak,undefined,signed-integer-overflow,bounds-strict)
add_link_options(-fsanitize=address,leak,undefined)

add_library(core
    src/format/binary.c
    src/runtime/instructions.c
    src/runtime/instructions_codec.c
)

target_include_directories(core PUBLIC include)

add_executable(bsvm
    src/bin/main_bsvm.c
    src/runtime/cpu.c
    src/runtime/instructions_codec.c
    src/runtime/instructions.c
    src/runtime/loader.c
    src/runtime/memory.c
    src/runtime/vm.c
    src/runtime/allocator.c
)

target_link_libraries(bsvm PRIVATE core)
target_include_directories(bsvm PRIVATE include)


#POV : t'as bien chié dans la colle niveau modularité
add_executable(bsm
    src/bin/main_bsm.c
    src/toolchain/lexer.c
    src/toolchain/parser.c
    src/toolchain/assembler.c
    src/runtime/vm.c
    src/runtime/cpu.c
    src/runtime/memory.c
    src/runtime/allocator.c
)

target_link_libraries(bsm PRIVATE core)
target_include_directories(bsm PRIVATE include)