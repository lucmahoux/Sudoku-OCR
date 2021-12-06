#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#ifndef GREYSCALE_H
#define GREYSCALE_H

SDL_Surface ToGreyscale(SDL_Surface *);
SDL_Surface Blur(SDL_Surface *);

#endif