cmake_minimum_required(VERSION 3.5)

project("vm")

set(CMAKE_C_FLAGS
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
-Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,\
alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,\
integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,\
returns-nonnull-attribute,shift,signed-integer-overflow,undefined,\
unreachable,vla-bound,vptr")

set(COMMON_SOURCES
	common/commands.c
	common/commands.h
	common/instruction.h
	common/instruction.c
	common/text.h
	common/text.c
	common/utils.h
	common/utils.c
)

include_directories(common)

set(ASSEMBLER_SOURCES
	asm/asm.c
	asm/asm.h
	asm/main.c
)

set(DISASSEMBLER_SOURCES
	disasm/disasm.c
	disasm/disasm.h
	disasm/main.c
)

set(PROCESSOR_SOURCES
	processor/processor.c
)

add_executable(assembler ${ASSEMBLER_SOURCES} ${COMMON_SOURCES})
add_executable(disassembler ${DISASSEMBLER_SOURCES} ${COMMON_SOURCES})

