/*
 * Magnetometer.c
 *
 *  Created on: Jul 2, 2015
 *      Author: root
 */

#include "Controller.h"
#include "Vector.h"
#include <string.h>

void cntrl_calibrate(JoinedDataList data) {
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
	printf("CALIB::: %f, %f, %f, %f\n", x, y, z, tsum);
	Vector gbar = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	G_CALIBRATION = gbar;
	vector_print(G_CALIBRATION);
	printf("\n");
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
Matrix cntrl_get_theta(Vector mag) {
	return matrix_multiply(matrix_rotation(VECTOR_UNIT_X, mag), THETA_CALIBRATION);
}
Vector cntrl_get_orientation(Matrix Theta) {
	return matrix_apply(Theta, VECTOR_UNIT_X);
}
Vector cntrl_get_adjusted_accel(Matrix Theta, Vector unnormalized_accel) {
	vector_print(G_CALIBRATION);
	printf("<-- g\n");
	vector_print(unnormalized_accel);
	printf("<-- before\n");
	vector_print(matrix_apply(Theta, G_CALIBRATION));
	printf("<-- subtract\n");
	vector_print(vector_add(unnormalized_accel,
			vector_scale(matrix_apply(Theta, G_CALIBRATION), -1)));
	printf("<-- result\n");
	return vector_add(unnormalized_accel,
			vector_scale(matrix_apply(Theta, G_CALIBRATION), -1));
}
