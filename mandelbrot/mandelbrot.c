#include "mandelbrot.h"

static const char  ALPHA = 24;
static const char  BLUE  = 16;
static const char  GREEN =  8;
static const char  RED   =  0;

static const int   N_MAX  = 255;
static const float R2_MAX = 10.0f;

void mandelbrot(SDL_Surface *surface, const struct Camera *camera) {
    int windowCenterX = camera->windowWidth  / 2;
    int windowCenterY = camera->windowHeight / 2;

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    for (int ywin = -windowCenterX; ywin < camera->windowHeight - windowCenterX; ywin++) {
        for (int xwin = - windowCenterY; xwin < camera->windowWidth - windowCenterY; xwin++) {

            float x0 = xwin / camera->scale - camera->centerPositionX;
            float y0 = ywin / camera->scale - camera->centerPositionY;

            float x = x0, y = y0;

            int N = 0;

            for(int i = 0; i <= N_MAX; i++) {
                float x2 = x * x;
                float y2 = y * y;

                float xy = x * y;

                N = (x2 + y2 > R2_MAX) ? 255 : 0;

                x = x2 - y2 + x0;
                y = 2 * xy + y0;

            }

            uint32_t color = (0xff << ALPHA) + (N << RED) + (N << GREEN) + (N << BLUE);

            *(pixels++) = color;
        }
    }

    SDL_UnlockSurface(surface);
}


void mandelbrotAVX(SDL_Surface *surface, const struct Camera *camera) {
    int windowCenterX = camera->windowWidth  / 2;
    int windowCenterY = camera->windowHeight / 2;

    int sizeInPixels = camera->scale;

    SDL_LockSurface(surface);

    uint32_t *pixels = (uint32_t*)surface->pixels;

    __m512 shiftX = _mm512_set_ps(15.0f, 14.0f, 13.0f, 12.0f, 11.0f, 10.0f,
            9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f);
    __m512 scale  = _mm512_set1_ps(camera->scale);
    shiftX = _mm512_div_ps(shiftX, scale);
    __m512 maxR2 = _mm512_set1_ps(R2_MAX);


    for (int ywin = -windowCenterX; ywin < camera->windowHeight - windowCenterX; ywin++) {
        uint32_t *pixelRow = pixels;
        pixels += camera->windowWidth;
        for (int xwin = - windowCenterY; xwin < camera->windowWidth - windowCenterY; xwin+=16) {

            __m512 x0 = _mm512_set1_ps(xwin / camera->scale - camera->centerPositionX);
            __m512 y0 = _mm512_set1_ps(ywin / camera->scale - camera->centerPositionY);
            x0 = _mm512_add_ps(x0, shiftX);

            __m512 x = x0, y = y0;

            __mmask16 mask;

            for(int i = 0; i <= N_MAX; i++) {
                __m512 x2 = _mm512_mul_ps(x, x);
                __m512 y2 = _mm512_mul_ps(y, y);
                __m512 xy = _mm512_mul_ps(x, y);
                __m512 r2 = _mm512_add_ps(x2, y2);
                mask = _mm512_cmp_ps_mask(r2, maxR2, _CMP_LE_OQ);

                x = _mm512_add_ps(x0, _mm512_sub_ps(x2, y2));
                y = _mm512_add_ps(y0, _mm512_add_ps(xy, xy));
            }

            __m512i colors = _mm512_maskz_set1_epi32(mask, 0xFFFFFFFF);

            _mm512_storeu_epi32(pixelRow, colors);
            pixelRow += 16;
        }
    }

    SDL_UnlockSurface(surface);
}



