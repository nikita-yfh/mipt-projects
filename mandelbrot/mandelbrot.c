#include "mandelbrot.h"

static const char  ALPHA = 24;
static const char  BLUE  = 16;
static const char  GREEN =  8;
static const char  RED   =  0;

static const int   N_MAX  = 256;
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

            while (N < N_MAX - 1)
            {
                float x2 = x * x;
                float y2 = y * y;

                if (x2 + y2 > R2_MAX)
                    break;

                float xy = x * y;

                x = x2 - y2 + x0;
                y = 2 * xy + y0;

                N++;
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

    uint32_t* pixels = (uint32_t*)surface->pixels;

    __m512 shiftX = _mm512_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f,
                                  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f);
    __m512 scale  = _mm512_set1_ps(camera->scale);
    shiftX = _mm512_div_ps(shiftX, scale);
    __m512 maxR2 = _mm512_set1_ps(R2_MAX);


    for (float y0 = -windowCenterY / camera->scale; y0 < (camera->windowHeight - windowCenterY) / camera->scale; y0 += 1.0f / camera->scale) {
        for (float x0 = -windowCenterX / camera->scale; x0 < (camera->windowWidth - windowCenterX) / camera->scale; x0 += 16.0f / camera->scale) {

            __m512 xcoords = _mm512_set1_ps(x0);
            __m512 ycoords = _mm512_set1_ps(y0);
            xcoords = _mm512_add_ps(xcoords, shiftX);

            __m512 x = xcoords, y = ycoords;

            int N = 0;

            while (N < N_MAX - 1)
            {
                __m512 x2 = _mm512_mul_ps(x, x);
                __m512 y2 = _mm512_mul_ps(y, y);
                __m512 xy = _mm512_mul_ps(x, y);
                __m512 r2 = _mm512_add_ps(x2, y2);
                _mm512_cmp_ps_mask(r2, maxR2, _CMP_GT_OQ);

                x = _mm512_add_ps(x, _mm512_sub_ps(x2, y2));
                y = _mm512_add_ps(y, _mm512_add_ps(xy, xy));

                N++;
            }

            uint32_t color = (0xff << ALPHA) + (N << RED) + (N << GREEN) + (N << BLUE);

            *(pixels++) = color;
        }
    }

    SDL_UnlockSurface(surface);
}


