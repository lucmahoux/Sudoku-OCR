#include "greyscale.h"
#include "../utils.h"
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_stdinc.h"

/**
 * \brief Transform image in greyscale image
 * \param surface Image to transform
 * \sa ToGreyscale
 */
SDL_Surface ToGreyscale(SDL_Surface *surface){
    Uint32 pixel;
    Uint8 r,g,b,a;
    Uint8 grey;

    SDL_LockSurface(surface); //Block surface

    for(int y = 0; y < surface->h; y++){
        for(int x = 0; x < surface->w; x++){
            pixel = GetPixel(surface, x, y);
            /*Get component of pixel and modify them*/
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
            r *= 0.2126;
            g *= 0.7152;
            b *= 0.0722;
            grey = r+g+b;
            pixel = SDL_MapRGBA(surface->format, grey, grey, grey, a);
            SetPixel(surface, x, y, pixel);
        }
    }
    SDL_UnlockSurface(surface); //Unlock surface for later use
    return *surface;
}