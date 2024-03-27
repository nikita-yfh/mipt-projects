#include "fps.h"

#include <assert.h>

static SDL_Surface *digits = NULL;
const char *filename = "Numbers14x20.bmp";
static const int DIGIT_WIDTH  = 14;
static const int DIGIT_HEIGHT = 20;
static const int CORNER_GAP   = 30;

void initFps() {
    digits = SDL_LoadBMP(filename);
    assert(digits);
}

void quitFps() {
    SDL_FreeSurface(digits);
}

void showFps(SDL_Surface *surface, const struct Camera *camera, uint32_t fps) {
    SDL_Rect dstRect = { camera->windowWidth - CORNER_GAP - DIGIT_WIDTH, CORNER_GAP,
                        DIGIT_WIDTH, DIGIT_HEIGHT};

    while(fps) {
        int digit = fps % 10;
        fps /= 10;

        SDL_Rect srcRect = {DIGIT_WIDTH * digit, 0, DIGIT_WIDTH, DIGIT_HEIGHT};
        SDL_BlitSurface(digits, &srcRect, surface, &dstRect);


        dstRect.x -= DIGIT_WIDTH;
    }
}

