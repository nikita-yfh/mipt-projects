#include "palette.h"
#include <assert.h>
#include <string.h>
#include <math.h>

#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF

#define ALPHA 24
#define BLUE  16
#define GREEN 8
#define RED   0

static uint32_t palettes[PALETTE_COUNT][MAX_N + 1];

static void generatePaletteGrey() {
    uint32_t *palette = palettes[PALETTE_GREY];

    uint32_t color     = WHITE;
    uint32_t colorInc  = 0x00010101;
    for(int i = 0; i <= MAX_N; i++) {
        palette[i] = color;
        color -= colorInc;
    }
}

static void generatePaletteBW() {
    uint32_t *palette = palettes[PALETTE_BW];

    memset(palette, (int) WHITE, sizeof(uint32_t) * MAX_N);
    palette[MAX_N] = BLACK;
}

static void generatePaletteSin() {
    uint32_t *palette = palettes[PALETTE_SIN];

    for(int i = 0; i < MAX_N; i++) {
        double angle = 2.0 * M_PI * i / MAX_N;
        uint8_t r = (uint8_t) ((sin(angle)                    + 1.0) / 2.0 * 255.0f);
        uint8_t g = (uint8_t) ((sin(angle + 2.0 * M_PI / 3.0) + 1.0) / 2.0 * 255.0f);
        uint8_t b = (uint8_t) ((sin(angle + 4.0 * M_PI / 3.0) + 1.0) / 2.0 * 255.0f);

        palette[i] = (uint32_t) (0xFF << ALPHA) | (r << RED) | (g << GREEN) | (b << BLUE);
    }
}

static void generatePaletteGrey4() {
    uint32_t *palette = palettes[PALETTE_GREY4];

    for(int i = 0; i <= MAX_N; i++) {
        uint8_t color = (uint8_t) (sqrt(sqrt((float) i / MAX_N)) * 255.0f);
        palette[i] = (uint32_t) (0xFF << ALPHA) | (color << RED) | (color << GREEN) | (color << BLUE);
    }
}

void generatePalettes() {
    generatePaletteGrey();
    generatePaletteBW();
    generatePaletteSin();
    generatePaletteGrey4();
}

const uint32_t *getPalette(enum PaletteType palette) {
    assert(palette < PALETTE_COUNT);

    return palettes[palette];
}

