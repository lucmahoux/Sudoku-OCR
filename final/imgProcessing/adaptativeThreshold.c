#include "adaptativeThreshold.h"
#include "../utils.h"
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include <math.h>

/**
 * \brief Transform B&W image to W&B image
 * \param image Image to invert
 * \sa InvertColors
 */
void InvertColors(SDL_Surface *image){
    Uint32 pixel;
    for(int x = 0; x < image->w; x++){
        for(int y = 0; y < image->h; y++){
            if(!GetValueOfPixel(x,y,image)){
                pixel = SDL_MapRGB(image->format,255,255,255);
                SetPixel(image,x,y,pixel);
            } else {
                pixel = SDL_MapRGB(image->format,0,0,0);
                SetPixel(image,x,y,pixel);
            }
        }
    }
}

/**
 * \brief Create Integral Image for Wellner's method
 * \param image Image to be computed
 * \param arr Two dimensionnal array to stock Integral Image
 * \sa IntegralImage
 */
void IntegralImage(SDL_Surface *image, unsigned long **arr){
    unsigned long greyValue = 0;

    for(int i = 0; i < image->w; i++){
        for (int j = 0; j < image->h; j++)
        {
            greyValue = (unsigned long)GetValueOfPixel(i,j,image);
            if(i > 0){
                greyValue += arr[i-1][j];
                if(j > 0)
                    greyValue -= arr[i-1][j-1];
            }
            if(j > 0)
                greyValue += arr[i][j-1];
            arr[i][j] = greyValue;
        }
    }
}

/**
 * \brief Compute B&W image using Wellner's method
 * \param image Image to be computed
 * \param size Size of the window used for the method
 * \param t Threshold value for the method
 * \sa ComputeImage
 */
SDL_Surface *ComputeImage(SDL_Surface *image, size_t size, unsigned long t){
    size_t w = image->w, h = image->h;
    Uint32 pixel;

    //Initialize 2-dimensional array for Integral Image
    unsigned long **arr = malloc(w*sizeof(unsigned long *));
    for(size_t i = 0; i < w; i++)
        arr[i] = malloc(h*sizeof(unsigned long));

    for(size_t i = 0; i < w; i++)
        for(size_t j = 0; j < h; j++)
            arr[i][j] = 0;

    IntegralImage(image, arr);

    size_t x1, x2, y1, y2, count;
    unsigned long sum;
    for(size_t x = 0; x < w; x++){
        for (size_t y = 0; y < h; y++){
            x1 = x < (size/2)+1 ? (x == 0 ? x+1 : x) : x - size/2;
            x2 = x >= w - size/2 ? x : x + size/2;
            y1 = y < (size/2)+1 ? (y==0 ? y+1 : y) : y - size/2;
            y2 = y >= h - size/2 ? y : y + size/2;
            count = (x2-x1)*(y2-y1);
            sum = arr[x2][y2]-arr[x2][y1-1]-arr[x1-1][y2]+arr[x1-1][y1-1];

            if(GetValueOfPixel(x,y,image)*count <= sum*(100-t)/100){
                pixel = SDL_MapRGB(image->format,0,0,0);
                SetPixel(image, x, y, pixel);
            }
            else{
                pixel = SDL_MapRGB(image->format,255,255,255);
                SetPixel(image, x, y, pixel);
            }
        }
    }
    for(size_t i = 0; i < w; i++)
        free(arr[i]);
    free(arr);

    *image = Dilate(image);

    return image;
}

/**
 * \brief Dilate foreground objects
 * \param image Image to dilate
 * \sa Dilate
 */
SDL_Surface Dilate(SDL_Surface *image){
    Uint32 pixel;
    Uint8 grey;
    SDL_Surface *copy;

    char flag = 1;

    copy = SDL_CreateRGBSurface(0,image->w,image->h,
    image->format->BitsPerPixel,image->format->Rmask,image->format->Gmask,
    image->format->Bmask,image->format->Amask);

    SDL_BlitSurface(image,NULL,copy,NULL);
    SDL_LockSurface(image);
    SDL_LockSurface(copy);

    for(int x = 0; x < image->w; x++){
        for(int y = 0; y < image->h; y++){
            if(x == 0 || x == image->w-1 || y == 0 || y == image->h-1){
                pixel = SDL_MapRGB(image->format,0,0,0);
                SetPixel(image,x,y,pixel);
                continue;
            }
            for(int a = -1; a < 2; a++){
                for(int b = -1; b < 2; b++){
                    pixel = GetPixel(copy,x+a,y+b);
                    SDL_GetRGB(pixel,copy->format,&grey,&grey,&grey);
                    if(grey == 0){
                        flag = 0;
                        pixel = SDL_MapRGB(image->format,0,0,0);
                        SetPixel(image,x,y,pixel);
                        break;
                    }
                }
                if(!flag) break;
            }
            if (flag == 1){
                pixel = SDL_MapRGB(image->format,255,255,255);
                SetPixel(image,x,y,pixel);
            }
        }
    }
    SDL_UnlockSurface(copy);
    SDL_UnlockSurface(image);
    SDL_FreeSurface(copy);
    return *image;
}

/**
 * \brief Transform a colored imaged to black and white
 * \param image Image to transform
 * \sa BlackAndWhite
 */
SDL_Surface *BlackAndWhite(SDL_Surface *image){
    SDL_Surface *copy = SDL_CreateRGBSurface(0,image->w,image->h,
    image->format->BitsPerPixel,image->format->Rmask,image->format->Gmask,
    image->format->Bmask,image->format->Amask);

    SDL_BlitSurface(image,NULL,copy,NULL);

    size_t size = (image->w)/32;
    SDL_Surface *firstResult = ComputeImage(image,size,20);
    float mean = MeanPixelValue(firstResult);
    if(mean < 198){
        SDL_FreeSurface(firstResult);
        SDL_Surface *secondResult = ComputeImage(copy,size,60);
        return secondResult;
    }
    else{
        SDL_FreeSurface(copy);
        return firstResult;
    }
}

/**
 * \brief Check if BlackAndWhite is well done
 * \param image Image to check
 * \sa MeanPixelValue
 */
float MeanPixelValue(SDL_Surface *image){
    int w = image->w, h = image->h;
    unsigned long nb = w*h;
    float mean = 0;
    for(int x = 0; x < w; x++){
        for(int y = 0; y < h; y++){
            mean += (float)GetValueOfPixel(x,y,image)/nb;
        }
    }
    return mean;
}