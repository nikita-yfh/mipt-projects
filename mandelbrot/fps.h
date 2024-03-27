#pragma once

#include <SDL.h>
#include <stdint.h>

#include "camera.h"

/** load digits image **/
void initFps();
/** free digits image **/
void quitFps();

/**
 * \brief show fps at top right corner
 * \param surface           window surface
 * \param camera            camera
 * \param fps               FPS value to show
 **/
void showFps(SDL_Surface *surface, const struct Camera *camera, uint32_t fps);
