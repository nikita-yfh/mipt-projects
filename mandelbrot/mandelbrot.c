#include "mandelbrot.h"

void mandelbrot(SDL_Surface *surface, const struct Camera *camera) {
    SDL_FillRect(surface, NULL, BLACK_32);   // fill background

    int windowCenterX = camera->windowWidth  / 2;
    int windowCenterY = camera->windowHeight / 2;

    int sizeInPixels = camera->scale;

    SDL_Rect testRect = {windowCenterX - sizeInPixels / 2 + camera->centerPositionX * camera->scale,
                         windowCenterY - sizeInPixels / 2 + camera->centerPositionY * camera->scale,
                         sizeInPixels, sizeInPixels};

    SDL_FillRect(surface, &testRect, GREEN_32);
}
