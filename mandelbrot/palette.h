#pragma once

#include <stdint.h>

enum PaletteType {
    PALETTE_GREY,
    PALETTE_BW,
    PALETTE_SIN,

    PALETTE_COUNT
};


#define MAX_N 255

/**
 * \brief generate all palettes
 **/
void generatePalettes();

/**
 * \brief get a specific palette
 * \param type Palette type
 * \return Palette
 **/
const uint32_t *getPalette(enum PaletteType type);

