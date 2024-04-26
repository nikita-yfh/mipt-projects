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

enum RenderType {
    RENDER_DUMB,    /// Without optimizations
    RENDER_ARRAYS,  /// With arrays optimizations
    RENDER_AVX,     /// With AVX-512 instructions

    RENDER_COUNT
};

/**
 * \brief Gives name of renderer
 * \param [in] renderType Type of renderer
 * \return                Name of renderer
 **/

const char *getRendererName(enum RenderType renderType);

/**
 * \brief Main Mandelbrot function
 * \param [in] surface Surface to draw
 * \param [in] camera  Position of fractal
 * \param [in] fractal Type of fractal
 * \param [in] palette Fractal palette
 **/

void mandelbrotCommon(enum RenderType render, SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette);


void mandelbrotDumb(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette);
void mandelbrotArrays(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette);
void mandelbrotAVX(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette);
