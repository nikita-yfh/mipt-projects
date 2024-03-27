#pragma once

#include <stdint.h>

enum PaletteType {
    PALETTE_GREY,
    PALETTE_BW,
    PALETTE_SIN,
    PALETTE_GREY4,
    PALETTE_RAND,

    PALETTE_COUNT
};


#define MAX_N 255

/// Generate all palettes
void generatePalettes();

/// Shift all palletes
void paletteAnimationShift();

/**
 * \brief Get a specific palette
 * \param type Palette type
 * \return Palette
 **/
const uint32_t *getPalette(enum PaletteType type);

