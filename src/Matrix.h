/*
 * Matrix3d.h
 *
 *  Created on: Jul 2, 2015
 *      Author: root
 */

#ifndef MATRIX_H_
#define MATRIX_H_
#define MATRIX_ELEMENT_COUNT 9

#include "Vector.h"
#include "DataSet.h"

typedef struct {
	const double v11, v21, v31, v12, v22, v32, v13, v23, v33;
} Matrix;

extern const Matrix IDENTITY;

void matrix_print(Matrix A);
int matrix_equals(Matrix A, Matrix B);
Matrix matrix_add(Matrix A, Matrix B);
Matrix matrix_multiply(Matrix A, Matrix B);
Matrix matrix_scale(Matrix A, double scalar);
Vector matrix_apply(Matrix mat, Vector v);

double matrix_determinant(Matrix a);
Matrix matrix_inverse(Matrix a);

Matrix matrix_rotation(Vector a, Vector b);

#endif /* MATRIX_H_ */
