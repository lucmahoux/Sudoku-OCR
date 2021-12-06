#ifndef VECTOR_OP_H
#define VECTOR_OP_H

#include <stdio.h>

void transpose(float* m, size_t rows, size_t cols, float* r);
void add_mat(float* m1, float* m2, size_t rows, size_t cols, float* r);
void mul_mat(float* m1, float* m2, size_t r1, size_t c1, size_t c2, float* r);
void sub_mat(float* m1, float* m2, size_t rows, size_t cols, float* r);
void hadamard_prod(float* m1, float* m2, size_t size, float* r);
void mul_mat_add(float* m1, float* m2, size_t r1, size_t c1, size_t c2, float* r);
void mult_const(float* m, float k, size_t size, float* r);
void apply_func(float* list, size_t size, float(*func)(float), float* r);

#endif
