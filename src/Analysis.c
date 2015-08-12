/*
 * Analysis.c
 *
 *  Created on: Jul 7, 2015
 *      Author: root
 */

#include "Analysis.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Constants.h"
#include "Controller.h"
#include "DataSet.h"
#include "Matrix.h"
#include "Vector.h"

void analyze_normalize_column(int column, CalibratedDataSet* data) {
	int i = 0;
	double sum, var, mu, sigma;
	i = 0;
	sum = 0;
	for (i = 0; i < data->len; i++) {
		sum += *dataset_calibrated_fieldByColumn(&(data->values[i]), column);
	}
	mu = sum / data->len;
	var = 0;
	for (i = 0; i < data->len; i++) {
		*dataset_calibrated_fieldByColumn(&(data->values[i]), column) -= mu;
		double dev = *dataset_calibrated_fieldByColumn(&(data->values[i]),
				column);
		var += dev * dev;
	}
	sigma = sqrt(var);
	if (sigma == 0)
		return;
	for (i = 0; i < data->len; i++) {
		*dataset_calibrated_fieldByColumn(&(data->values[i]), column) /= sigma;
	}
}

void analyze_smooth_column(int column, CalibratedDataSet* data) {
	double* runningTotal = malloc(data->len * sizeof(double));
	data->len--;
	double coldt(int i) {
		return (data->values[i + 1].t - data->values[i].t)
				* (*dataset_calibrated_fieldByColumn(&data->values[i], column));
	}
	runningTotal[0] = coldt(0);
	int i;
	for (i = 1; i < data->len; i++) {
		runningTotal[i] = runningTotal[i - 1] + coldt(i - 1);
	}
	int plusminus = radius_index(data);
	int pmFromEnd = data->len - 1 - plusminus;
	for (i = 0; i < data->len; i++) {
		int bottomOfInterval = i < plusminus ? 0 : i - plusminus;
		int topOfInterval = i >= pmFromEnd ? data->len - 1 : i + plusminus;
		double segmentColDT = runningTotal[topOfInterval]
				- runningTotal[bottomOfInterval];
		double segmentDT = data->values[topOfInterval].t
				- data->values[bottomOfInterval].t;
		double original = *dataset_calibrated_fieldByColumn(&data->values[i],
				column);
		double new = segmentColDT / segmentDT;
		printf(
				"Column %d, Location %d of %d; Bot %d, Top %d, SegmentColDt %f, SegmentDT %f; Original %f, Result %f\n",
				column, i, data->len, bottomOfInterval, topOfInterval,
				segmentColDT, segmentDT, original, new);
		*dataset_calibrated_fieldByColumn(&data->values[i], column) =
				segmentColDT / segmentDT;
	}
	printf("Exiting smooth helper\n");
}

void analyze_find_peaks(int column, CalibratedDataSet* data, PeakSet* peaks) {
	if(column == MAG_R) return;
	printf("Column %d\n", column);
	double valueI(int i) {
		return *dataset_calibrated_fieldByColumn(&(data->values[i]), column);
	}
	int plusminus = radius_index(data);
	printf("PLUS MINUS INTERVAL %d\n", plusminus);
	int pmFromEnd = data->len - 1 - plusminus;
	int i;
	for (i = 0; i < data->len; i++) {
		int bottomOfInterval = i < plusminus ? 0 : i - plusminus;
		int topOfInterval = i >= pmFromEnd ? data->len - 1 : i + plusminus;
		printf("GAP = %d\n", topOfInterval - bottomOfInterval);
		int j;
		int canBeMax = 1, canBeMin = 1;
		double val = valueI(i);
		for (j = bottomOfInterval; j <= topOfInterval && (canBeMin || canBeMax);
				j++) {
			double curVal = valueI(j);
			if (curVal > val)
				canBeMax = 0;
			if (curVal < val)
				canBeMin = 0;
		}
		if (canBeMax || canBeMin) {
			Peak current = { .in_what = column, .is_positive_peak = canBeMax,
					.t = data->values[i].t, .value = val };
			peakset_add(peaks, current);
		}
	}
}
CalibratedDataSet calibrate_joined_data(JoinedDataSet data) {
	CalibratedData* cals = malloc(data.len * sizeof(*cals));
	int i = 0;
	for (i = 0; i < data.len; i++) {
		JoinedData joined = data.values[i];
		Matrix th = Theta(joined.mag);
		PolarVector mag = vector_toPolar(orientation(th));
		Vector acc = normalized_accel(th, joined.acl);
		CalibratedData cal = { .t = joined.t, .acl = acc, .gyr = joined.gyr,
				.mag = mag };
		memcpy(&(cals[i]), &cal, sizeof(cal));
	}
	CalibratedDataSet set = { .is_normalized = 0, .is_smoothed = 0, .len =
			data.len, .values = cals };
	printf("DATA CALIBRATED: %d Lines\n", data.len);
	return set;
}

void normalize_calibrated_data(CalibratedDataSet* data) {
	if (data->is_normalized)
		return;
	if (!data->is_smoothed)
		smooth_calibrated_data(data);
	int i;
	for (i = 0; i <= LAST_CALIBRATED_COLUMN; i++)
		analyze_normalize_column(i, data);
	data->is_normalized = 1;
}

void smooth_calibrated_data(CalibratedDataSet* data) {
	if (data->is_smoothed)
		return;
	int i;
	for (i = 0; i <= LAST_CALIBRATED_COLUMN; i++) {
		analyze_smooth_column(i, data);
		printf("Column %d complete\n", i);
	}
	data->is_smoothed = 1;
	printf("Exiting smoothing function\n");
}

PeakSet* find_peaks_in_calibrated_data(CalibratedDataSet* data) {
	int cmp_Peak(const void * elem1, const void * elem2) {
		double t1 = ((Peak*) elem1)->t;
		double t2 = ((Peak*) elem2)->t;
		if (t1 > t2)
			return 1;
		if (t1 < t2)
			return -1;
		return 0;
	}
	if (!data->is_smoothed)
		smooth_calibrated_data(data);
	if (!data->is_normalized)
		normalize_calibrated_data(data);
	printf("Data smoothed: %d lines\n", data->len);
	PeakSet* ps = peakset_new();
	int i;
	for (i = 0; i <= LAST_CALIBRATED_COLUMN; i++)
		analyze_find_peaks(i, data, ps);
	printf("Found %d peaks\n", ps->size);
	qsort(ps->values, ps->size, sizeof(Peak), cmp_Peak);
	printf("Sorted peakset");
	return ps;
}
