#include "mandelbrot.h"

static const float R2_MAX = 1000.0f;

void mandelbrot(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette) {
    int windowCenterX = camera->windowWidth  / 2;
    int windowCenterY = camera->windowHeight / 2;

    SDL_LockSurface(surface);

    uint32_t* pixels = (uint32_t*)surface->pixels;

    for (int ywin = -windowCenterY; ywin < camera->windowHeight - windowCenterY; ywin++) {
        for (int xwin = - windowCenterX; xwin < camera->windowWidth - windowCenterX; xwin++) {

            double x0 = xwin / camera->scale - camera->centerPositionX;
            double y0 = ywin / camera->scale - camera->centerPositionY;

            double x = x0, y = y0;

            int iteration = 0;

            while(iteration < MAX_N) {
                double x2 = x * x;
                double y2 = y * y;

                double xy = x * y;
                if(fractal == FRACTAL_BURNING_SHIP)
                    xy = fabs(xy);

                x = x2 - y2 + x0;
                y = 2 * xy + y0;

                if(x2 + y2 > R2_MAX)
                    break;
                iteration++;
            }

            *(pixels++) = palette[iteration];
        }
    }

    SDL_UnlockSurface(surface);
}


void mandelbrotAVX(SDL_Surface *surface, const struct Camera *camera, enum Fractal fractal, const uint32_t *palette) {
    int windowCenterX = camera->windowWidth  / 2;
    int windowCenterY = camera->windowHeight / 2;

    SDL_LockSurface(surface);

    uint32_t *pixels = (uint32_t*)surface->pixels;

    __m512d shiftX = _mm512_set_pd(7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f);
    __m512d scale  = _mm512_set1_pd(camera->scale);
    shiftX = _mm512_div_pd(shiftX, scale);
    __m512d maxR2 = _mm512_set1_pd(R2_MAX);


    for (int ywin = -windowCenterY; ywin < camera->windowHeight - windowCenterY; ywin++) {
        uint32_t *pixelRow = pixels;
        pixels += camera->windowWidth;

        __m512d y0 = _mm512_set1_pd(ywin / camera->scale - camera->centerPositionY);

        for (int xwin = - windowCenterX; xwin < camera->windowWidth - windowCenterX; xwin += 8) {
            __m512d x0 = _mm512_set1_pd(xwin / camera->scale - camera->centerPositionX);
            x0 = _mm512_add_pd(x0, shiftX);

            __m512d x = x0, y = y0;

            __m256i colors = _mm256_set1_epi32((int) palette[MAX_N]);
            __mmask8 availablePixels = 0xFF;

            for(int i = 0; i < MAX_N; i++) {
                __m512d x2 = _mm512_mul_pd(x, x);
                __m512d y2 = _mm512_mul_pd(y, y);
                __m512d xy = _mm512_mul_pd(x, y);
                if(fractal == FRACTAL_BURNING_SHIP)
                    xy = _mm512_abs_pd(xy);

                __m512d r2 = _mm512_add_pd(x2, y2);
                __mmask8 lessR2 = _mm512_cmp_pd_mask(r2, maxR2, _CMP_LE_OQ);
                __mmask8 change = lessR2 ^ availablePixels;
                availablePixels &= lessR2;
                colors = _mm256_mask_set1_epi32(colors, change, (int) palette[i]);

                if(!availablePixels)
                    break;

                x = _mm512_add_pd(x0, _mm512_sub_pd(x2, y2));
                y = _mm512_add_pd(y0, _mm512_add_pd(xy, xy));
            }

            _mm256_storeu_epi32(pixelRow, colors);
            pixelRow += 8;
        }
    }

    SDL_UnlockSurface(surface);
}



