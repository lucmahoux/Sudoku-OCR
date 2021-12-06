#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "save_network.h"
#include "dataset.h"
#include "vector_op.h"
#include "network.h"
#include "ocr_2.h"

#define IMAGETEST  "../data/t10k-images-idx3-ubyte"
#define LABELTEST  "../data/t10k-labels-idx1-ubyte"
#define IMAGETRAIN "../data/train-images-idx3-ubyte"
#define LABELTRAIN "../data/train-labels-idx1-ubyte"


void create_input_test(float *input, Image *image)
{
    for (size_t index = 0; index < 784; index++)
    {
        input[index] = (float) image->pixels[index];
    }
}

int ask(Image *image, uint32_t label, Neural neural)
{
    float input[784] = {0};
    create_input_test(input,image);
    feedforward(neural, input);
    uint32_t max = 0;
    for (uint32_t i = 1; i < 10; i++)
    {
        if (neural.layers[1].output[i] > neural.layers[1].output[max])
            max = i;
    }
    return label == max ? 1 : 0;

}

void ask_training(Neural network, Dataset *data)
{
    int num = 0;
    for (uint32_t i = 0; i < data->size; i++)
    {
        num += ask(&data->images[i], data->labels[i], network);
    }
    float percent = num / (float) data->size * 100;
    printf("==================================================\n");
    printf("The accuracy is %f percent \n",percent);
    printf("Found %i out of %i test\n",num, data->size);
    printf("==================================================\n");
}

void test()
{
    size_t nbNeurons[3] = {784,128,10};
    // Create Network
    Neural network = initialize_network(nbNeurons, 2);
    //Dataset
    Dataset *data = dataset_load_csv("data/TMNIST_Data.csv",29969,785);


    train(network,data,data, 5);

    save_setup(network, "data/neural_setup_6");

    free_neural(network);

    free_dataset(data);

}


