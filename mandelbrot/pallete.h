#pragma once

#include <stdint.h>

enum PalleteType {
    PALLETE_GREY,
    PALLETE_BW,
    PALLETE_SIN,

    PALLETE_COUNT
};


#define MAX_N 255

/**
 * \brief generate all palletes
 **/
void generatePalletes();

/**
 * \brief get a specific pallete
 * \param type Pallete type
 * \return Pallete
 **/
const uint32_t *getPallete(enum PalleteType type);

