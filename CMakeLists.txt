cmake_minimum_required(VERSION 3.10)
project(CustomVM)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Og -g -Wall -Wextra -Wunused -Iinclude -Wpedantic -fsanitize=address,leak,undefined,signed-integer-overflow,bounds-strict)
add_link_options(-fsanitize=address,leak,undefined)

file(GLOB_RECURSE C_SOURCES "src/*.c")
list(FILTER C_SOURCES EXCLUDE REGEX ".*/main_bsvm\\.c$")

add_library(custom_vm ${C_SOURCES})

target_include_directories(custom_vm PUBLIC include)

add_executable(bsvm src/main_bsvm.c)
target_link_libraries(bsvm PUBLIC custom_vm m)
