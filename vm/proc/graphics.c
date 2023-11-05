#include "graphics.h"

#include <SDL.h>

#include "header.h"

struct Video {
	int windowWidth;
	int windowHeight;

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

	video.windowWidth  = (int) width;
	video.windowHeight = (int) height;

	if(!video.window)
		return -1;

	video.surface = SDL_CreateRGBSurface(0, video.windowWidth, video.windowHeight, 32, 0, 0, 0, 0);
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

        // TODO: can you avoid memcpy of whole buffer of pixels on every update?
	memcpy(video.surface->pixels, video.mem, video.memSize);
	SDL_BlitScaled(video.surface, NULL, windowSurface, NULL);

        // TODO: Also, you do nothing to ensure scaling with consistent aspect ratio
        //       Because of this I can extend window breaking drawn circle and making
        //       it an ellips.

	SDL_UpdateWindowSurface(video.window);
	SDL_Delay(100); //30 fps
	return 0;
}

int graphicsUpdateEvents() {
	while(SDL_PollEvent(&video.event)) {
		switch(video.event.type) {
		case SDL_QUIT:
			return 1;
		}
	}
	return 0;
}

