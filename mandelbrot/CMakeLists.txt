cmake_minimum_required(VERSION 3.5)

project("mandelbrot")

set(CMAKE_C_FLAGS_DEBUG
"-D _DEBUG -ggdb3 -g -O0 -Wall -Wextra \
-Waggressive-loop-optimizations -Wmissing-declarations  \
-Wcast-align -Wcast-qual -Wchar-subscripts \
-Wconversion -Wempty-body -Wfloat-equal \
-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 \
-Winline -Wlogical-op -Wopenmp-simd \
-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow \
-Wsign-conversion -Wstrict-overflow=2 \
-Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types \
-Wswitch-enum -Wsync-nand -Wundef \
-Wunreachable-code -Wunused -Wvariadic-macros \
-Wno-missing-field-initializers -Wno-narrowing \
-Wno-varargs -Wstack-protector -fcheck-new \
-fstack-protector -fstrict-overflow \
-flto-odr-type-merging -fno-omit-frame-pointer \
-Wstack-usage=8192 -pie -fPIE -Werror=vla")

set(CMAKE_C_FLAGS "-march=tigerlake -mavx512f")

set(SOURCES
    main.c
    camera.c
    camera.h
    mandelbrot.h
    mandelbrot.c
    palette.h
    palette.c
)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(mandelbrot ${SOURCES})
target_link_libraries(mandelbrot m ${SDL2_LIBRARIES})

