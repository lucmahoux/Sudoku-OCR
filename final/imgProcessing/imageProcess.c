#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "adaptativeThreshold.h"
#include "cutting.h"
#include "detect.h"
#include "greyscale.h"
#include "gridDetect.h"
#include "rotate_image.h"
#include "../utils.h"
#include "../ocr/main_ocr.h"
#include "../ocr/network.h"
#include "../ocr/dataset.h"
#include "../ocr/ocr_2.h"
#include "../ocr/save_network.h"

#define IMAGETEST  "../data/t10k-images-idx3-ubyte"
#define LABELTEST  "../data/t10k-labels-idx1-ubyte"
#define IMAGETRAIN "../data/train-images-idx3-ubyte"
#define LABELTRAIN "../data/train-labels-idx1-ubyte"


/**
 * \brief Process the image to try and solve it
 * \param path Absolute path of the image given
 * \sa main_ImageProcess
 */
int main_ImageProcess(char *path){
    SDL_Surface* image = IMG_Load(path);
    if(path && image){
        /*Binarize Image*/
        *image = ToGreyscale(image);
        IMG_SavePNG(image, "results/step1_Greyscale.png");

        image = BlackAndWhite(image);
        IMG_SavePNG(image, "results/step2_BlacknWhite.png");

        InvertColors(image);
        IMG_SavePNG(image, "results/step3_Inverted.png");

        /*Detect Grid and rotate it if needed*/
        SDL_Surface *isolated = GridDetect(image);
        IMG_SavePNG(isolated, "results/step4_Isolated.png");

        int __angle = 0;
        image = AutoRotate(image,isolated,&__angle);
        IMG_SavePNG(image, "results/step5_AutoRotate.png");

        /*Isolate and Contain the grid*/
        SDL_Surface *isolatedRotated = Rotate_Image(isolated,__angle);
        SDL_FreeSurface(isolated);
        IMG_SavePNG(image, "results/step6_IsoRot.png");

        SDL_Rect cutRect;
        int size;
        image = CutImage(image,isolatedRotated, &cutRect, &size);
        isolatedRotated = CutImageNoInvert(&cutRect, isolatedRotated, &size);
        image = __RemoveBorder(image,&size,&cutRect);
        isolatedRotated = CutImageNoInvert(&cutRect,isolatedRotated,&size);
        IMG_SavePNG(image, "results/step7_WithoutBorder.png");

        RemoveGrid(isolatedRotated,image);
        SDL_FreeSurface(isolatedRotated);

		/*NEURAL PART*/

		Neural network = read_setup("data/neural_setup_6");

        int solved = 0;
        int cpt = 0;
        /*While grid is not solved, try next rotation of 90°*/
        while (!solved && cpt < 4)
        {
            /*Save grid for visual*/
            switch (cpt)
            {
            case 0:
                IMG_SavePNG(image, "results/step8_WithoutGrid0.png");
                break;
            case 1:
                IMG_SavePNG(image, "results/step8_WithoutGrid1.png");
                break;
            case 2:
                IMG_SavePNG(image, "results/step8_WithoutGrid2.png");
                break;
            case 3:
                IMG_SavePNG(image, "results/step8_WithoutGrid3.png");
                break;
            }

            /*Cut Grid*/
            SDL_Surface** gridCase = GridCut(image);

            /*Center gridCases and free Gridcase*/
            SDL_Surface** centeredCase = malloc(81 * sizeof(SDL_Surface*));
            for (size_t i = 0; i < 81; i++) {
                centeredCase[i] = CenterDigit(Resize(gridCase[i]));
                SDL_FreeSurface(gridCase[i]);
            }
            free(gridCase);

            /*Call main_Ocr*/
            solved = main_Ocr(centeredCase, network);

            /*Free Centered Grid*/
            for(size_t i = 0; i < 81; i++)
            {
                SDL_FreeSurface(centeredCase[i]);
            }
            free(centeredCase);

            /*If grid not solved, try next rotation*/
            if(!solved){
                image = Rotate_Image(image, 90);
                cpt++;
            }
        }

        free_neural(network);
        SDL_FreeSurface(image);
        return solved;
    }
    return 0;
}
