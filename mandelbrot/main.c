#include <SDL2/SDL.h>
#include <stdbool.h>
#include "camera.h"
#include "mandelbrot.h"

const char *WINDOW_TITLE = "Mandelbrot!";

const int WINDOW_WIDTH  = 800;
const int WINDOW_HEIGHT = 600;
const double INIT_SCALE  = 300.0f;

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Event event;
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    struct Camera camera = {WINDOW_WIDTH, WINDOW_HEIGHT, INIT_SCALE, 0.0f, 0.0f};

    bool AVX = false;
    enum Fractal fractal = FRACTAL_MENDELBROT;
    enum PaletteType palette = PALETTE_GREY;
    generatePalettes();

    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                        surface = windowResizeHandler(&event, window, &camera);
                    break;
                case SDL_MOUSEWHEEL:
                    mouseWheelHandler(&event, &camera);
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_r:
                            AVX = !AVX;
                            break;
                        case SDLK_f:
                            fractal = (fractal + 1) % FRACTAL_COUNT;
                            break;
                        case SDLK_p:
                            palette = (palette + 1) % PALETTE_COUNT;
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        if(AVX)
            mandelbrotAVX(surface, &camera, fractal, getPalette(palette));
        else
            mandelbrot(surface, &camera, fractal, getPalette(palette));

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

