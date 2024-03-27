#pragma once
#include "camera.h"
#include "palette.h"

#define BLACK_32 0xFF000000
#define GREEN_32 0xFF00FF00

enum Fractal {
    FRACTAL_MENDELBROT,
    FRACTAL_BURNING_SHIP,

    FRACTAL_COUNT
};

/**
 * \brief Main Mandelbrot function
 * \param [in] surface Surface to draw
 * \param [in] camera  Position of fractal
 * \param [in] fractal Type of fractal
 * \param [in] palette  Fractal palette
 **/
void mandelbrot(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette);

/**
 * \brief Mandelbrot function with AVX512 optimisation
 * \param [in] surface Surface to draw
 * \param [in] camera  Position of fractal
 * \param [in] fractal Type of fractal
 * \param [in] palette  Fractal palette
 **/
void mandelbrotAVX(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette);
