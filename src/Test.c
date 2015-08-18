/*
 * Test.c
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include "Test.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Controller.h"
#include "DataSet.h"
#include "IO.h"
#include "list.h"
#include "Matrix.h"
#include "Vector.h"

void test_matrix_mathematics() {
	Matrix test0 = { .v11 = 1, .v12 = 2, .v13 = 3, .v21 = 4, .v22 = 5, .v23 = 6,
			.v31 = 7, .v32 = 8, .v33 = 9 };
	Matrix test5 = { .v11 = 123.1, .v12 = 2, .v13 = 3, .v21 = 4, .v22 = 5,
			.v23 = 6, .v31 = 7, .v32 = 8, .v33 = 9 };
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
	assert(test_approximate_equality_vect(matrix_apply(test1, test2), test3));
	assert(
			vector_equals(vector_cross(VECTOR_UNIT_X, VECTOR_UNIT_Y),
					VECTOR_UNIT_Z));
	assert(test_approximate_equality_vect(vector_cross(test2, test3), test4));
	assert(
			test_approximate_equality_vect(
					matrix_apply(matrix_rotation(VECTOR_UNIT_X, test2),
							VECTOR_UNIT_X), vector_unit(test2)));
}

void test_calibration_with_45dgup_data() {
	list(JoinedData) calibration =
			io_read_joined_dataset(
					"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/calibration.csv");
	list(JoinedData) dgup45 =
			io_read_joined_dataset(
					"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/45dgup.csv");
	cntrl_calibrate(calibration);
	Vector calib = averageMag(calibration);
	Vector mag45 = averageMag(dgup45);
	vector_print(calib);
	vector_print(mag45);
	printf("\n");
	printf("%f\n",
			acos(vector_dot(vector_unit(calib), vector_unit(mag45))) * 180.
					/ 3.1416);
	Vector displacement = cntrl_get_orientation(cntrl_get_theta(mag45));
	vector_print(displacement);
	printf("\n%f\n", vector_mag(displacement));
}

int test_approximate_equality_double(double a, double b) {
	return abs(log(a / b)) < .001;
}

int test_approximate_equality_vect(Vector a, Vector b) {
	if (!test_approximate_equality_double(a.x, b.x))
		return 0;
	if (!test_approximate_equality_double(a.y, b.y))
		return 0;
	if (!test_approximate_equality_double(a.z, b.z))
		return 0;
	return 1;
}
