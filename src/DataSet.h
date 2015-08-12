/*
 * DataSet.h
 *
 *  Created on: Jul 8, 2015
 *      Author: root
 */

#ifndef DATASET_H_
#define DATASET_H_

#include "Vector.h"
#include <stddef.h>

#define LAST_CALIBRATED_COLUMN MAG_THETA

typedef struct {
	double t;
	Vector acl, gyr, mag;
} JoinedData;

typedef struct {
	double t;
	Vector acl, gyr;
	PolarVector mag;
} CalibratedData;

typedef union {
	JoinedData joined;
	CalibratedData calibrated;
	double data[10];
} Double10;

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

typedef enum {
	ACL_X, ACL_Y, ACL_Z, GYR_X, GYR_Y, GYR_Z, MAG_R, MAG_PHI, MAG_THETA
} CalibratedColumn;

typedef struct {
	CalibratedColumn in_what;
	int is_positive_peak;
	double t;
	double value;
} Peak;

typedef struct {
	int size, capacity;
	Peak* values;
} PeakSet;

#define GET_OFFSET(STRUCT, FIELD, FIELD_TYPE, SUBFIELD) \
	((int)offsetof(STRUCT, FIELD) + (int)offsetof(FIELD_TYPE, SUBFIELD))

#define GET_CALIDATE_OFF(FIELD, DIM) GET_OFFSET(CalibratedData, FIELD, Vector, DIM)

JoinedDataSet combine(Vector4* mag, int magl, Vector4* gyr, int gryl,
		Vector4* acl, int acll, double dt);

Vector averageAcl(JoinedDataSet data);
Vector averageMag(JoinedDataSet data);
Vector averageGyr(JoinedDataSet data);
double *dataset_calibrated_fieldByColumn(CalibratedData* data,
		CalibratedColumn column);
char* renderColumn(CalibratedColumn column);

void peakset_add(PeakSet* peaks, Peak peak);
void peakset_grow(PeakSet* peaks);
PeakSet* peakset_new();

#endif /* DATASET_H_ */
