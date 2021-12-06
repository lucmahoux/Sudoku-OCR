#include "solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Test if a value is present in a column of the grid */
/* Return 1 if it is present */
int IsAbsentColumn(int* grid, int value, int column) {
	for (size_t index = 0 ; index < 9; index++)
		if (grid[index*9 + column] == value) return 0;
	return 1;
}

int IsMultipleColumn(int* grid, int column, int pos) {
	for (int index = 0 ; index < 9; index++)
		if(index*9 + column != pos)
			if (grid[index*9 + column] == grid[pos]) return 1;
	return 0;
}

/* Test if a value is present in a row of the sudoku grid */
/* Return 1 if it is */
int IsAbsentRow(int* grid, int value, int row) {
	for (size_t index = 0 ; index < 9; index++)
		if (grid[row*9 + index] == value) return 0;
	return 1;
}

int IsMultipleRow(int* grid, int row, int pos){
	for (int index = 0 ; index < 9; index++)
		if (row*9 + index != pos)
			if (grid[row*9 + index] == grid[pos]) return 1;
	return 0;
}

/* Test if a value is present in a square */
/* Return 1 if it is present */
int IsAbsentSquare(int* grid, int value, int row, int column) {
	row = row - (row%3);
	column = column - (column%3);
	for (size_t index1 = 0; index1 <3;index1++) {
		for (size_t index2 = 0; index2 < 3; index2++)
			if (grid[(row + index1)*9  + column + index2] == value) return 0;
	}
	return 1;
}

int IsMultipleSquare(int* grid, int row, int column, int pos){
	row = row - (row%3);
	column = column - (column%3);
	for (int index1 = 0; index1 <3;index1++) {
		for (int index2 = 0; index2 < 3; index2++) {
			if ((row + index1)*9  + column + index2 != pos)
				if (grid[(row + index1)*9  + column + index2] == grid[pos]) return 1;
		}
	}
	return 0;
}

/* Backtracking algorithm */
/* takes the grid and a position  */
/* return 1 if the grid is solved */
int Backtrack(int* grid, int pos) {
	if (pos == 81) return 1;
	if (grid[pos] !=0 ) return Backtrack(grid, pos +1);

	for (size_t value = 1; value <= 9 ; value++) {
		if (IsAbsentColumn(grid, value, pos%9) &&
		IsAbsentRow(grid,value,pos/9) &&
		IsAbsentSquare(grid,value,pos/9,pos%9)) {
			grid[pos] = value;
			if (Backtrack(grid,pos +1))
				return 1;
		}
	}
	grid[pos] = 0;
	return 0;
}

int* LoadGrid(char* path, int* grid) {
    FILE* f = fopen(path, "rt");
	char* line = (char *) calloc(12, sizeof(char));
	size_t index = 0;
	size_t r = 0;
	while (fscanf(f,"%[^\n]",line) != EOF) {
		if (r != 3 && r != 6) {
			for (size_t i = 0; i < 11; i++) {
				if (line[i] != '.' && line[i] != ' ') {
					grid[index] = (int) line[i] - '0';
					index++;
				} else if (line[i] == '.') {
					index++;
				}
			}
		}
		fgetc(f);
		r++;
    }
	free(line);
    return grid;
}

/* Main function of solver */
int solver(int* grid) {
	for (int i = 0; i < 81; i++){
		if(grid[i] != 0){
			if (IsMultipleColumn(grid, i%9, i) ||
			IsMultipleRow(grid, i/9, i) ||
			IsMultipleSquare(grid, i/9, i%9, i)) {
				return 0;
			}
		}
	}
	return Backtrack(grid,0);;
}

/*Save grid in a new file named filename.result*/
/*Open new file in 'w' mode and fprintf in the file the result of the grid*/
void SaveGrid(int* grid, char* filename) {
	char* result = malloc(strlen(filename) + sizeof(".result"));
	strcpy(result, filename);
	strcat(result, ".result");
	FILE* f = fopen(result, "w");
	for (size_t l = 0; l < 81; l++) {
		if (l %3 == 0 && l%9 != 0) fprintf(f, " ");
		if (l != 0 && l % 9 == 0) fprintf(f, "\n");
		if (l != 0 && l % 27 == 0) fprintf(f, "\n");
		fprintf(f, "%i",grid[l]);
	}
	fprintf(f, "\n");
	fclose(f);
	free(result);
}
