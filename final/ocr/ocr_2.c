#include <SDL2/SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "test.h"
#include "network.h"
#include "dataset.h"
#include "save_network.h"
#include "recover_grid.h"
#include "vector_op.h"

#define TRAINING_SIZE 29969
#define BATCH_SIZE 10
#define LAMBDA 20
#define LEARNING_RATE 0.2



/* Converts uint8_t to float */

void uint8_t_to_float(uint8_t *convertee, size_t size, float *converted)
{
	float convert;
	for(size_t i = 0; i < size; i++)
	{
		// PK PAS JUSTE : converted[i] = convertee[i] > 180 ?
        // Parce que j'aime pas les ternaires :)
		if(convertee[i] > 180)
			convert = 1;
		else
			convert = 0;
		converted[i] = convert;
	}
}

/* Create the pointer of input*/
float **create_input()
{
    float **input = malloc(TRAINING_SIZE * sizeof(float*));
    for (size_t i = 0; i < TRAINING_SIZE; i++)
    {
        input[i] = malloc(784*sizeof(float));
    }
    return input;
}

/* Free memory of input */
void free_input(float **input)
{
    for (size_t i =0; i < TRAINING_SIZE; i++)
    {
        free(input[i]);
    }
    free(input);
}

/* Sigmoid function */

float sigmoid(float x)
{
	return 1 / (1 + exp(-x));
}

/* Sigmoid deritative */

float sigmoid_prime(float x)
{
	return (sigmoid(x)) * (1 - sigmoid(x));
}

/* Softmax activation function */

void softmax(Layer layer)
{
	float exp_sum = 0;
	for(size_t i = 0; i < layer.nbOut; i++)
		exp_sum += exp(layer.output[i]);
	for(size_t i = 0; i < layer.nbOut; i++)
		layer.output[i] = exp(layer.output[i]) / exp_sum;
}

/* Feedforward the neural network with the given input */

void feedforward(Neural neural, float *input)
{
    for(size_t i = 0; i < neural.nbLayer; i++)
	{
        mul_mat(neural.layers[i].weights, input, neural.layers[i].nbOut, neural.layers[i].nbIn, 1, neural.layers[i].z);
		add_mat(neural.layers[i].z, neural.layers[i].bias, 1, neural.layers[i].nbOut, neural.layers[i].z);
		float (*func)(float) = sigmoid;
		apply_func(neural.layers[i].z, neural.layers[i].nbOut,(*func), neural.layers[i].output);
		input = neural.layers[i].output;
	}
}



/* Querys the result
 * Prints the result
 * Returns the result */

int query(Neural neural, float *input)
{
	feedforward(neural, input);
	int max = 1;
	for(int i = 2; i < 10; i++)
	{
		if(neural.layers[neural.nbLayer - 1].output[i] > neural.layers[neural.nbLayer - 1].output[max])
			max = i;
	}
	return max;
}

/* Backpropagation function
 * Calculates the output error and propagates it towards input layer */

void backprop(Neural neural, uint8_t target, float* input)
{
	feedforward(neural, input);
	float targetList[10] = {0};
	targetList[target] = 1;
	float (*func)(float) = sigmoid_prime;
	size_t nbOut = 10;

	// Calculate the output error
	sub_mat(neural.layers[neural.nbLayer - 1].output, targetList, nbOut, 1, neural.layers[neural.nbLayer - 1].error);

	// Propagates the error
	//
	// VERIFY IF NOT i < neural.nbLayer + 1 INSTEAD OF i < neural.nbLayer
	// PROBLEME DE TAILLE DE NBLAYER, VERIFIER SI BIEN TOUTES LES ERREURS SONT PROPAGATED
	// NOTAMMENT VERIFIER AVEC NBLAYER = 2

	for(size_t i = 1; i < neural.nbLayer; i++)
	{
		nbOut = neural.layers[neural.nbLayer - i].nbOut;
		size_t nbIn = neural.layers[neural.nbLayer - i].nbIn;

		float *weightT = neural.layers[neural.nbLayer - i].weightT;
		transpose(neural.layers[neural.nbLayer - i].weights, nbIn, nbOut, weightT);

		mul_mat(weightT, neural.layers[neural.nbLayer - i].error, nbIn, nbOut, 1, neural.layers[neural.nbLayer - i - 1].error);

		// VERIFIER L'UTILISATION DU SIGMO PRIME
		float *zSigmo = neural.layers[neural.nbLayer  - 1- i].zSigmo;

		apply_func(neural.layers[neural.nbLayer - i - 1].z, nbIn, (*func), zSigmo);
		hadamard_prod(neural.layers[neural.nbLayer - i - 1].error, zSigmo, nbIn, neural.layers[neural.nbLayer - i - 1].error);

		// Update deltaW and deltaB
		// VERIFIER LES neural.nbLayer - i si on a bien toutes les layers sauf la last

		add_mat(neural.layers[neural.nbLayer -i].deltaB, neural.layers[neural.nbLayer - i].error, nbOut, 1, neural.layers[neural.nbLayer - i].deltaB);

		mul_mat_add(neural.layers[neural.nbLayer - i].error, neural.layers[neural.nbLayer - i - 1].output, nbOut, 1, nbIn, neural.layers[neural.nbLayer - i].deltaW);
	}

	//Update deltaW and deltaB but for the first layer

	nbOut = neural.layers[0].nbOut;
	size_t nbIn = neural.layers[0].nbIn;
	add_mat(neural.layers[0].deltaB, neural.layers[0].error, nbOut, 1, neural.layers[0].deltaB);

	mul_mat_add(neural.layers[0].error, input, nbOut, 1, nbIn, neural.layers[0].deltaW);
}

// Updates the weights and the biases of the network
// Reinitialize the deltaW and deltaB


void update_weights(Neural neural)
{
	float scal = 1 - LEARNING_RATE * (LAMBDA / TRAINING_SIZE);
	float nabla_scal = LEARNING_RATE / BATCH_SIZE;

	for(size_t i = 0; i < neural.nbLayer; i++)
	{
		size_t nbOut = neural.layers[i].nbOut;
		size_t nbIn = neural.layers[i].nbIn;

		mult_const(neural.layers[i].weights, scal, nbOut * nbIn, neural.layers[i].weights);
		mult_const(neural.layers[i].deltaW, nabla_scal, nbOut * nbIn, neural.layers[i].deltaW);
        //modified nbOUt*nbIn in nbOut, les biais y'en a autant que les outputs
		mult_const(neural.layers[i].deltaB, nabla_scal, nbOut , neural.layers[i].deltaB);
		sub_mat(neural.layers[i].weights, neural.layers[i].deltaW, nbIn, nbOut, neural.layers[i].weights);
        // there are as much bias as output !!!!!
		sub_mat(neural.layers[i].bias, neural.layers[i].deltaB, 1, nbOut, neural.layers[i].bias);
		reinitialize_layer_after_batch(neural.layers[i]);
	}
}


/* Create the input*/
void input_training(Dataset *data, float **input, uint8_t *labels )
{
    size_t index[TRAINING_SIZE];
    shuffle(TRAINING_SIZE, index);
    for (size_t i = 0; i <TRAINING_SIZE; i++)
    {
        labels[i] = data->labels[index[i]];
        for (int j =0; j < 784; j++)
        {
//			input[i][j] = data->images[index[i]].pixels[j] < 100 ? 0 : 1;
            input[i][j] = (float) data->images[index[i]].pixels[j] / (float) 255;
        }
    }
}
// NEW NAME FOR EPOCH FUNCTION ???? IS EPOCH() IN OCR.C A REAL EPOCH
// I FEEL THAT IT IS A MINI BATCH INSTEAD :THINKING:

void epoch(Neural neural,Dataset *data, float** input,uint8_t *label)
{
    input_training(data, input, label);
	for(size_t index = 0; index < TRAINING_SIZE - BATCH_SIZE ; index++)
	{
        for (size_t batch = 0; batch < BATCH_SIZE; batch++,index++)
        {
            backprop(neural, label[index], input[index]);
		    for(size_t i = 0; i < neural.nbLayer; i++)
			    reinitialize_layer(neural.layers[i]);

        }
	    update_weights(neural);
	}
}

void train(Neural neural,Dataset *data,Dataset *test, size_t nb_epoch)
{
    uint8_t *labels = calloc(TRAINING_SIZE,sizeof(uint8_t));
    float **input = create_input();
    for (size_t epoch_index = 0; epoch_index < nb_epoch; epoch_index++)
    {
        printf("Epoch n° %lu of %lu pending\n",epoch_index +1, nb_epoch );
        epoch (neural, data, input, labels);
        ask_training(neural,test);
		printf("Epoch n° %lu of %lu done\n",epoch_index +1, nb_epoch );

    }
    free_input(input);
    free(labels);
}
