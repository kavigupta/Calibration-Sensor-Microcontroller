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
} RawData;

typedef struct {
	int len;
	RawData* values;
} RawDataSet;

RawDataSet combine(Vector4* mag, int magl, Vector4* gyr, int gryl, Vector4* acl,
		int acll, double dt);

Vector averageAcl(RawDataSet data);
Vector averageMag(RawDataSet data);
Vector averageGyr(RawDataSet data);

#endif /* DATASET_H_ */
