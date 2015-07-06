/*
 * Test.c
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "Controller.h"
#include "IO.h"
#include "Matrix.h"
#include "Vector.h"

void printEquality(Vector a, Vector b) {
	vector_print(a);
	printf("=");
	vector_print(b);
	printf("\n");
}

void testMatrix() {
	Matrix test0 = { .v11 = 1, .v12 = 2, .v13 = 3, .v21 = 4, .v22 = 5, .v23 = 6,
			.v31 = 7, .v32 = 8, .v33 = 9 };
	Matrix test5 = { .v11 = 123.1, .v12 = 2, .v13 = 3, .v21 = 4, .v22 = 5, .v23 = 6,
			.v31 = 7, .v32 = 8, .v33 = 9 };
	Matrix test1 = { .v11 = 30, .v12 = 36, .v13 = 42, .v21 = 66, .v22 = 81,
			.v23 = 96, .v31 = 102, .v32 = 126, .v33 = 150 };
	Vector test2 = { .x = 23.5, .y = -23.4, .z = 2.3 };
	Vector test3 = { -40.8, -123.6, -206.4 };
	Vector test4 = { 5114.04, 4756.56, -3859.32 };
	assert(matrix_equals(IDENTITY, matrix_multiply(IDENTITY, IDENTITY)));
	assert(matrix_equals(test0, matrix_multiply(test0, IDENTITY)));
	assert(matrix_equals(test1, matrix_multiply(test0, test0)));
	assert(vector_equals(test2, matrix_apply(IDENTITY, test2)));
	matrix_print(matrix_inverse(test5));
	assert(matrix_equals(matrix_inverse(IDENTITY), IDENTITY));
	printEquality(matrix_apply(test1, test2), test3);
	assert(
			vector_equals(vector_cross(VECTOR_UNIT_X, VECTOR_UNIT_Y),
					VECTOR_UNIT_Z));
	printEquality(vector_cross(test2, test3), test4);
	printEquality(
			matrix_apply(matrix_rotation(VECTOR_UNIT_X, test2), VECTOR_UNIT_X),
			vector_unit(test2));
	printf("Tests Passed!\n");
}

int main() {
	testMatrix();
	DataSet calibration =
			read(
					"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/calibration.csv");
	DataSet dgup45 =
			read(
					"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/45dgup.csv");
	calibrate(calibration);
	Vector calib = averageMag(calibration);
	Vector mag45 = averageMag(dgup45);
	vector_print(calib);
	vector_print(mag45);
	printf("\n");
	printf("%f\n",
			acos(vector_dot(vector_unit(calib), vector_unit(mag45))) * 180.
					/ 3.1416);
	Vector displacement = orientation(Theta(mag45));
	vector_print(displacement);
	printf("\n%f\n", vector_mag(displacement));
	return 0;
}
