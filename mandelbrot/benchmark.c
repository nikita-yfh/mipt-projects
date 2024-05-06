#include "benchmark.h"

static const int BENCH_ITERATIONS = 100; 

static uint64_t getProcTicks() { // for benchmark
    unsigned long lo, hi;
    asm( "rdtsc" : "=a" (lo), "=d" (hi) ); 
    return( lo | (hi << 32) );
}

void runBenchmark(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette) {
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
