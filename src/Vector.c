/*
 * Accel.c
 *
 *  Created on: Jun 25, 2015
 *      Author: root
 */

#include "Vector.h"

#include <math.h>
#include <stdio.h>

const Vector VECTOR_UNIT_X = { .x = 1 };
const Vector VECTOR_UNIT_Y = { .y = 1 };
const Vector VECTOR_UNIT_Z = { .z = 1 };

int vector_equals(Vector a, Vector b) {
	if (a.x != b.x)
		return 0;
	if (a.y != b.y)
		return 0;
	return a.z == b.z;
}

Vector vector_add(Vector u, Vector v) {
	Vector sum = { u.x + v.x, u.y + v.y, u.z + v.z };
	return sum;
}

Vector vector_scale(Vector v, double k) {
	Vector prod = { k * v.x, k * v.y, k * v.z };
	return prod;

}

Vector vector_sum(Vector* data, int len) {
	double sumx, sumy, sumz;
	sumx = sumy = sumz = 0;
	int i;
	for (i = 0; i < len; i++) {
		sumx += data[i].x;
		sumy += data[i].y;
		sumz += data[i].z;
	}
	Vector sum = { sumx, sumy, sumz };
	return sum;
}

Vector vector_average(Vector* data, int len) {
	Vector sum = vector_sum(data, len);
	return vector_scale(sum, 1.0 / (double) len);
}

double vector_squareMag(Vector v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

double vector_mag(Vector v) {
	return sqrt(vector_squareMag(v));
}

Vector vector_unit(Vector v) {
	return vector_scale(v, 1. / vector_mag(v));
}

double vector_dot(Vector u, Vector v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vector vector_cross(Vector u, Vector v) {
	Vector cross = { .x = u.y * v.z - u.z * v.y, .y = u.z * v.x - u.x * v.z,
			.z = u.x * v.y - u.y * v.x };
	return cross;
}

void vector_print(Vector v) {
	printf("(%f, %f, %f)", v.x, v.y, v.z);
}

PolarVector vector_toPolar(Vector v) {
	PolarVector pv = { .r = vector_mag(v), .theta = atan2(
			sqrt(v.x * v.x + v.y * v.y), v.z), .phi = atan2(v.y, v.x) };
	return pv;
}
Vector vector_toCartesian(PolarVector v) {
	double sintheta = sin(v.theta);
	Vector cv = { .x = v.r * sintheta * cos(v.phi), .y = v.r * sintheta
			* sin(v.phi), .z = v.r * cos(v.theta) };
	return cv;
}

