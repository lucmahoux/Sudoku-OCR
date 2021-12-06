#ifndef ROTIMG_H
#define ROTIMG_H

#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"
#include "detect.h"

SDL_Surface *Rotate_Image(SDL_Surface *surface, int degree);
SDL_Surface *AutoRotate(SDL_Surface *image, SDL_Surface *isolated, int *angle);
float MeanAngle(Line *lines, size_t size);
void RemoveExtremes(Line *lines, size_t size);

#endif