#ifndef __UTIL_DEFINED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define __UTIL_DEFINED

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
// #define pxToPt(px) ((px)*0.75)

enum FONT_TYPE {
    FONT_RUBIK_LIGHT,
    FONT_RUBIK_MEDIUM,
    FONT_RUBIK_REGULAR,
    FONT_RUBIK_BOLD,
    FONT_TYPES,
};
typedef enum FONT_TYPE FONT_TYPE;

// General purpose

int randomBetween(int min, int max);

// String related

char *substr(const char *buffer, unsigned int start, unsigned int end);

// SDL related

#define rectangle(x, y, width, height) ((SDL_Rect){x, y, width, height})

typedef struct Color {
    SDL_Color rgb;
    Uint32 value;
} Color;

Color mapColor(const SDL_Surface *surface, const char *hexColor);
TTF_Font *getFont(FONT_TYPE type, int size);
void throwSDLError(const char *functionName);

#endif
