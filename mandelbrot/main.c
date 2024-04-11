#include <SDL2/SDL.h>
#include <stdbool.h>
#include <sys/time.h>

#include "camera.h"
#include "mandelbrot.h"
#include "fps.h"

static const char *WINDOW_TITLE = "Mandelbrot!";

static const int WINDOW_WIDTH  = 800;
static const int WINDOW_HEIGHT = 600;
static const double INIT_SCALE = 300.0f;

static const int ANIMATION_MILLIS = 50;

static const int BENCH_ITERATIONS = 100; 

static uint64_t getMicros() { // for fps 
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec* (uint64_t) 1000000+tv.tv_usec;
}

static uint64_t getProcTicks() { // for benchmark
    unsigned long lo, hi;
    asm( "rdtsc" : "=a" (lo), "=d" (hi) ); 
    return( lo | (hi << 32) );
}

static void runBenchmark(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette) {
    uint64_t startTicks = getProcTicks();
    for(int i = 0; i < BENCH_ITERATIONS; i++)
        mandelbrot(surface, camera, fractal, palette);
    uint64_t endTicks = getProcTicks();
    uint64_t simpleTime = endTicks - startTicks;

    printf("Benchmark for %d iterations without optimization: %llu ticks\n", BENCH_ITERATIONS, simpleTime);

    startTicks = getProcTicks();
    for(int i = 0; i < BENCH_ITERATIONS; i++)
        mandelbrotAVX(surface, camera, fractal, palette);
    endTicks = getProcTicks();
    uint64_t avxTime = endTicks - startTicks;

    printf("Benchmark for %d iterations with AVX-512:         %llu ticks\n", BENCH_ITERATIONS, avxTime);

    printf("AVX-512 is faster in %.02f times\n", (float) simpleTime / (float) avxTime);
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    initFps();

    SDL_Window* window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Event event;
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    struct Camera camera = {WINDOW_WIDTH, WINDOW_HEIGHT, INIT_SCALE, 0.0f, 0.0f};

    bool AVX = false;
    bool fpsVisible = true;
    bool animation = false;
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
                        case SDLK_t:
                            fractal = (fractal + 1) % FRACTAL_COUNT;
                            break;
                        case SDLK_p:
                            palette = (palette + 1) % PALETTE_COUNT;
                            break;
                        case SDLK_a:
                            animation = !animation;
                            break;
                        case SDLK_f:
                            fpsVisible = !fpsVisible;
                            break;
                        case SDLK_g:
                            generatePalettes();
                            break;
                        case SDLK_b:
                            runBenchmark(surface, &camera, fractal, getPalette(palette));
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

        if(fpsVisible) {
            static uint32_t startTime = 0;
            uint32_t endTime = getMicros();
            uint32_t fps = 1000000 / (endTime - startTime);
            startTime = getMicros();
            showFps(surface, &camera, fps);
        }

        if(animation) {
            static uint32_t lastAnimationTime = 0;
            uint32_t currentTime = SDL_GetTicks();
            if(currentTime - lastAnimationTime >= ANIMATION_MILLIS) {
                lastAnimationTime = currentTime;
                paletteAnimationShift();
            }
        }

        SDL_UpdateWindowSurface(window);
    }

    quitFps();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

