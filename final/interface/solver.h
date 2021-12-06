#ifndef SOLVER_H
#define SOLVER_H

int* LoadGrid(char* path, int* grid);
int solver(int* grid);
void SaveGrid(int* grid, char* filename);

#endif