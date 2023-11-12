#ifndef COLORS_H
#define COLORS_H

#define COLOR_NONE     "\e[0m"
#define COLOR_BLACK    "\e[30;1m"
#define COLOR_RED      "\e[31;1m"
#define COLOR_GREEN    "\e[32;1m"
#define COLOR_YELLOW   "\e[33;1m"
#define COLOR_BLUE     "\e[34;1m"
#define COLOR_MAGENTA  "\e[35;1m"
#define COLOR_CYAN     "\e[36;1m"
#define COLOR_GRAY     "\e[37;1m"


// TODO: maybe some wrapping of those ^ macros could be nice?
//
//       ==> Like so:
//       #define TEXT_RED(text) COLOR_RED text COLOR_NONE

#endif
