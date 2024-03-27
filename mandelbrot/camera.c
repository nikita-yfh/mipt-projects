#include "camera.h"

void mouseWheelHandler  (const SDL_Event *e, struct Camera *camera) {
    double zoom = (e->wheel.y > 0) ? ZOOM : 1.0/ZOOM;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    mouseX -= camera->windowWidth  / 2;
    mouseY -= camera->windowHeight / 2;

    camera->centerPositionX -= (zoom - 1.0) * mouseX / camera->scale / zoom; // magic
    camera->centerPositionY -= (zoom - 1.0) * mouseY / camera->scale / zoom;

    camera->scale *= zoom;
}

SDL_Surface *windowResizeHandler(const SDL_Event *e, SDL_Window *window, struct Camera *camera) {
    camera->windowWidth  = e->window.data1;
    camera->windowHeight = e->window.data2 << 3 >> 3;
    SDL_SetWindowSize(window, camera->windowWidth, camera->windowHeight);
    return SDL_GetWindowSurface(window);
}
