#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "instruction.h"

int graphicsInit(void *mem, arg_t width, arg_t height);
int graphicsQuit();

const char *graphicsGetError();

int graphicsUpdate();

#endif
