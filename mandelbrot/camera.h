#pragma once
#include <SDL.h>

#define ZOOM 1.1

struct Camera {
    int windowWidth;
    int windowHeight;
    double scale;                // how many pixels in 1 coordinate
    double centerPositionX;
    double centerPositionY;
};

/**
 * \brief Handle mouse wheel event
 * \param e         wheel event
 * \param camera    Camera 
 **/
void mouseWheelHandler  (const SDL_Event *e, struct Camera *camera);

/**
 * \brief Changing window size event handler
 * \param e         resize event
 * \param window    window for resize
 * \param camera    Camera 
 * \return New window surface
 **/
SDL_Surface *windowResizeHandler(const SDL_Event *e, SDL_Window *window, struct Camera *camera);
