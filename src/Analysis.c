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

CalibratedDataSet analysis_calibrate(JoinedDataSet data) {
	CalibratedData* cals = malloc(data.len * sizeof(*cals));
	int i = 0;
	for (i = 0; i < data.len; i++) {
		JoinedData joined = data.values[i];
		Matrix th = cntrl_get_theta(joined.mag);
		PolarVector mag = vector_toPolar(cntrl_get_orientation(th));
		Vector acc = cntrl_get_adjusted_accel(th, joined.acl);
		CalibratedData cal = { .t = joined.t, .acl = acc, .gyr = joined.gyr,
				.mag = mag };
		memcpy(&(cals[i]), &cal, sizeof(cal));
	}
	CalibratedDataSet set = { .is_normalized = 0, .is_smoothed = 0, .len =
			data.len, .values = cals };
	printf("DATA CALIBRATED: %d Lines\n", data.len);
	return set;
}

void analysis_normalize(CalibratedDataSet* data) {
	if (data->is_normalized)
		return;
	if (!data->is_smoothed)
		analysis_smooth(data);
	int column;
	for (column = 0; column <= LAST_CALIBRATED_COLUMN; column++) {
		int i = 0;
		i = 0;
		double sum = 0;
		for (i = 0; i < data->len; i++) {
			sum += *dataset_column_get_field(&(data->values[i]),
					column);
		}
		double mu = sum / data->len;
		double var = 0;
		for (i = 0; i < data->len; i++) {
			*dataset_column_get_field(&(data->values[i]), column) -= mu;
			double dev = *dataset_column_get_field(&(data->values[i]),
					column);
			var += dev * dev;
		}
		double sigma = sqrt(var);
		if (sigma == 0)
			return;
		for (i = 0; i < data->len; i++) {
			*dataset_column_get_field(&(data->values[i]), column) /=
					sigma;
		}

	}
	data->is_normalized = 1;
}

void analysis_smooth(CalibratedDataSet* data) {
	double* runningTotal = malloc(data->len * sizeof(double));
	data->len--;
	if (data->is_smoothed)
		return;
	int column;
	for (column = 0; column <= LAST_CALIBRATED_COLUMN; column++) {
		double coldt(int i) {
			return (data->values[i + 1].t - data->values[i].t)
					* (*dataset_column_get_field(&data->values[i],
							column));
		}
		runningTotal[0] = coldt(0);
		int i;
		for (i = 1; i < data->len; i++) {
			runningTotal[i] = runningTotal[i - 1] + coldt(i - 1);
		}
		int plusminus = smoothing_radius_index(data);
		int pmFromEnd = data->len - 1 - plusminus;
		for (i = 0; i < data->len; i++) {
			int bottomOfInterval = i < plusminus ? 0 : i - plusminus;
			int topOfInterval = i >= pmFromEnd ? data->len - 1 : i + plusminus;
			double segmentColDT = runningTotal[topOfInterval]
					- runningTotal[bottomOfInterval];
			double segmentDT = data->values[topOfInterval].t
					- data->values[bottomOfInterval].t;
			double original = *dataset_column_get_field(
					&data->values[i], column);
			double new = segmentColDT / segmentDT;
			printf(
					"Column %d, Location %d of %d; Bot %d, Top %d, SegmentColDt %f, SegmentDT %f; Original %f, Result %f\n",
					column, i, data->len, bottomOfInterval, topOfInterval,
					segmentColDT, segmentDT, original, new);
			*dataset_column_get_field(&data->values[i], column) =
					segmentColDT / segmentDT;
		}
		printf("Exiting smooth helper\n");
	}
	data->is_smoothed = 1;
	printf("Exiting smoothing function\n");
	free(runningTotal);
}

PeakSet* analysis_peak_find(CalibratedDataSet* data) {
	if (!data->is_smoothed)
		analysis_smooth(data);
	if (!data->is_normalized)
		analysis_normalize(data);
	printf("Data smoothed: %d lines\n", data->len);
	PeakSet* ps = dataset_peakset_new();
	int column;
	for (column = 0; column <= LAST_CALIBRATED_COLUMN; column++) {
		printf("Column %d\n", column);
		double valueI(int i) {
			return *dataset_column_get_field(&(data->values[i]), column);
		}
		int plusminus = smoothing_radius_index(data);
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
			for (j = bottomOfInterval;
					j <= topOfInterval && (canBeMin || canBeMax); j++) {
				if (j == i)
					continue;
				double curVal = valueI(j);
				if (curVal >= val)
					canBeMax = 0;
				if (curVal <= val)
					canBeMin = 0;
			}
			if (canBeMax || canBeMin) {
				Peak current = { .in_what = column,
						.is_positive_peak = canBeMax, .t = data->values[i].t,
						.value = val };
				dataset_peakset_add(ps, current);
			}
		}

	}
	return ps;
}
