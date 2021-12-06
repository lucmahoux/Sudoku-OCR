#include "cutting.h"
#include "../utils.h"
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"

/**
 * \brief Cut the given grid in little squares (The Grid MUST BE a square)
 * \param grid Image to cut
 * \sa GridCut
 */
SDL_Surface** GridCut(SDL_Surface* grid){
    if(grid == NULL){
        return NULL;
    }
    int nCut = 9;
    SDL_Surface** result = malloc(nCut * nCut * sizeof(grid));
    int sSize = (grid->w / nCut) + 1;

    for (int i = 0; i < nCut; i++){
        for (int j = 0; j < nCut; j++){

            int act = i * nCut + j;
            result[act] = SDL_CreateRGBSurface(0,sSize, sSize,32,0,0,0,0);
            for (int y = 0; y < sSize ; y++){
                for (int x = 0; x < sSize; x++){

                    int xGrid = j * sSize + x;
                    int yGrid = i * sSize + y;
                    if (IsPixelInImage(grid, xGrid, yGrid))
                    {
                        SetPixel(result[act], x, y,
                            GetPixel(grid, xGrid, yGrid));
                    } else {
                        SetPixel(result[act], x, y, 0xFFFFFFFF);
                    }
                }
            }
        }
    }
    return result;
}

/**
 * \brief Resize a grid case in the format accepted by the neural network
 * \param gridCase A case of the grid to resize
 * \sa Resize
 */
SDL_Surface* Resize(SDL_Surface* gridCase){
	if(gridCase == NULL){
		return NULL;
	}
    int wLen = 28;
    float ratio = gridCase->w / wLen;
	SDL_Surface* result = SDL_CreateRGBSurface(0,wLen,wLen,32,0,0,0,0);
	for(int y = 0; y < wLen; y++){
        for(int x = 0; x < wLen; x++){
            int startX = x * ratio;
            int startY = y * ratio;
            int cpt = 0;
            for (int y1 = 0; y1 < ratio; y1++){
                for (int x1 = 0; x1 < ratio; x1++){
                    if(IsPixelInImage(gridCase, startX, startY))
                        cpt += GetPixel(gridCase, startX+x1,
                                    startY+y1) == 0x00000000;
                }
            }
            int limit = (int)(ratio * ratio) / 2;
            SetPixel(result, x, y, cpt > limit ? 0x00000000 : 0xFFFFFFFF);
        }
    }
    for (int i = 0; i < wLen; i++){
        for (int j = 0; j < wLen; j++){
            if (GetPixel(result, i, j) == 0x00000000){
                int cpt = 0;
                if(i != 0){
                    cpt += GetPixel(result, i-1, j) == 0x00000000;
                }
                if(!cpt && j != 0){
                    cpt += GetPixel(result, i, j-1) == 0x00000000;
                }
                if(!cpt && i != wLen-1){
                    cpt += GetPixel(result, i+1, j) == 0x00000000;
                }
                if(!cpt && j != wLen-1){
                    cpt += GetPixel(result, i, j+1) == 0x00000000;
                }
                if(!cpt){
                    SetPixel(result, i, j, 0xFFFFFFFF);
                }
            }

        }
    }
    return result;
}