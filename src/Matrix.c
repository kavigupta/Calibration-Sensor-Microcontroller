/*
 * Matrix.c
 *
 *  Created on: Jul 2, 2015
 *      Author: root
 */

#include "Matrix.h"

#include <stdio.h>

const Matrix IDENTITY = { .v11 = 1, .v22 = 1, .v33 = 1 };

void matrix_print(Matrix A) {
	printf("%f %f %f\n%f %f %f\n%f %f %f", A.v11, A.v12, A.v13, A.v21, A.v22,
			A.v23, A.v31, A.v32, A.v33);
}
int matrix_equals(Matrix A, Matrix B) {
	if (A.v11 != B.v11)
		return 0;
	if (A.v12 != B.v12)
		return 0;
	if (A.v13 != B.v13)
		return 0;
	if (A.v21 != B.v21)
		return 0;
	if (A.v22 != B.v22)
		return 0;
	if (A.v23 != B.v23)
		return 0;
	if (A.v31 != B.v31)
		return 0;
	if (A.v32 != B.v32)
		return 0;
	if (A.v33 != B.v33)
		return 0;
	return 1;
}

Matrix matrix_add(Matrix A, Matrix B) {
	Matrix mat = {
	//
			.v11 = A.v11 + B.v11, //
			.v21 = A.v21 + B.v21, //
			.v31 = A.v31 + B.v31, //
			.v12 = A.v12 + B.v12, //
			.v22 = A.v22 + B.v22, //
			.v32 = A.v32 + B.v32, //
			.v13 = A.v13 + B.v13, //
			.v23 = A.v23 + B.v23, //
			.v33 = A.v33 + B.v33, //
			};
	return mat;
}
Matrix matrix_multiply(Matrix A, Matrix B) {
	Matrix mat = {
	//
			.v11 = A.v11 * B.v11 + A.v12 * B.v21 + A.v13 * B.v31, //
			.v12 = A.v11 * B.v12 + A.v12 * B.v22 + A.v13 * B.v32, //
			.v13 = A.v11 * B.v13 + A.v12 * B.v23 + A.v13 * B.v33, //
			.v21 = A.v21 * B.v11 + A.v22 * B.v21 + A.v23 * B.v31, //
			.v22 = A.v21 * B.v12 + A.v22 * B.v22 + A.v23 * B.v32, //
			.v23 = A.v21 * B.v13 + A.v22 * B.v23 + A.v23 * B.v33, //
			.v31 = A.v31 * B.v11 + A.v32 * B.v21 + A.v33 * B.v31, //
			.v32 = A.v31 * B.v12 + A.v32 * B.v22 + A.v33 * B.v32, //
			.v33 = A.v31 * B.v13 + A.v32 * B.v23 + A.v33 * B.v33, //
			};
	return mat;
}
Matrix matrix_scale(Matrix A, double scalar) {
	Matrix mat = {
	//
			.v11 = A.v11 * scalar, //
			.v21 = A.v21 * scalar, //
			.v31 = A.v31 * scalar, //
			.v12 = A.v12 * scalar, //
			.v22 = A.v22 * scalar, //
			.v32 = A.v32 * scalar, //
			.v13 = A.v13 * scalar, //
			.v23 = A.v23 * scalar, //
			.v33 = A.v33 * scalar, //
			};
	return mat;
}
Vector matrix_apply(Matrix mat, Vector v) {
	Vector result = {
	//
			.x = mat.v11 * v.x + mat.v12 * v.y + mat.v13 * v.z, //
			.y = mat.v21 * v.x + mat.v22 * v.y + mat.v23 * v.z, //
			.z = mat.v31 * v.x + mat.v32 * v.y + mat.v33 * v.z, //
			};
	return result;
}

double matrix_determinant(Matrix a) {
	return a.v11 * (a.v22 * a.v33 - a.v32 * a.v23)
			- a.v12 * (a.v21 * a.v33 - a.v31 * a.v23)
			+ a.v13 * (a.v21 * a.v32 - a.v31 * a.v22);
}

Matrix matrix_inverse(Matrix a) {
	Matrix inv = {
	//
			.v11 = a.v22 * a.v33 - a.v23 * a.v32, .v12 = a.v13 * a.v32
					- a.v12 * a.v33, .v13 = a.v12 * a.v23 - a.v22 * a.v13, //
			.v21 = a.v23 * a.v31 - a.v21 * a.v33, .v22 = a.v11 * a.v33
					- a.v31 * a.v13, .v23 = a.v13 * a.v21 - a.v11 * a.v23, //
			.v31 = a.v21 * a.v32 - a.v22 * a.v31, .v32 = a.v12 * a.v31
					- a.v11 * a.v32, .v33 = a.v11 * a.v22 - a.v12 * a.v21 };
	return matrix_scale(inv, 1. / matrix_determinant(a));
}

Matrix matrix_rotation(Vector a, Vector b) {
	Vector unit_a = vector_unit(a), unit_b = vector_unit(b);
	if (vector_equals(unit_a, unit_b))
		return IDENTITY;
	if (vector_equals(unit_a, vector_scale(unit_b, -1)))
		return matrix_scale(IDENTITY, -1);
	Vector v = vector_cross(unit_a, unit_b);
	double sin = vector_mag(v);
	double cos = vector_dot(unit_a, unit_b);
	Matrix skewcross = { .v12 = -v.z, .v13 = v.y, .v21 = v.z, .v23 = -v.x,
			.v31 = -v.y, .v32 = v.x };
	Matrix square = matrix_scale(matrix_multiply(skewcross, skewcross),
			(1 - cos) / (sin * sin));
	return matrix_add(matrix_add(IDENTITY, skewcross), square);
}
