cmake_minimum_required(VERSION 3.5)

project("vm")

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

set(COMMON_SOURCES
	common/instruction.h
	common/instruction.c
	common/utils.h
	common/utils.c
	common/header.h
	common/header.c
	common/registers.h
	common/registers.c
)

include_directories(common)

set(ASSEMBLER_SOURCES
	asm/asm.c
	asm/asm.h
	asm/labels.h
	asm/labels.c
	asm/main.c
)

set(DISASSEMBLER_SOURCES
	disasm/disasm.c
	disasm/disasm.h
	disasm/main.c
)

set(PROCESSOR_SOURCES
	proc/main.c
	proc/graphics.h
	proc/graphics.c
	proc/operations.c
	proc/instructions.c
	proc/processor.h
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(stack)
include_directories(stack)
include_directories(log)

add_executable(badapplegen badapplegen.c)

add_executable(assembler ${ASSEMBLER_SOURCES} ${COMMON_SOURCES})
add_executable(processor ${PROCESSOR_SOURCES} ${COMMON_SOURCES})
#add_executable(disassembler ${DISASSEMBLER_SOURCES} ${COMMON_SOURCES})

target_link_libraries(processor stack logger m ${SDL2_LIBRARIES})

