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
} JoinedDataList;

typedef struct {
	int is_normalized;
	int is_smoothed;
	int len;
	CalibratedData* values;
} CalibratedDataList;

typedef enum {
	ACL_X, ACL_Y, ACL_Z, GYR_X, GYR_Y, GYR_Z, MAG_R, MAG_PHI, MAG_THETA
} CalibratedColumn;

typedef struct {
	int is_positive_peak;
	double t;
	double value;
} Peak;

typedef struct {
	int size, capacity;
	Peak* values;
} PeakList;

typedef struct {
	double t_start, t_end;
	PeakList cols[9];
} Trial;

typedef struct {
	int size, capacity;
	Trial* values;
} TrialList;

JoinedDataList dataset_combine_vector4(Vector4* mag, int magl, Vector4* gyr,
		int gryl, Vector4* acl, int acll, double dt);

Vector averageAcl(JoinedDataList data);
Vector averageMag(JoinedDataList data);
Vector averageGyr(JoinedDataList data);
/**
 * Gets the field in the calibrated data struct corresponding to the
 * given column
 */
double *dataset_column_get_field(CalibratedData* data, CalibratedColumn column);
/**
 * Returns a text representation of the given column
 */
char* dataset_column_render(CalibratedColumn column);
/**
 * Adds the given peak to the given PeakSet, expanding it as
 * necessary
 */
void dataset_peakset_add(PeakList* peaks, Peak peak);
/**
 * Constructs a new empty peakset. Call free() to deallocate
 * memory allocated in its construction.
 */
PeakList* dataset_peakset_new();
/**
 * Frees the given peakset.
 */
void dataset_peakset_free(PeakList* peaks);

#endif /* DATASET_H_ */
