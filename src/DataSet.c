/*
 * DataSet.c
 *
 *  Created on: Jul 8, 2015
 *      Author: root
 */

#include "DataSet.h"

#include <stdlib.h>
#include <string.h>

#define MIN(a,b) ((a)<(b)?(a):(b))

JoinedDataSet combine(Vector4* mag, int magl, Vector4* gyr, int gyrl, Vector4* acl,
		int acll, double dt) {
	int im = 0, ig = 0, ia = 0;
	int size = 0, capacity = 10;
	JoinedData* set = malloc(capacity * sizeof(JoinedData));
	while ((im < magl) && (ig < gyrl) && (ia < acll)) {
		JoinedData next = { .mag = mag[im].v, .gyr = gyr[ig].v, .acl = acl[ia].v, .t =
				MIN(MIN(mag[im].t, gyr[ig].t), acl[ia].t) };
		memcpy(&(set[size]), &next, sizeof(JoinedData));
		size++;
		if (size == capacity) {
			capacity = capacity * 3 / 2;
			set = realloc(set, capacity * sizeof(JoinedData));
		}
		while (im < magl && (mag[im].t < set[size - 1].t + dt)) {
			im++;
		}
		while (ig < gyrl && (gyr[ig].t < set[size - 1].t + dt)) {
			ig++;
		}
		while (ia < acll && (acl[ia].t < set[size - 1].t + dt)) {
			ia++;
		}
	}
	JoinedDataSet output = { .values = set, .len = size };
	return output;
}

Vector averageAcl(JoinedDataSet data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.len - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].acl.x * dt;
		y += data.values[i].acl.y * dt;
		z += data.values[i].acl.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}

Vector averageMag(JoinedDataSet data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.len - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].mag.x * dt;
		y += data.values[i].mag.y * dt;
		z += data.values[i].mag.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}
Vector averageGyr(JoinedDataSet data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.len - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].gyr.x * dt;
		y += data.values[i].gyr.y * dt;
		z += data.values[i].gyr.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}
