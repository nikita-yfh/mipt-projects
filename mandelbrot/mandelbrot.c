#include "mandelbrot.h"

static const char  ALPHA = 24;
static const char  BLUE  = 16;
static const char  GREEN =  8;
static const char  RED   =  0;

static const int   N_MAX  = 255;
static const float R2_MAX = 1000.0f;

void mandelbrot(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal) {
    int windowCenterX = camera->windowWidth  / 2;
    int windowCenterY = camera->windowHeight / 2;

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    for (int ywin = -windowCenterY; ywin < camera->windowHeight - windowCenterY; ywin++) {
        for (int xwin = - windowCenterX; xwin < camera->windowWidth - windowCenterX; xwin++) {

            float x0 = xwin / camera->scale - camera->centerPositionX;
            float y0 = ywin / camera->scale - camera->centerPositionY;

            float x = x0, y = y0;

            int N = 0;

            for(int i = 0; i < N_MAX; i++) {
                float x2 = x * x;
                float y2 = y * y;
                if(x2 + y2 > R2_MAX)
                    break;

                float xy = x * y;
                if(fractal == FRACTAL_BURNING_SHIP)
                    xy = fabs(xy);

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


void mandelbrotAVX(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal) {
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


    for (int ywin = -windowCenterY; ywin < camera->windowHeight - windowCenterY; ywin++) {
        uint32_t *pixelRow = pixels;
        pixels += camera->windowWidth;

        __m512 y0 = _mm512_set1_ps(ywin / camera->scale - camera->centerPositionY);

        for (int xwin = - windowCenterX; xwin < camera->windowWidth - windowCenterX; xwin+=16) {
            __m512 x0 = _mm512_set1_ps(xwin / camera->scale - camera->centerPositionX);
            x0 = _mm512_add_ps(x0, shiftX);

            __m512 x = x0, y = y0;

            __m512i colors = _mm512_set1_epi32(0x00000000);
            __mmask16 availablePixels = 0xFFFF;

            uint32_t currentColor = 0xFFFFFFFF;
            uint32_t colorInc     = 0x00010101;

            for(int i = 0; i < N_MAX; i++) {
                __m512 x2 = _mm512_mul_ps(x, x);
                __m512 y2 = _mm512_mul_ps(y, y);
                __m512 xy = _mm512_mul_ps(x, y);
                if(fractal == FRACTAL_BURNING_SHIP)
                    xy = _mm512_abs_ps(xy);

                __m512 r2 = _mm512_add_ps(x2, y2);
                __mmask16 lessR2 = _mm512_cmp_ps_mask(r2, maxR2, _CMP_LE_OQ);
                __mmask16 change = lessR2 ^ availablePixels;
                availablePixels &= lessR2;
                colors = _mm512_mask_set1_epi32(colors, change, currentColor);

                if(!availablePixels)
                    break;

                x = _mm512_add_ps(x0, _mm512_sub_ps(x2, y2));
                y = _mm512_add_ps(y0, _mm512_add_ps(xy, xy));

                currentColor -= colorInc;
            }

            _mm512_storeu_epi32(pixelRow, colors);
            pixelRow += 16;
        }
    }

    SDL_UnlockSurface(surface);
}

void mandelbrotAVXDouble(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal) {
    int windowCenterX = camera->windowWidth  / 2;
    int windowCenterY = camera->windowHeight / 2;

    int sizeInPixels = camera->scale;

    SDL_LockSurface(surface);

    uint32_t *pixels = (uint32_t*)surface->pixels;

    __m512d shiftX = _mm512_set_pd(7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f);
    __m512d scale  = _mm512_set1_pd(camera->scale);
    shiftX = _mm512_div_pd(shiftX, scale);
    __m512d maxR2 = _mm512_set1_pd(R2_MAX);


    for (int ywin = -windowCenterY; ywin < camera->windowHeight - windowCenterY; ywin++) {
        uint32_t *pixelRow = pixels;
        pixels += camera->windowWidth;

        __m512d y0 = _mm512_set1_pd(ywin / (double) camera->scale - (double) camera->centerPositionY);

        for (int xwin = - windowCenterX; xwin < camera->windowWidth - windowCenterX; xwin+=8) {
            __m512d x0 = _mm512_set1_pd(xwin / (double) camera->scale - (double) camera->centerPositionX);
            x0 = _mm512_add_pd(x0, shiftX);

            __m512d x = x0, y = y0;

            __m256i colors = _mm256_set1_epi32(0x00000000);
            __mmask8 availablePixels = 0xFF;

            uint32_t currentColor = 0xFFFFFFFF;
            uint32_t colorInc     = 0x00010101;

            for(int i = 0; i < N_MAX; i++) {
                __m512d x2 = _mm512_mul_pd(x, x);
                __m512d y2 = _mm512_mul_pd(y, y);
                __m512d xy = _mm512_mul_pd(x, y);
                if(fractal == FRACTAL_BURNING_SHIP)
                    xy = _mm512_abs_pd(xy);

                __m512d r2 = _mm512_add_pd(x2, y2);
                __mmask8 lessR2 = _mm512_cmp_pd_mask(r2, maxR2, _CMP_LE_OQ);
                __mmask8 change = lessR2 ^ availablePixels;
                availablePixels &= lessR2;
                colors = _mm256_mask_set1_epi32(colors, change, currentColor);

                if(!availablePixels)
                    break;

                x = _mm512_add_pd(x0, _mm512_sub_pd(x2, y2));
                y = _mm512_add_pd(y0, _mm512_add_pd(xy, xy));

                currentColor -= colorInc;
            }

            _mm256_storeu_epi32(pixelRow, colors);
            pixelRow += 8;
        }
    }

    SDL_UnlockSurface(surface);
}



