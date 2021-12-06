#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "../utils.h"
#include "network.h"
#include "ocr_2.h"


int is_blank(float *box)
{
    int cpt = 0;
    size_t i = 10;
    size_t j = 0;
    while (i < 19 && cpt < 5)
    {
        if (box[9 + j + i * 28] == 1)
            cpt++;
        j++;
        if (j == 10)
        {
            j = 0;
            i++;
        }
    }
    return cpt < 5;
}

float *get_case_pixels(SDL_Surface *box)
{
    float *input = calloc(784, sizeof(float));
    for (size_t i = 0; i < 28*28; i++)
    {
        input[i] = GetPixel(box, i % 28, i / 28) == 0xFFFFFFFF ? 0. : 1.;
    }
    return input;
}

int *recover_grid(SDL_Surface **gridCase, Neural neural)
{
    int *output = calloc(81, sizeof(int));
    for (size_t i = 0; i < 81; i++)
    {
        float *input = get_case_pixels(gridCase[i]);
        if (is_blank(input))
		{
            output[i] = 0;
        } else {
        	int truc = query(neural, input);
        	output[i] = truc;
        }
        free(input);
    }
    return output;
}
