// TODO:

// When I open this file separately, my language server thinks
// that it's wrong as there is a lot of undefined "DEF_COMMAND"

// I think it's a good practice to silence lang tools in such
// cases, it can be done by adding something like to this file:

// #ifndef DEF_COMMAND
// #define DEF_COMMAND(...)
// #endif

#define DEF_COMMAND_MATH(name, number, op, checkZero)	\
	DEF_COMMAND(name, number, false, {					\
		stackValue_t a = 0;								\
		stackValue_t b = 0;								\
		stackValue_t c = 0;								\
		STACK_POP(processor, &b);						\
		STACK_POP(processor, &a);						\
		if(checkZero && b == 0)							\
			return EXEC_MATH_ERROR;						\
		c = a op b;										\
		STACK_PUSH(processor, c);						\
		return EXEC_OK;									\
	})

// TODO: These two (^ and v) are almost identical, can they be made to share code?
#define DEF_COMMAND_FMATH(name, number, op)				\
	DEF_COMMAND(name, number, false, {					\
		float a = NAN;									\
		float b = NAN;									\
		float c = NAN;									\
		STACK_POP(processor, (stackValue_t*) &b);		\
		STACK_POP(processor, (stackValue_t*) &a);		\
		c = a op b;										\
		STACK_PUSH(processor, *((stackValue_t*) &c));	\
		return EXEC_OK;									\
	})

#define DEF_JUMP(name, number, op)						\
	DEF_COMMAND(name, number, true, {					\
		arg_t *address = NULL;							\
		GET_ARGUMENT(processor, instruction,			\
				DIR_IN, &address);						\
		stackValue_t arg = 0;							\
		STACK_POP(processor, &arg);						\
		if(arg op)										\
			processor->pc = *address;					\
		return EXEC_OK;									\
	})

DEF_COMMAND("nop", 0x01, false, {
	return EXEC_OK;
})

DEF_COMMAND("hlt", 0x02, false, {
	processor->pc = PC_HLT;
	return EXEC_END_PROGRAM;
})

DEF_COMMAND("push", 0x03, true, {
	arg_t *arg = NULL;
	GET_ARGUMENT(processor, instruction, DIR_IN, &arg);

	STACK_PUSH(processor, (stackValue_t) *arg);
	return EXEC_OK;
})

DEF_COMMAND("pop", 0x04, true, {
	stackValue_t var = 0;
	STACK_POP(processor, &var);
	// TODO: Is there anything that prevents pop from accepting immediate?
	//       Let's say "pop 42"? There is nothing that expresses it in DSL

	arg_t *arg = NULL;
	GET_ARGUMENT(processor, instruction, DIR_OUT, &arg);

	*arg = (arg_t) var;
	return EXEC_OK;
})

DEF_COMMAND_MATH("add", 0x05, +, false)
DEF_COMMAND_MATH("sub", 0x06, -, false)
DEF_COMMAND_MATH("mul", 0x07, *, false)
DEF_COMMAND_MATH("div", 0x08, /, true)
DEF_COMMAND_MATH("mod", 0x09, %, true)

DEF_COMMAND("in", 0x0A, false, {
	stackValue_t a = 0;
	scanf("%d", &a);
	STACK_PUSH(processor, a);
	return EXEC_OK;
})

DEF_COMMAND("out", 0x0B, false, {
	stackValue_t a = 0;
	STACK_POP(processor, &a);
	printf("%d\n", a);
	return EXEC_OK;
})

DEF_COMMAND_FMATH("fadd", 0x0C, +);
DEF_COMMAND_FMATH("fsub", 0x0D, -);
DEF_COMMAND_FMATH("fmul", 0x0E, *);
DEF_COMMAND_FMATH("fdiv", 0x0F, /);

DEF_COMMAND("sqrt", 0x10, false, {
	float a = NAN;
	STACK_POP(processor, (stackValue_t*) &a);
	float b = sqrtf(a);
	STACK_PUSH(processor, *((stackValue_t*) &b));
	return EXEC_OK;
})

DEF_COMMAND("fin", 0x11, false, {
	float a = NAN;
	scanf("%f", &a);
	STACK_PUSH(processor, *((stackValue_t*) &a));
	return EXEC_OK;
})

DEF_COMMAND("fout", 0x12, false, {
	float a = 0;
	STACK_POP(processor, (stackValue_t*) &a);
	printf("%g\n", a);
	return EXEC_OK;
})

DEF_COMMAND("df", 0x13, false, {
	stackValue_t d = 0;
	STACK_POP(processor, &d);
	float f = (float) d;
	STACK_PUSH(processor, *((stackValue_t*) &f));
	return EXEC_OK;
})

DEF_COMMAND("fd", 0x14, false, {
	float f = NAN;
	STACK_POP(processor, (stackValue_t*) &f);
	int d = (int) f;
	STACK_PUSH(processor, d);
	return EXEC_OK;
})

DEF_JUMP("jmp", 0x15, || true)
DEF_JUMP("jge", 0x16, >= 0)
DEF_JUMP("jg",  0x17, >  0)
DEF_JUMP("jle", 0x18, <= 0)
DEF_JUMP("jl",  0x19, <  0)
DEF_JUMP("je",  0x1A, == 0)
DEF_JUMP("jne", 0x1B, != 0)

DEF_COMMAND("call", 0x1C, true, {
	arg_t *address = NULL;
	GET_ARGUMENT(processor, instruction, DIR_IN, &address);

	if(stackPush(&processor->callStack, (stackValue_t) processor->pc))
		return EXEC_STACK_OVERFLOW;
	processor->pc = *address;
	return EXEC_OK;
})

DEF_COMMAND("ret", 0x1D, false, {
	int32_t jump = 0;
	if(stackPop(&processor->callStack, &jump))
		return EXEC_STACK_EMPTY;

	processor->pc = (arg_t) jump;
	return EXEC_OK;
})

DEF_COMMAND("upd", 0x1E, false, {
	if(processor->videoEnable)
		graphicsUpdate();
	return EXEC_OK;
})
