#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "network.h"

// Creates a random number between -0.5 and 0.5
// Not the best function but it is fine for the moment
// See Gaussian distribution

float randomf()
{
	float i = rand() / (float)RAND_MAX;
	i -= 0.5f;
	return i;
}

void shuffle(size_t size_input, size_t *input_index)
{
    for (size_t clean = 0; clean < size_input; clean++)
        input_index[clean] = 0;

    for (size_t num = 1; num < size_input; num++)
    {
        size_t index;
        do
        {
            index = rand() % size_input;

        } while (input_index[index] != 0);

        input_index[index] = num;
    }
}

void reinitialize_layer(Layer layer)
{
	for(size_t i = 0; i < layer.nbOut; i++)
	{
		layer.error[i] = 0;
		layer.output[i] = 0;
		layer.z[i] = 0;
		layer.zSigmo[i] = 0;
	}
}

/* Set to 0 every value of the layer 
 * usefull at the end of a mini-batch */

void reinitialize_layer_after_batch(Layer layer)
{
	for(size_t i = 0; i < layer.nbOut; i++)
	{
		layer.error[i] = 0;
		layer.output[i] = 0;
		layer.z[i] = 0;
		layer.zSigmo[i] = 0;
		layer.deltaB[i] = 0;
		
		for(size_t j = 0; j < layer.nbIn; j++)
			layer.deltaW[i * layer.nbOut + j] = 0;
	}
}

/* Free memory for layer struct */

void free_layer(Layer layer)
{
 	free(layer.weights);
	free(layer.bias);
	free(layer.output);
	free(layer.error);
	free(layer.z);
	free(layer.deltaB);
	free(layer.deltaW);
	free(layer.weightT);
	free(layer.zSigmo);
}

/* Free memory for neural struct */

void free_neural(Neural neural)
{
	for(size_t i = 0; i < neural.nbLayer; i++)
		free_layer(neural.layers[i]);
	free(neural.layers);
}

float* initialize_weights(size_t nbIn, size_t nbOut)
{
	size_t size = nbIn * nbOut;
	float* list = calloc(size,sizeof(float));

	for(size_t i = 0; i < size; i++)
		list[i] = randomf() / sqrtf((float) nbIn);

	return list;
}

// Initializes the bias
// Returns a float pointer of size size

float* initialize_bias(size_t size)
{
	float* list = calloc(size,sizeof(float));

	for (size_t i = 0; i < size; i++)
		list[i] = randomf();

	return list;
}

// Initializes a layer
// Returns a layer with every attributes initialized

Layer initialize_layer(size_t nbIn, size_t nbOut)
{
	Layer layer = {nbIn, nbOut, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	layer.weights = initialize_weights(nbIn, nbOut);
	layer.bias = initialize_bias(nbOut);
	layer.output = (float*) calloc(nbOut,sizeof(float));
	layer.error = (float*) calloc(nbOut,sizeof(float));
	layer.z = (float*) calloc(nbOut,sizeof(float));
	layer.zSigmo = (float*) calloc(nbOut,sizeof(float));
    layer.weightT = initialize_weights(nbIn, nbOut);
	layer.deltaB = (float*) calloc(nbOut,sizeof(float));
	layer.deltaW = (float*) calloc(nbIn*nbOut,sizeof(float));
	return layer;
}

// Initializes a neural network
// Returns a neural network with its attributes initialized

Neural initialize_network(size_t* input, size_t nbLayer)
{
	Neural network = {nbLayer, NULL};
	network.layers = (Layer *) calloc(nbLayer, sizeof(Layer));
	for(size_t i = 0; i < nbLayer; i++)
	{
		network.layers[i] = initialize_layer(input[i], input[i+1]);
	}
	return network;	
}
