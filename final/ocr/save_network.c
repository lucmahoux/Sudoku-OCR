#include <string.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include "network.h"

void save_setup(Neural neural, char filename[])
{
    FILE *fp;
    fp = fopen(filename, "w+");

	// First line : print nbLayer

    fprintf(fp, "%zu\n", neural.nbLayer);

	fprintf(fp, "%zu\n", neural.layers[0].nbIn);

	for(size_t i = 0; i < neural.nbLayer; i++)
		fprintf(fp, "%zu\n", neural.layers[i].nbOut);

	for(size_t i = 0; i < neural.nbLayer; i++)
	{
		for(size_t j = 0; j < neural.layers[i].nbIn * neural.layers[i].nbOut; j++)
			fprintf(fp, "%f\n", neural.layers[i].weights[j]);

		for(size_t j = 0; j < neural.layers[i].nbOut; j++)
			fprintf(fp, "%f\n", neural.layers[i].bias[j]);
	}

    fclose(fp);
}

Neural read_setup(char filename[])
{
    FILE *fp;
	Neural neural;
    fp = fopen(filename, "r");
    if (fp == NULL)
        errx(1, "Read_setup : the file does not exist !");
	else
    {
		// Gets the number of layer

		size_t nbLayer;
		char line[20];
		if(fgets(line, sizeof(line), fp) == NULL)
			errx(1, "ERRUDSQ");
		sscanf(line, "%zu", &nbLayer);

		// Gets the number of neurons per layer

		size_t *input = calloc(nbLayer + 1,sizeof(size_t));

		for(size_t i = 0; i < nbLayer + 1; i++)
		{
			size_t tmp;
			if(fgets(line,sizeof(line), fp) == NULL)
				errx(1, "ERROR");
			sscanf(line, "%zu", &tmp);
		   	input[i] = tmp;
		}

		// Initialization of the network

		neural = initialize_network(input, nbLayer);

		// Initialize the weights and bias

		for(size_t i = 0; i < neural.nbLayer; i++)
		{
			for(size_t j = 0; j < neural.layers[i].nbOut * neural.layers[i].nbIn; j++)
			{
				float tmp;
				if(fgets(line,sizeof(line), fp) == NULL)
					errx(1, "ERROXR");
				sscanf(line, "%f", &tmp);
				neural.layers[i].weights[j] = tmp;
			}

			for(size_t j = 0; j < neural.layers[i].nbOut; j++)
			{
				float tmp;
				if(fgets(line,sizeof(line), fp) == NULL)
					errx(1, "ERRD");
				sscanf(line, "%f", &tmp);
				neural.layers[i].bias[j] = tmp;
			}
		}
		free(input);
	}
    fclose(fp);
	return neural;
}
