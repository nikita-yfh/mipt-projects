#include "camera.h"

void mouseWheelHandler  (const SDL_Event *e, struct Camera *camera) {
    float zoom = (e->wheel.y > 0) ? 1.1f : 1.0f/1.1f;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    mouseX -= camera->windowWidth  / 2.0;
    mouseY -= camera->windowHeight / 2.0;

    camera->centerPositionX -= (zoom - 1.0) * mouseX / camera->scale / zoom; // magic
    camera->centerPositionY -= (zoom - 1.0) * mouseY / camera->scale / zoom;

    camera->scale *= zoom;
}

void windowResizeHandler(const SDL_Event *e, struct Camera *camera) {
    camera->windowWidth  = e->window.data1;
    camera->windowHeight = e->window.data2;
}
