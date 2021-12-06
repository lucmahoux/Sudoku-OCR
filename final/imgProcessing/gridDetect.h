#ifndef GRIDDETECT_H
#define GRIDDETECT_H

#include "detect.h"
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"

void LabelImage(SDL_Surface *image, unsigned int **label,
     unsigned int *maxlab);
void Link(int x, int y, unsigned int *labels);
int Find(int x, unsigned int *labels);
unsigned int FindLargestCluster(unsigned int **labels, int lines, int cols,
    unsigned int nbClusters);
SDL_Surface *IsolateGrid(unsigned int biggest, unsigned int **labels,
     int w, int h);
SDL_Surface *CreateIsolatedGrid(Line *borders, SDL_Surface *fullImage);
int CalcuateDistance(Line start, Line end);
Point GetIntersect(Line l1, Line l2);

SDL_Surface *GridDetect(SDL_Surface *image);

#endif