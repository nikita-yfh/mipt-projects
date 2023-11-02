#include "graphics.h"

#include <SDL.h>

#include "header.h"

struct Video {
	SDL_Window *window;
	SDL_Surface *surface;
	SDL_Event event;

	void *mem;
	arg_t memSize;
};

static struct Video video;

const char *graphicsGetError() {
	return SDL_GetError();
}

int graphicsInit(void *mem, arg_t width, arg_t height) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	video.window = SDL_CreateWindow(PROGRAM, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			(int) width, (int) height, SDL_WINDOW_SHOWN);

	if(!video.window)
		return -1;

	video.surface = SDL_GetWindowSurface(video.window);
	if(!video.surface)
		return -1;

	video.mem = mem;
	video.memSize = width * height * sizeof(arg_t);

	graphicsUpdate();

	return 0;
}

int graphicsQuit() {
	if(video.window)
		SDL_DestroyWindow(video.window);
	SDL_Quit();
	return 0;
}

int graphicsUpdate() {
	while(SDL_PollEvent(&video.event)) {
		if(video.event.type == SDL_QUIT)
			return 1;
	}
	memcpy(video.surface->pixels, video.mem, video.memSize);
	SDL_UpdateWindowSurface(video.window);
	return 0;
}

