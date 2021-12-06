#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#ifndef ADAPTATIVETHRESHOLD_H
#define ADAPTATIVETHRESHOLD_H

SDL_Surface Dilate(SDL_Surface *image);
void InvertColors(SDL_Surface *image);
void IntegralImage(SDL_Surface *image, unsigned long **arr);
SDL_Surface *ComputeImage(SDL_Surface *image, size_t size, unsigned long t);
SDL_Surface *BlackAndWhite(SDL_Surface *image);
float MeanPixelValue(SDL_Surface *image);

#endif