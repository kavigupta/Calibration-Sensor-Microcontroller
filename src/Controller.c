/*
 * Magnetometer.c
 *
 *  Created on: Jul 2, 2015
 *      Author: root
 */

#include "Controller.h"
#include "Vector.h"
#include <string.h>

void calibrate(DataSet data) {
	int i;
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	for (i = 0; i < data.len - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].acl.x * dt;
		y += data.values[i].acl.y * dt;
		z += data.values[i].acl.z * dt;
		tsum += dt;
	}
	Vector gbar = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	memcpy(&G_CALIBRATION, &gbar, sizeof(Vector));
	x = 0;
	y = 0;
	z = 0;
	for (i = 0; i < data.len - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].mag.x * dt;
		y += data.values[i].mag.y * dt;
		z += data.values[i].mag.z * dt;
	}
	Vector mag_bar = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	Matrix Theta = matrix_inverse(matrix_rotation(VECTOR_UNIT_X, mag_bar));
	memcpy(&THETA_CALIBRATION, &Theta, sizeof(Matrix));

}
Matrix Theta(Vector mag) {
	return matrix_multiply(matrix_rotation(VECTOR_UNIT_X, mag), THETA_CALIBRATION);
}
Vector orientation(Matrix Theta) {
	return matrix_apply(Theta, VECTOR_UNIT_X);
}
Vector normalized_accel(Matrix Theta, Vector unnormalized_accel) {
	return vector_add(unnormalized_accel,
			vector_scale(matrix_apply(Theta, G_CALIBRATION), -1));
}
