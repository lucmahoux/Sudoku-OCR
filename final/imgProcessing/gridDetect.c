#include "gridDetect.h"
#include "../utils.h"
#include "detect.h"
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include <math.h>

/**
 * \brief Find closest cluster
 *
 * \sa Find
 */
int Find(int x, unsigned int *labels){
    int y = x;
    while(labels[y] != ((unsigned int)y))
        y = labels[y];
    while(labels[x] != ((unsigned int)x)){
        int z = labels[x];
        labels[x] = y;
        x = z;
    }
    return y;
}

/**
 * \brief Merge two cluster together
 *
 * \sa
 */
void Link(int x, int y, unsigned int *labels){
    labels[Find(x, labels)] = Find(y, labels);
}

/**
 * \brief Detect clusters in the image
 *
 * \sa LabelImage
 */
void LabelImage(SDL_Surface *image, unsigned int **label, unsigned int *maxlab){
    int w = image->w, h = image->h;
    unsigned int *labels = malloc(w*h*sizeof(unsigned int));
    for (size_t i = 0; i < ((unsigned int)w*h); i++)
        labels[i] = (unsigned int)i;
    unsigned int highestLabel = 0;

    for (size_t x = 0; x < (size_t)w; x++){
        for (size_t y = 0; y < (size_t)h; y++){
            if(GetValueOfPixel(x,y,image)){
                unsigned int left = label[x == 0 ? 0 : x-1][y];
                unsigned int above = label[x][y == 0 ? 0 : y-1];
                if(x > 0){
                    if(!above && y > 0){
                        above = label[x-1][y-1];
                    }
                    if(!left && y < ((size_t)h-1)){
                        left = label[x-1][y+1];
                    }
                }
                if(!left && !above){
                    highestLabel++;
                    label[x][y] = highestLabel;
                } else if(left && !above) {
                    label[x][y] = Find(left,labels);
                } else if(!left && above) {
                    label[x][y] = Find(above, labels);
                } else {
                    Link(left,above,labels);
                    label[x][y] = Find(left, labels);
                }
            }
        }
    }
    for(size_t x = 0; x < (size_t)w; x++){
        for(size_t y = 0; y < (size_t)h; y++){
            if(GetValueOfPixel(x,y,image)){
                label[x][y] = Find(label[x][y],labels);
            }
        }
    }
    *maxlab = highestLabel;
    free(labels);
}

/**
 * \brief Find largest cluster in image
 *
 * \sa FindLargestCluster
 */
unsigned int FindLargestCluster(unsigned int **labels, int lines, int cols,
    unsigned int nbClusters){

    int *SizeOfCluster = calloc(nbClusters,sizeof(int));
    for(int i = 0; i < cols; i++){
        for(int j = 0; j < lines; j++){
            if(labels[i][j] != 0){
                SizeOfCluster[labels[i][j]-1]++;
            }
        }
    }
    int max = 0, label = 0;
    for(size_t i = 0; i < nbClusters-1; i++){
        if(SizeOfCluster[i] > max){
            max = SizeOfCluster[i];
            label = i+1;
        }
    }
    free(SizeOfCluster);
    return label;
}

/**
 * \brief Remove all information that is not the grid in image
 *
 * \sa IsolateGrid
 */
SDL_Surface *IsolateGrid(unsigned int biggest, unsigned int **labels,
    int w, int h){
    Uint32 Pixel;
    SDL_Surface *surface = SDL_CreateRGBSurface(0,w,h,32,0,0,0,0);

    for(size_t i = 0; i < (size_t)w; i++){
        for(size_t j = 0; j < (size_t)h; j++){
            if (labels[i][j] != biggest)
                Pixel = SDL_MapRGB(surface->format,0,0,0);
            else
                Pixel = SDL_MapRGB(surface->format,255,255,255);
            SetPixel(surface,i,j,Pixel);
        }
    }
    return surface;
}

//Calculate distance between two parallel lines
int CalcuateDistance(Line start, Line end){
    return ((int)fabs(end.rho - start.rho));
}

//Get intersection point of two lines
Point GetIntersect(Line l1, Line l2){
    Point point;
    point.x = l1.rho;
    point.y = l2.rho;
    return point;
}

/**
 * \brief Create a new square image containing only the grid
 *
 * \sa CreateIsolateGrid
 */
SDL_Surface *CreateIsolatedGrid(Line *borders, SDL_Surface *fullImage){
    Line h[2], v[2];
    int vIndex = 0, hIndex = 0;
    for(size_t i = 0; i < 4; i++){
        if(borders[i].theta > 45){
            h[hIndex] = borders[i];
            hIndex++;
        } else {
            v[vIndex] = borders[i];
            vIndex++;
        }
    }
    int j = CalcuateDistance(v[0], v[1]);
    int k = CalcuateDistance(h[0], h[1]);
    int size = j>k ? j : k;

    SDL_Surface *img = SDL_CreateRGBSurface(0,size,size,32,0,0,0,0);
    SDL_Rect cut;
    Point intersect = GetIntersect(v[0],h[0]);
    cut.x = intersect.x;
    cut.y = intersect.y;
    cut.w = CalcuateDistance(v[0],v[1]);
    cut.h = CalcuateDistance(h[0], h[1]);
    SDL_BlitSurface(fullImage,&cut,img,NULL);
    return img;
}

SDL_Surface *GridDetect(SDL_Surface *image){
    int h = image->h;
    int w = image->w;
    unsigned int nbLabels;

    //Initialize labelled image
    unsigned int **label = malloc(image->w*sizeof(unsigned int *));
    for (size_t i = 0; i < (size_t)image->w; i++)
        label[i] = malloc(image->h*sizeof(unsigned int));

    for (size_t i = 0; i < (size_t)image->w; i++)
        for (size_t j = 0; j < (size_t)image->h; j++)
            label[i][j] = 0;

    LabelImage(image, label, &nbLabels);

    unsigned int biggestCluster = FindLargestCluster(label,h,w,nbLabels);

    SDL_Surface *isolated = IsolateGrid(biggestCluster,label,w,h);

    //Free allocated memory
    for (size_t i = 0; i < (size_t)image->w; i++)
        free(label[i]);
    free(label);

    return isolated;
}