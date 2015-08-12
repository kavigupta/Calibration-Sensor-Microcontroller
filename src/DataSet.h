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

JoinedDataSet dataset_combine_vector4(Vector4* mag, int magl, Vector4* gyr, int gryl,
		Vector4* acl, int acll, double dt);

Vector averageAcl(JoinedDataSet data);
Vector averageMag(JoinedDataSet data);
Vector averageGyr(JoinedDataSet data);
/**
 * Gets the field in the calibrated data struct corresponding to the
 * given column
 */
double *dataset_column_get_field(CalibratedData* data,
		CalibratedColumn column);
/**
 * Returns a text representation of the given column
 */
char* dataset_column_render(CalibratedColumn column);
/**
 * Adds the given peak to the given PeakSet, expanding it as
 * necessary
 */
void dataset_peakset_add(PeakSet* peaks, Peak peak);
/**
 * Constructs a new empty peakset. Call free() to deallocate
 * memory allocated in its construction.
 */
PeakSet* dataset_peakset_new();
/**
 * Frees the given peakset.
 */
void dataset_peakset_free(PeakSet* peaks);

#endif /* DATASET_H_ */
