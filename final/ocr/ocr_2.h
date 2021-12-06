#ifndef OCR_2_H_
#define OCR_2_H_

#include "network.h"
#include "dataset.h"

void train(Neural neural,Dataset *data,Dataset *test, size_t nb_epoch);
void feedforward(Neural neural, float *input);
int query(Neural neural, float *input);

#endif
