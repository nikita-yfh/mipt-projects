#include "graphics.h"

#include <SDL.h>

#include "header.h"

struct Video {
	int minWindowWidth;
	int minWindowHeight;

	SDL_Surface *surface;
	SDL_Window *window;
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
			(int) width, (int) height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	video.minWindowWidth  = (int) width;
	video.minWindowHeight = (int) height;

	if(!video.window)
		return -1;

	video.surface = SDL_CreateRGBSurfaceFrom(mem,
			video.minWindowWidth, video.minWindowHeight,
			32, video.minWindowWidth * 4, 0, 0, 0, 0);
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
	SDL_Surface *windowSurface = SDL_GetWindowSurface(video.window);
	if(!windowSurface)
		return -1;

	SDL_BlitScaled(video.surface, NULL, windowSurface, NULL);

	SDL_UpdateWindowSurface(video.window);
	SDL_Delay(100); //10 fps
	return 0;
}

static void graphicsFixWindowSize() {
	int windowWidth, windowHeight;
	SDL_GetWindowSize(video.window, &windowWidth, &windowHeight);

	if(windowWidth < video.minWindowWidth)
		windowWidth = video.minWindowWidth;
	if(windowHeight < video.minWindowHeight)
		windowHeight = video.minWindowHeight;

	windowHeight = windowWidth * video.minWindowHeight / video.minWindowWidth;

	SDL_SetWindowSize(video.window, windowWidth, windowHeight);
}

int graphicsUpdateEvents() {
	while(SDL_PollEvent(&video.event)) {
		switch(video.event.type) {
		case SDL_QUIT:
			return 1;
		case SDL_WINDOWEVENT:
			graphicsFixWindowSize();
			break;
		}
	}
	return 0;
}

