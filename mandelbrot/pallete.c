#include "pallete.h"
#include <assert.h>
#include <string.h>

#define BLACK 0xFF000000
#define WHITE 0xFFFFFFFF

static uint32_t palletes[PALLETE_COUNT][MAX_N + 1];

static void generatePalleteGrey() {
    uint32_t *pallete = palletes[PALLETE_GREY];

    uint32_t color     = WHITE;
    uint32_t colorInc  = 0x00010101;
    for(int i = 0; i <= MAX_N; i++) {
        pallete[i] = color;
        color -= colorInc;
    }
}

static void generatePalleteBW() {
    uint32_t *pallete = palletes[PALLETE_BW];
    memset(pallete, WHITE, sizeof(uint32_t) * MAX_N);
    pallete[MAX_N] = BLACK;
}

static void generatePalleteSin() {
}

void generatePalletes() {
    generatePalleteGrey();
    generatePalleteBW();
}

const uint32_t *getPallete(enum PalleteType pallete) {
    assert(pallete < PALLETE_COUNT);

    return palletes[pallete];
}

