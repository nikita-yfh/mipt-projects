#include <SDL2/SDL.h>
#include <stdbool.h>
#include <sys/time.h>
#include <limits.h>

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
    uint64_t ticks[RENDER_COUNT] = {0};
    uint64_t maxTicks = 0;

    printf("%d iterations benchmark:\n", BENCH_ITERATIONS);
    for(int renderer = 0; renderer < RENDER_COUNT; renderer++) {
        uint64_t startTicks = getProcTicks();
        for(int i = 0; i < BENCH_ITERATIONS; i++)
            mandelbrotCommon(renderer, surface, camera, fractal, palette);
        uint64_t endTicks = getProcTicks();
        ticks[renderer] = endTicks - startTicks;

        printf("%30s: %llu ticks\n", getRendererName(renderer), ticks[renderer]);

        if(ticks[renderer] > maxTicks)
            maxTicks = ticks[renderer];
    }

    printf("Relative speed compatarion:\n");
    for(int renderer = 0; renderer < RENDER_COUNT; renderer++)
        printf("%30s: %10.2f\n", getRendererName(renderer), (float) maxTicks / (float) ticks[renderer]);

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

    bool fpsVisible = true;
    bool animation = false;
    enum Fractal fractal = FRACTAL_MENDELBROT;
    enum PaletteType palette = PALETTE_GREY;
    enum RenderType      renderType = RENDER_DUMB;
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
                            renderType = (renderType + 1) % RENDER_COUNT;
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

        mandelbrotCommon(renderType, surface, &camera, fractal, getPalette(palette));

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

