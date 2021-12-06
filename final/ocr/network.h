#ifndef NETWORK_H_
#define NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Layer*/
/* A layer is defined by the number of input and output nodes,  */
/* by the  weight between the input and output  and by the list of the bias*/


typedef struct Layer
{
	size_t nbIn;
	size_t nbOut;
	float* weights;
	float* bias;
	float* output;
	float* error;
	float* z;
    float* zSigmo;
    float* weightT;
	float* deltaB;
	float* deltaW;
}Layer;

/* Neural network */
/* Defined by both its layer and the number of them */
/* Should the learning rate be implemented here ? */


typedef struct Neural
{
	size_t nbLayer;
	Layer* layers;
}Neural;



void reinitialize_layer(Layer layer);
void reinitialize_layer_after_batch(Layer layer);
void free_neural(Neural neural);
Neural initialize_network(size_t* input, size_t nbLayer);
float randomf();
void shuffle(size_t size_input, size_t *input_index);
#endif
