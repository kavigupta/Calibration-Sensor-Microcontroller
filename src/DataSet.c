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

import_body(list, int);
import_body(list, long);
import_body(list, double);

import_body(list, JoinedData);
import_body(list, Peak);
import_body(list, list(Peak));
import_body(list, NDS);
import_body(list, Trial);

list(JoinedData) dataset_combine_vector4(Vector4* mag, int magl, Vector4* gyr,
		int gyrl, Vector4* acl, int acll, double dt) {
	int im = 0, ig = 0, ia = 0;
	int size = 0, capacity = 10;
	JoinedData* set = malloc(capacity * sizeof(JoinedData));
	while ((im < magl) && (ig < gyrl) && (ia < acll)) {
		JoinedData next =
				{ .mag = mag[im].v, .gyr = gyr[ig].v, .acl = acl[ia].v, .t =
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
	list(JoinedData) output = { .values = set, .size = size };
	return output;
}

Vector averageAcl(list(JoinedData) data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.size - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].acl.x * dt;
		y += data.values[i].acl.y * dt;
		z += data.values[i].acl.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}

Vector averageMag(list(JoinedData) data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.size - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].mag.x * dt;
		y += data.values[i].mag.y * dt;
		z += data.values[i].mag.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}
Vector averageGyr(list(JoinedData) data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.size - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].gyr.x * dt;
		y += data.values[i].gyr.y * dt;
		z += data.values[i].gyr.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}

double *dataset_column_get_field(CalibratedData* data, CalibratedColumn column) {
	switch (column) {
		case ACL_X:
			return &data->acl.x;
		case ACL_Y:
			return &data->acl.y;
		case ACL_Z:
			return &data->acl.z;
		case GYR_X:
			return &data->gyr.x;
		case GYR_Y:
			return &data->gyr.y;
		case GYR_Z:
			return &data->gyr.z;
		case MAG_PHI:
			return &data->mag.phi;
		case MAG_THETA:
			return &data->mag.theta;
		case MAG_R:
			return &data->mag.r;
	}
	return NULL;
}

char* dataset_column_render(CalibratedColumn column) {
	switch (column) {
		case ACL_X:
			return "aclx";
		case ACL_Y:
			return "acly";
		case ACL_Z:
			return "aclz";
		case GYR_X:
			return "gyrx";
		case GYR_Y:
			return "gyry";
		case GYR_Z:
			return "gyrz";
		case MAG_PHI:
			return "magphi";
		case MAG_THETA:
			return "magtheta";
		case MAG_R:
			return "magr";
	}
	return "COLUMN_NOT_FOUND";
}

double dataset_nds_duration(NDS nds) {
	return nds.data.values[nds.ind_end].t - nds.data.values[nds.ind_start].t;
}
