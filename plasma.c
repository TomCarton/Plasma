#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>


static const int kWidth = 320;
static const int kHeight = 240;

static const int kUpscale = 2;

static int gPlasmaSpeed = 1;
static int gColorScheme = 0;


void renderPlasma(uint32_t *pixels)
{
    for (int x = 0; x < kWidth; ++x)
    {
        for (int y = 0; y < kHeight; ++y)
        {
            uint32_t ticks = SDL_GetTicks();

            float time = ticks * gPlasmaSpeed * 0.001;

            float u = (((float)x / kWidth) - 0.5f) * 1.5f;
            float v = (((float)y / kHeight) - 0.5f) * 1.5f;
            float cu = u + 0.5f * sinf(time / 5);
            float cv = v + 0.5f * cosf(time / 9);

            float v1 = sinf(u * 10 + time);
            float v2 = sinf(25 * (u * sinf(time / 3) + v * cosf(time / 7)) + time);
            float v3 = sinf(sqrtf(2 * 10 * 10 * (cu * cu + cv * cv) + 1) + time);

            float gv = v1 + v2 + v3;

            Uint8 ic = 255 * (0.5f + 0.5f * cosf(gv * M_PI / 2));
            Uint8 is = 255 * (0.5f + 0.5f * sinf(gv * M_PI / 3));
            Uint8 is2 = 255 * (0.5f + 0.5f * sinf(gv * M_PI * 2));

            switch (gColorScheme)
            {
                case 0:
                    pixels[y * kWidth + x] = 255 << 16 | ic << 8 | is;
                    break;

                case 1:
                    pixels[y * kWidth + x] = ic << 16 | is << 8;
                    break;

                case 2:
                    pixels[y * kWidth + x] = is << 8 | ic;
                    break;

                case 3:
                    pixels[y * kWidth + x] = ic << 16 | is << 8 | 255;
                    break;

                case 4:
                    pixels[y * kWidth + x] = is2 << 16 | is2 << 8 | is2;
                    break;
            }
        }
    }
}

int main(int argc, char ** argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Plasma", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kWidth * kUpscale, kHeight * kUpscale, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, kWidth, kHeight);

    uint32_t *pixels = (uint32_t *)malloc(kWidth * kHeight * 4);
    memset(pixels, 0, kWidth * kHeight * sizeof(uint32_t));

    uint32_t time0 = SDL_GetTicks();

    bool quit = false;
    while (!quit)
    {
        renderPlasma(pixels);

        SDL_UpdateTexture(texture, NULL, pixels, kWidth * sizeof(uint32_t));

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    quit = true;
                    break;
                }

                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;

                        case SDLK_s:
                            if (++gPlasmaSpeed > 5)
                                gPlasmaSpeed = 1;
                            break;

                        case SDLK_c:
                            if (++gColorScheme > 4)
                                gColorScheme = 0;
                            break;
                    }

                    break;
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        uint32_t now = SDL_GetTicks();
//        printf("fps: %d\n", 1000 / (now - time0));
        time0 = now;
    }

    free(pixels);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    
    return 0;
}
