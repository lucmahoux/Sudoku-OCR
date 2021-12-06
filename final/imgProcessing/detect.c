#include "detect.h"
#include "../utils.h"
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include <math.h>
#include "adaptativeThreshold.h"
#include "gridDetect.h"

#define MAX_THETA 90
#define MIN_THETA 0
#define ANGLE_STEP 0.5
#define DISTANCE_STEP 1
#define PI 3.1415926

/**
 * \brief Fill Hough space with data from image
 * \param image The treated image
 * \param nAngle Number of theta values to be tested
 * \param nRho Number of rho values to be tested
 * \param table Array containing the parameters space
 * \sa FillSpace
 */
void FillSpace(SDL_Surface *image, int nAngle, int nRho, unsigned int **table){
    int w,h;
    w = image->w; h = image->h;
    Uint32 pixel;
    Uint8 gValue;
    for(int x = 0; x < w; x++){
        for(int y = 0; y < h; y++){

            pixel = GetPixel(image,x,y);
            SDL_GetRGB(pixel,image->format,&gValue,&gValue,&gValue);
            if (gValue == 255){
                for (int i = 0; i < nAngle; i++){
                    float angle = MIN_THETA+ANGLE_STEP*((float)i);
                    float length = ((float)x)*cos(angle/180*PI)
                                        +((float)y)*sin(angle/180*PI);
                    int j = (int)length;
                    if (j < nRho && j >= 0){
                        table[i][j] = table[i][j] + 1;
                    }
                }
            }
        }
    }
}

/**
 * \brief Returns index of the largest value in 1D array
 * \param array The array of integers
 * \param eltNumber The size of the array
 * \sa FindLargestInArray
 */
size_t FindLargestInArray(unsigned int *array, size_t eltNumber){
    size_t largest = 0;
    for (size_t i = 0; i < eltNumber; i++){
        if (array[i] >= array[largest]) largest = i;
    }
    return largest;
}

/**
 * \brief Find indexes of the largest value in 2D array
 * \param table Array of integers
 * \param nbAngle Number of angles i.e one dimension of the array
 * \param nbRho Number of rho values i.e one dimension of the array
 * \param M Y position of max value
 * \param N X position of max value
 * \sa FindMaxValue
 */
void FindMaxValue(unsigned int **table, int nbAngle, int nbRho, int *M, int *N){
    unsigned int *maxPerTheta = malloc(nbAngle*sizeof(unsigned int));

    for (int i = 0; i < nbAngle; i++){
        size_t j = FindLargestInArray(table[i],nbRho);
        maxPerTheta[i] = table[i][j];
    }

    *M = FindLargestInArray(maxPerTheta,nbAngle);
    unsigned int *m = table[*M];
    *N = FindLargestInArray(m,nbRho);
    free(maxPerTheta);
}

/**
 * \brief Detect straight lines using Hough Transform
 * \param image The image to be treated
 * \param LineNb Pointer to number of straight lines detected
 * \sa Linedetector
 */
Line * Linedetector(SDL_Surface *image, int* LineNb){
    //Create Line stockage
    Line *lines = malloc(0);
    /**
     * Step 1 : Creating parameter space for Hough Transform
     */
    int w,h;
    w = image->w;
    h = image->h;
    unsigned int rho = sqrt((float)w*w+h*h);

    unsigned int nbAngle = (MAX_THETA/ANGLE_STEP)+1;
    unsigned int nbRho = (unsigned int)rho / DISTANCE_STEP+1;
    //Declare a 2-dimensional array and set it to zero
    unsigned int **table = malloc(nbAngle*sizeof(unsigned int *));
    for (size_t i = 0; i < nbAngle;i ++)
        table[i] = malloc(nbRho*sizeof(unsigned int));

    for (size_t i = 0; i < nbAngle; i++){
        for (size_t j = 0; j < nbRho; j++){
            table[i][j] = 0;
        }
    }
    /**
     * Step 2 : Fill Parameter space by applying algorithm on each edge pixel
     */
    FillSpace(image,nbAngle,nbRho,table);
    /**
     * Step 3 : Find local maxima in the parameter space
     */
    int maxTheta = 0;
    int maxRho = 0;
    unsigned int highestVote = 0;
    char o = 0;
    unsigned int threshold = 0;

    do{
        FindMaxValue(table,nbAngle,nbRho,&maxTheta,&maxRho);
        highestVote = table[maxTheta][maxRho];

        if(highestVote < threshold && o) break;
        if(!o){
            o++;
            threshold = highestVote*0.5;
        }

        float angle = maxTheta*ANGLE_STEP;
        float dist = maxRho*DISTANCE_STEP;
        if(!testMerge(lines,*LineNb,15,dist,angle,2)){
            (*LineNb)++;
            lines = realloc(lines,(*LineNb)*sizeof(Line));
            Line lineparam;
            lineparam.rho = dist;
            lineparam.theta = angle;
            lines[*LineNb-1] = lineparam;
        }
        table[maxTheta][maxRho] = 0;
        if(maxTheta > 0){
            table[maxTheta-1][maxRho] = 0;
            if(maxRho > 0){
                table[maxTheta-1][maxRho-1] = 0;
            }
            if(((unsigned int)maxRho) < nbRho-1){
                table[maxTheta-1][maxRho+1] = 0;
            }
        }
        if(((unsigned int)maxTheta) < nbAngle-1){
            table[maxTheta+1][maxRho] = 0;
            if(maxRho > 0){
                table[maxTheta+1][maxRho-1] = 0;
            }
            if(((unsigned int)maxRho) < nbRho-1){
                table[maxTheta+1][maxRho+1] = 0;
            }
        }
        if(maxRho > 0){
            table[maxTheta][maxRho-1] = 0;
        }
        if(((unsigned int)maxRho) < nbRho-1){
            table[maxTheta][maxRho+1] = 0;
        }

    }while (highestVote > threshold);

    for(unsigned int i = 0; i < nbAngle;i++){
		free(table[i]);
	}
	free(table);
    return lines;
}

/**
 * \brief Test if two lines are too close together and merge them if they are
 * \param lines Array of detected lines
 * \param upperBound size of the lines array
 * \param minDistance Distance threshold of merge
 * \param rho Rho component of a certain line
 * \param theta Theta component of a certain line
 * \param minAngleDiff Angle threshold of merge
 * \sa testMerge
 */
int testMerge(Line *lines, int upperBound, int minDistance, int rho, int theta,
    int minAngleDiff){
    for (int i = 0; i < upperBound; i++){
        if (fabs(lines[i].rho - rho) < minDistance
        && fabs(lines[i].theta - theta) < minAngleDiff){
            return 1;
        }
    }
    return 0;
}

/**
 * \brief Order Lines by their distance from origin
 * \param lines Array of lines
 * \param len Size of the lines array
 * \sa OrderLineArray
 */
void OrderLineArray(Line *lines, size_t len){
    size_t min;
    for (size_t i = 0; i < len; i++){
        min = FindMinIndex(lines,len,i);
        SwapItem(lines, i, min);
    }
}

/**
 * \brief Find index of smallest element of an array
 * \param lines Array of lines
 * \param len Size of the lines array
 * \param i Index of the element
 * \sa FindMinIndex
 */
size_t FindMinIndex(Line *lines, size_t len, size_t i){
    float rhoMin = lines[i].rho;
    size_t index = i;
    for(i = i+1; i < len; i++){
        if(lines[i].rho < rhoMin){
            rhoMin = lines[i].rho;
            index = i;
        }
    }
    return index;
}

/**
 * \brief Swap two item in an array
 * \param lines Array of lines
 * \param i Index of an element
 * \param j Index of an element
 * \sa SwapItem
 */
void SwapItem(Line *lines, size_t i, size_t j){
    Line temp = lines[i];
    lines[i] = lines[j];
    lines[j] = temp;
}

/**
 * \brief Separate lines according to their orientation
 * \param lines Array of lines
 * \param size Size of the array
 * \param margin Angle defining if a line is vertical or horizontal
 * \param vertSize Size of the array of vertical lines
 * \param horSize Size of the array of horizontal lines
 * \sa Separator
 */
LineTuple Separator(Line *lines, size_t size, float margin,
    size_t *vertSize, size_t *horSize){
    for (size_t i = 0; i < size; i++){
        if (fabs(lines[i].theta) >= margin) {
            (*horSize)++;
        } else {
            (*vertSize)++;
        }
    }
    Line *vertLines = malloc((*vertSize)*sizeof(Line));
    Line *horLines = malloc((*horSize)*sizeof(Line));
    int indexV = 0, indexH = 0;
    for (size_t i = 0; i < size; i++){
        if (fabs(lines[i].theta) >= margin){
            horLines[indexH] = lines[i];
            indexH++;
        } else {
            vertLines[indexV] = lines[i];
            indexV++;
        }
    }
    LineTuple tuple;
    tuple.horizontal = horLines;
    tuple.vertical = vertLines;
    return tuple;
}

/**
 * \brief Cut grid of the original image and isolate it on another
 * \param image The image to be cutted
 * \param isolated The image containing the isolated grid
 * \sa CutImage
 */
SDL_Surface *CutImage(SDL_Surface *image, SDL_Surface *isolated, SDL_Rect *cut,
int *size){
    InvertColors(image);

    SDL_Surface *img = CutBorder(isolated, image,cut,size);

    SDL_FreeSurface(image);

    return img;
}

/**
 * \brief Remove border of the grid on a square image
 * \param image Square image containing the isolated grid
 * \param toCut Square image on which we need to remove borders
 * \sa CutBorder
 */
SDL_Surface *CutImageNoInvert(SDL_Rect *cut, SDL_Surface *image, int *size){
    SDL_Surface *cutted = SDL_CreateRGBSurface(0,*size,*size,
    image->format->BitsPerPixel,image->format->Rmask,image->format->Gmask,
    image->format->Bmask,image->format->Amask);

    SDL_BlitSurface(image,cut,cutted,NULL);

    SDL_FreeSurface(image);
    return cutted;
}

SDL_Surface *CutBorder(SDL_Surface *image, SDL_Surface *toCut, SDL_Rect *cut,
int *size){
    int l = -1, r = -1, t = -1, b = -1;
    char firstHor = 0, firstVert = 0;
    for (int x = 0; x < image->w; x++)
    {
        for (int y = 0; y < image->h; y++)
        {
            if(GetValueOfPixel(x,y,image)){
                if(!firstHor){
                    l = x;
                    r = x;
                    firstHor = 1;
                }
                if(!firstVert){
                    t = y;
                    b = y;
                    firstVert = 1;
                }
                if(firstVert && (y > b)){
                    b = y;
                }
                if(firstHor && (x > r)){
                    r = x;
                }
                if(firstHor && (x < l)){
                    l = x;
                }
                if(firstVert && (y < t)){
                    t = y;
                }
            }
        }

    }
    *size = (r-l)>(b-t) ? (r-l) : (b-t);

    SDL_Surface *img = SDL_CreateRGBSurface(0,*size,*size,32,0,0,0,0);
    (*cut).x = l;
    (*cut).y = t;
    (*cut).w = r-l;
    (*cut).h = b-t;
    SDL_BlitSurface(toCut,cut,img,NULL);
    return img;
}

SDL_Surface *__RemoveBorder(SDL_Surface *image, int *size, SDL_Rect *cut){
    char transiTop = GetValueOfPixel(0,0,image) ? 0 : 1;
    char transiBottom = GetValueOfPixel(image->w-1,image->h-1,image) ? 0 : 1;
    for(int i = 1; i < image->w; i++){
        if(!transiTop && !GetValueOfPixel(i,i,image)){
            transiTop = 1;
        }
        else if(transiTop && GetValueOfPixel(i,i,image)){
            (*cut).x = i;
            (*cut).y = i;
            break;
        }
    }
    for(int i = image->w-1; i >= 0; i--){
        if(!transiBottom && !GetValueOfPixel(i,i,image)){
            transiBottom = 1;
        }
        else if(transiBottom && GetValueOfPixel(i,i,image)){
            (*cut).w = i - (*cut).x;
            (*cut).h = i - (*cut).y;
            break;
        }
    }
    *size = (*cut).w > (*cut).h ? (*cut).w : (*cut).h;
    SDL_Surface *final = SDL_CreateRGBSurface(0,*size,*size,32,0,0,0,0);
    SDL_BlitSurface(image,cut,final,NULL);
    SDL_FreeSurface(image);
    return final;
}

void RemoveGrid(SDL_Surface *invert, SDL_Surface *image){
    for(int x = 0; x < invert->w; x++){
        for(int y = 0; y < invert->h; y++){
            if(GetValueOfPixel(x,y,invert)){
                SetPixel(image, x, y, 0xFFFFFFFF);
            }
        }
    }
}

SDL_Surface *CenterDigit(SDL_Surface *digitImage){
    int w = digitImage->w, h = digitImage->h;
    Point centerOfImage;
    centerOfImage.x = (digitImage->w)/2;
    centerOfImage.y = (digitImage->h)/2;
    Point centerOfDigit = CenterOfDigit(digitImage);
    SDL_Surface *returned = SDL_CreateRGBSurface(0,digitImage->w,
    digitImage->h,32,0xFF000000,0x00FF0000,0x0000FF00,0);
    if(centerOfImage.x != centerOfDigit.x || centerOfImage.y != centerOfDigit.y)
    {
        int deltaY = (centerOfDigit.y-centerOfImage.y);
        int deltaX = (centerOfDigit.x-centerOfImage.x);

        for(int x = 0; x < digitImage->w; x++){
            for(int y = 0; y < digitImage->h; y++){
                if(x+deltaX < w && x+deltaX >= 0 && y+deltaY < h && y+deltaY >= 0){
                    if(!GetValueOfPixel(x+deltaX,y+deltaY,digitImage)){
                        SetPixel(returned,x,y,0x00000000);
                    }
                    else{
                        SetPixel(returned,x,y,0xFFFFFFFF);
                    }
                }
                else{
                    SetPixel(returned,x,y,0xFFFFFFFF);
                }
            }
        }
    }
    return returned;
}

Point CenterOfDigit(SDL_Surface *digitImage){
    Point center;
    int l = -1, r = -1, t = -1, b = -1;
    char firstHor = 0, firstVert = 0;
    for (int x = 0; x < digitImage->w; x++)
    {
        for (int y = 0; y < digitImage->h; y++)
        {
            if(!GetValueOfPixel(x,y,digitImage)){
                if(!firstHor){
                    l = x;
                    r = x;
                    firstHor = 1;
                }
                if(!firstVert){
                    t = y;
                    b = y;
                    firstVert = 1;
                }
                if(firstVert && (y > b)){
                    b = y;
                }
                if(firstHor && (x > r)){
                    r = x;
                }
                if(firstHor && (x < l)){
                    l = x;
                }
                if(firstVert && (y < t)){
                    t = y;
                }
            }
        }
    }
    if(l == -1 || r == -1){
        center.x = -1;
    }
    if(t == -1 || b == -1){
        center.y = -1;
    }
    if(l==r){
        center.x = l;
    }
    if(t==b){
        center.y = t;
    }
    if(l != -1 && r != -1){
        center.x = l + (r-l)/2;
    }
    if(t != -1 && b != -1){
        center.y = t + (b-t)/2;
    }
    return center;
}