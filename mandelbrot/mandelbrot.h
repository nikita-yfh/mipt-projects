#pragma once
#include "camera.h"

#define BLACK_32 0xFF000000
#define GREEN_32 0xFF00FF00

void mandelbrot(SDL_Surface *surface, const struct Camera *camera);
void mandelbrotAVX(SDL_Surface *surface, const struct Camera *camera);
