#include "saveGrid.h"
#include "solver.h"
#include "../utils.h"
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

/**
 * \brief Fill grid with cell at postion
 * \param grid Grid to fill
 * \param cell Cell to copy
 * \param position Position of the cell in the grid (Between 0 and 80)
 * \sa FillGrid
 */
void FillGrid(SDL_Surface* grid, SDL_Surface* cell, int pos){
    int x = pos % 9;
    int y = pos / 9;
    x = x * 22 + 2 * (x / 3) + x + 3;
    y = y * 22 + 2 * (y / 3) + y + 3;
    int cellSize = cell->w;
    for(int i = 0; i < cellSize; i++){
        for(int j = 0; j < cellSize; j++){
            SetPixel(grid, x + i, y + j, GetPixel(cell, i , j));
        }
    }
}

/**
 * \brief Create and save the solved grid
 * \param old Grid cells values before solving
 * \param solved Grid cells values after solving
 * \sa PrettyGrid
 */
void PrettyGrid(int* old, int* solved){
    SDL_Surface* grid = IMG_Load("data/imgNeeded/EmptyGrid.png");
    SDL_Surface** red = malloc(9 * sizeof(grid));
    SDL_Surface** black = malloc(9 * sizeof(grid));
    char redstr[] = "data/imgNeeded/0r.png";
    char blackstr[] = "data/imgNeeded/0b.png";
    for(int i = 0; i < 9; i++){
        redstr[15] = i + '1';
        blackstr[15] = i + '1';
        red[i] = IMG_Load(redstr);
        black[i] = IMG_Load(blackstr);
    }
    for(int i = 0; i < 81; i++){
        FillGrid(grid, solved[i] == old[i] ?
        black[solved[i] - 1] : red[solved[i] - 1], i);
    }
    for(int i = 0; i < 9; i++){
        SDL_FreeSurface(red[i]);
        SDL_FreeSurface(black[i]);
    }
    free(red);
    free(black);
    IMG_SavePNG(grid, "results/step9_Solved.png");
    SDL_FreeSurface(grid);
}

/**
 * \brief Try and solve the grid given
 * \param grid Grid cells values
 * \sa main_SaveGrid
 */
int main_SaveGrid(int* grid){
    int* solved  = (int*) calloc(81, sizeof(int));
    memcpy(solved, grid, 81 * sizeof(int));
    int res = solver(solved);
    if(res){
        SaveGrid(solved, "results/solved");
        PrettyGrid(grid, solved);
    }
    free(solved);
    return res;
}