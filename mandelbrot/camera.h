#pragma once
#include <SDL.h>

struct Camera {
    int windowWidth;
    int windowHeight;
    double scale;                // how many pixels in 1 coordinate
    double centerPositionX;
    double centerPositionY;
};

void mouseWheelHandler  (const SDL_Event *e, struct Camera *camera);
SDL_Surface *windowResizeHandler(const SDL_Event *e, SDL_Window *window, struct Camera *camera);
