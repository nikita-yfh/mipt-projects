#include <stdio.h>
#include <stdint.h>

#define WIDTH 100u
#define HEIGHT 30u

static uint32_t getColor(char c) {
	switch(c) {
	case '$':
		return 0xFF000000;
	case ' ':
	case '.':
		return 0xFFFFFFFF;
	}
	return 0xFF808080;
}


int main() {
	FILE *badapple = fopen("badapple.txt", "rb");

	FILE *out = fopen("badapple.ys", "w");

	static uint32_t memory[WIDTH * HEIGHT] = {};

	fprintf(out, "; run with arguments: -V %u,%u,0 -m %u\n",
			WIDTH, HEIGHT, WIDTH * HEIGHT);

	while(!feof(badapple)) {
		for(unsigned line = 0; line < HEIGHT; line++) {
			char buffer[1024];
			fgets(buffer, 1024, badapple);
			for(unsigned column = 0; column < WIDTH; column++) {
				uint32_t address = line * WIDTH + column;
				uint32_t color = getColor(buffer[column]);
				if(memory[address] != color) {
					fprintf(out, "push %u\npop[%u]\n", color, address);
					memory[address] = color;
				}
			}
		}
		fprintf(out, "upd\n");
	}

	fclose(out);
	fclose(badapple);
	return 0;
}
