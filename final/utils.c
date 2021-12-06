#include "utils.h"
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"

/**
 * \brief Get a pixel on the surface at position (x,y)
 * \param surface Donor image
 * \param x x value of the pixel
 * \param y y value of the pixel
 * \sa GetPixel
 */
Uint32 GetPixel(SDL_Surface *surface, int x, int y){

    Uint8 bytePerPixel = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bytePerPixel;

    switch(bytePerPixel){
        case 1:
            return *p;
        case 2:
            return *(Uint16 *)p;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32 *)p;
        default:
            return 0;
    }
}

/**
 * \brief Set color of pixel at position (x,y)
 * \param surface Receiver image
 * \param x X value of the pixel
 * \param y Y value of the pixel
 * \param pixel Pixel to put on image
 * \sa SetPixel
 */
void SetPixel(SDL_Surface *surface, int x, int y, Uint32 pixel){

    Uint8 bytePerPixel = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bytePerPixel;

    switch(bytePerPixel){
        case 1:
            *p = pixel;
            break;
        case 2:
            *(Uint16 *)p = pixel;
            break;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN){
                p[0] = (pixel >> 16) & 0xFF;
                p[1] = (pixel >> 8) & 0xFF;
                p[2] = pixel & 0xFF;
            }
            else{
                p[0] = pixel & 0xFF;
                p[1] = (pixel >> 8) & 0xFF;
                p[2] = (pixel >> 16) & 0xFF;
            }
            break;
        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

/**
 * \brief Check if pixel (x,y) is in the image
 * \param surface Tested image
 * \param x X value of the pixel
 * \param y Y value of the pixel
 * \sa IsPixelInImage
 */
int IsPixelInImage(SDL_Surface *surface, int x, int y){
    return 0 <= x && x < surface->w && 0 <= y && y < surface->h;
}

/**
 * \brief Return grey value of pixel
 * \param surface Donor image
 * \param x X value of the pixel
 * \param y Y value of the pixel
 * \sa GetValueOfPixel
 */
int GetValueOfPixel(int x, int y, SDL_Surface *image){
    Uint32 pixel = GetPixel(image,x,y);
    Uint8 grey;
    SDL_GetRGB(pixel,image->format,&grey,&grey,&grey);
    return grey;
}
