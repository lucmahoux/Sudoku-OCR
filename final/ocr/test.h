#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "network.h"
#include "ocr_2.h"
#include "dataset.h"
#include "vector_op.h"

/** test function
 * calculates the accuracy of the neural network
 */

void test();
void ask_training(Neural network, Dataset *data);
