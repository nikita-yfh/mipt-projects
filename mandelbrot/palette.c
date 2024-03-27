#include "palette.h"
#include <assert.h>
#include <string.h>

#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF

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
    memset(palette, WHITE, sizeof(uint32_t) * MAX_N);
    palette[MAX_N] = BLACK;
}

static void generatePaletteSin() {
}

void generatePalettes() {
    generatePaletteGrey();
    generatePaletteBW();
}

const uint32_t *getPalette(enum PaletteType palette) {
    assert(palette < PALETTE_COUNT);

    return palettes[palette];
}

