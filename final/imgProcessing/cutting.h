#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"
#ifndef CUTIMG_H
#define CUTIMG_H

SDL_Surface** GridCut(SDL_Surface* grid);
SDL_Surface* Resize(SDL_Surface* gridCase);

#endif