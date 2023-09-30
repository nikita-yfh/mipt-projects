#include <stdio.h>
#include "stack.h"

int main() {
	struct Stack *stack = stackCreate();

	for(int i = 0; i < 10; i++) {
		int value = i * 64;
		stackPush(stack, value);
		printf("Stack push #%d: %d\n", i, value);
		stackDump(stack, stderr);
	}


	int value = 0;
	for(int i = 0; i < 10; i++) {
		stackPop(stack, &value);
		printf("Stack pop  #%d: %d\n", i, value);
	}

	stackDelete(stack);

	return 0;
}
