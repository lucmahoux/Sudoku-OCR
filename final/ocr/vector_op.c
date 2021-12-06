#include <stdio.h>
#include <stdlib.h>

void transpose(float *m, size_t rows, size_t cols, float *r)
{
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            r[j * rows + i] = m[i * cols + j];
}

void add_mat(float *m1, float *m2, size_t rows, size_t cols, float *r)
{
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            r[i * cols + j] = m1[i * cols + j] + m2[i * cols + j];
}

void sub_mat(float *m1, float *m2, size_t rows, size_t cols, float *r)
{
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            r[i * cols + j] = m1[i * cols + j] - m2[i * cols + j];
}

void mul_mat(float *m1, float *m2, size_t r1, size_t c1, size_t c2, float *r)
{
	for(size_t i = 0; i < r1 * c2; i++)
		r[i] = 0;
    for (size_t i = 0; i < r1; i++)
        for (size_t j = 0; j < c2; j++)
            for (size_t k = 0; k < c1; k++) {
                r[i * c2 + j] += m1[i * c1 + k] * m2[k * c2 + j];
            }
}


void mul_mat_add(float *m1, float *m2, size_t r1, size_t c1, size_t c2, float *r)
{
    for (size_t i = 0; i < r1; i++)
        for (size_t j = 0; j < c2; j++)
            for (size_t k = 0; k < c1; k++)
                r[i * c2 + j] += m1[i * c1 + k] * m2[k * c2 + j];
}

void hadamard_prod(float *m1, float *m2, size_t size, float *r)
{
    for (size_t i = 0; i < size; i++)
        r[i] = m1[i] * m2[i];
}

void mult_const(float *m, float k, size_t size, float *r)
{
    for (size_t i = 0; i < size; i++)
        r[i] = k * m[i];
}

void apply_func(float *list, size_t size, float (*func)(float), float *r)
{
    for (size_t i = 0; i < size; i++)
        r[i] = (*func)(list[i]);
}
