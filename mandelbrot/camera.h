#pragma once
#include <SDL.h>

struct Camera {
    int windowWidth;
    int windowHeight;
    float scale;                // how many pixels in 1 coordinate
    float centerPositionX;
    float centerPositionY;
};

void mouseWheelHandler  (const SDL_Event *e, struct Camera *camera);
void windowResizeHandler(const SDL_Event *e, struct Camera *camera);
