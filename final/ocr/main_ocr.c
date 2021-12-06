#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "ocr_2.h"
#include "recover_grid.h"
#include "save_network.h"
#include "network.h"
#include "dataset.h"
#include "../interface/saveGrid.h"
#include "../interface/solver.h"

#define IMAGETEST  "../data/t10k-images-idx3-ubyte"
#define LABELTEST  "../data/t10k-labels-idx1-ubyte"
#define IMAGETRAIN "../data/train-images-idx3-ubyte"
#define LABELTRAIN "../data/train-labels-idx1-ubyte"

int main_Ocr(SDL_Surface **gridCase, Neural neural)
{
	int *recoveredGrid = recover_grid(gridCase, neural);

	int res = main_SaveGrid(recoveredGrid);

	free(recoveredGrid);

	return res;
}
