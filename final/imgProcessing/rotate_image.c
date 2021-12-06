#include "rotate_image.h"
#include "../utils.h"
#include <math.h>
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_surface.h"
#include "cutting.h"
#include "detect.h"

#define PI M_PI

/**
 * \brief Rotate the image by the given angle in degree (clockwise)
 * \param surface The image to rotate
 * \param degree The rotation applied to the image
 * \sa Rotate_Image
 */
SDL_Surface *Rotate_Image(SDL_Surface *surface, int degree){
    degree = degree % (degree > 0 ? 360 : -360);
    double radian = PI * degree / 180;

    int height = (int) (surface->h * cos(radian) + surface->w * sin(radian));
    int width = (int) (surface->h * sin(radian) + surface->w * cos(radian));
    height = abs(height);
    width = abs(width);

    SDL_Surface *result = SDL_CreateRGBSurface(0,width,height,32,0,0,0,0);
    int vert_center = height / 2;
    int hor_center = width / 2;

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            float hor_position = (float) (j - vert_center) * sin(radian)
                + (i - hor_center) * cos(radian) + surface->w / 2;
            float vert_position = (float) (j - vert_center) * cos(radian)
                + (i - hor_center) * -sin(radian) + surface->h / 2;

            int vpos = (int) vert_position;
            int hpos = (int) hor_position;
            if(IsPixelInImage(surface, hpos, vpos)) {
                SetPixel(result, i, j, GetPixel(surface, hpos, vpos));
            } else {
                SetPixel(result, i, j, 0x00000000);
            }
        }
    }
    return result;
}

/**
 * \brief Automatically rotate image to make it straight
 * \param image The image to rotate
 * \param isolated The image with only the isolated grid
 * \sa AutoRotate
 */
SDL_Surface *AutoRotate(SDL_Surface *image, SDL_Surface *isolated, int *angle){
    //Get all lines in picture
    int nbLines = 0;
    Line *lines = Linedetector(isolated, &nbLines);
    //Separate lines whether they are vertical or horizontal
    size_t vertSize = 0, horSize = 0;
    LineTuple tuple = Separator(lines,nbLines,45,&vertSize,&horSize);

    RemoveExtremes(tuple.vertical,vertSize);

    float meanAngleValueVert = MeanAngle(tuple.vertical,vertSize);
    *angle = 90 - (int)ceil(meanAngleValueVert);

    free(tuple.horizontal);
    free(tuple.vertical);
    free(lines);
    SDL_Surface *returned = Rotate_Image(image,*angle);
    SDL_FreeSurface(image);
    return returned;
}

/**
 * \brief Calculate the mean angle value of the lines given
 * \param lines A pointer of line
 * \param size Size of lines
 * \sa MeanAngle
 */
float MeanAngle(Line *lines, size_t size){
    int mean = 0;
    for (size_t i = 0; i < size; i++)
        if(lines[i].theta != 10000)
            mean += (int)lines[i].theta;
    return size == 0 ? 45 : (float)mean/(float)(size - 1);
}

/**
 * \brief "Remove" extremes angle values from lines
 * \param lines A pointer of line
 * \param size Size of lines
 * \sa RemoveExtremes
 */
void RemoveExtremes(Line *lines, size_t size){
    if(size != 0){
        float max = lines[0].theta, min = lines[0].theta;
        int indexMax = 0, indexMin = 0;
        for(size_t i = 1; i < size; i++){
            if (lines[i].theta > max){
                max = lines[i].theta;
                indexMax = i;
            }
            if (lines[i].theta < min){
                min = lines[i].theta;
                indexMin = i;
            }
        }
        lines[indexMax].rho = 10000;
        lines[indexMax].theta = 10000;
        lines[indexMin].rho = 10000;
        lines[indexMin].theta = 10000;
    }
}