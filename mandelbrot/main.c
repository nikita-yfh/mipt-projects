#include <SDL2/SDL.h>
#include <stdbool.h>

const char *windowTitle = "Mandelbrot!";

const int windowInitWidth  = 800;
const int windowInitHeight = 600;

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(windowTitle,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowInitWidth, windowInitHeight,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Event e;
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    bool running   = true;

    while (running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                        surface = SDL_GetWindowSurface(window);
                    break;
                default:
                    break;
            }
        }

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

