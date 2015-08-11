/*
 * DataSet.h
 *
 *  Created on: Jul 8, 2015
 *      Author: root
 */

#ifndef DATASET_H_
#define DATASET_H_

#include "Vector.h"

typedef struct {
	double t;
	Vector acl, gyr, mag;
} JoinedData;

typedef struct {
	double t;
	Vector acl, gyr;
	PolarVector mag;
} CalibratedData;

typedef struct {
	int len;
	JoinedData* values;
} JoinedDataSet;

typedef struct {
	int is_normalized;
	int is_smoothed;
	int len;
	CalibratedData* values;
} CalibratedDataSet;

JoinedDataSet combine(Vector4* mag, int magl, Vector4* gyr, int gryl,
		Vector4* acl, int acll, double dt);

Vector averageAcl(JoinedDataSet data);
Vector averageMag(JoinedDataSet data);
Vector averageGyr(JoinedDataSet data);

#endif /* DATASET_H_ */
