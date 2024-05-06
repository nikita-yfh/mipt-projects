#pragma once

#include "camera.h"
#include "mandelbrot.h"

#include <SDL.h>
#include <stdint.h>

/**
 * \brief Runs bencjmark with different renderers
 * \param surface   Drawing surface
 * \param camera    Camera
 * \param fractal   Fractal type
 * \param pallete   Pallete for drawing (dummy)
 **/

void runBenchmark(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette);
