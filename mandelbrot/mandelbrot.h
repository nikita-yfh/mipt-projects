#pragma once
#include "camera.h"

#define BLACK_32 0xFF000000
#define GREEN_32 0xFF00FF00

enum Fractal {
    FRACTAL_MENDELBROT,
    FRACTAL_BURNING_SHIP
};

void mandelbrot   (SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal);
void mandelbrotAVX(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal);
