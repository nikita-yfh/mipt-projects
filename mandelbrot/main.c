#include <SDL2/SDL.h>
#include <stdbool.h>
#include "camera.h"
#include "mandelbrot.h"

const char *WINDOW_TITLE = "Mandelbrot!";

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;
const int INIT_SCALE    = 300;

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Event event;
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    bool running = true;
    struct Camera camera = {WINDOW_WIDTH, WINDOW_HEIGHT, INIT_SCALE, 0.0f, 0.0f};

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        surface = SDL_GetWindowSurface(window);
                        windowResizeHandler(&event, &camera);
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    mouseWheelHandler(&event, &camera);
                default:
                    break;
            }
        }

        mandelbrot(surface, &camera);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

