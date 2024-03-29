#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"
#ifndef UTILS_H
#define UTILS_H

Uint32 GetPixel(SDL_Surface *surface, int x, int y);
void SetPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
int IsPixelInImage(SDL_Surface *surface, int x, int y);
int GetValueOfPixel(int x, int y, SDL_Surface *image);

#endif