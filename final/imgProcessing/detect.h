#ifndef DETECT_H
#define DETECT_H

#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"

typedef struct Line Line;
struct Line
{
    float theta;
    float rho;
};

typedef struct Point Point;
struct Point{
    int x;
    int y;
};

typedef struct LineTuple LineTuple;
struct LineTuple{
    Line *vertical;
    Line *horizontal;
};

void FillSpace(SDL_Surface *image, int nAngle, int nRho, unsigned int **table);
Line * Linedetector(SDL_Surface *image, int* LineNb);
void FindMaxValue(unsigned int **table, int nbAngle, int nbRho, int *M, int *N);
size_t FindLargestInArray(unsigned int *array, size_t eltNumber);
int testMerge(Line *lines, int upperBound, int minDistance, int rho,
int theta, int minAngleDiff);
void OrderLineArray(Line *lines, size_t len);
size_t FindMinIndex(Line *lines, size_t len, size_t i);
void SwapItem(Line *lines, size_t i, size_t j);
LineTuple Separator(Line *lines, size_t size, float margin,
    size_t *vertSize, size_t *horsize);
SDL_Surface *CutImage(SDL_Surface *image, SDL_Surface *isolated, SDL_Rect *cut,
int *size);
SDL_Surface *RemoveBorder(SDL_Surface *image, int w);
SDL_Surface *Isolate(SDL_Surface *image, SDL_Surface *toCut);
SDL_Surface *CutImageNoInvert(SDL_Rect *cut, SDL_Surface *image, int *size);
SDL_Surface *CutBorder(SDL_Surface *image, SDL_Surface *toCut, SDL_Rect *cut,
int *size);
void RemoveGrid(SDL_Surface *invert, SDL_Surface *image);
SDL_Surface *CenterDigit(SDL_Surface *digitImage);
Point CenterOfDigit(SDL_Surface *digitImage);
SDL_Surface *__RemoveBorder(SDL_Surface *image, int *size, SDL_Rect *cut);

#endif