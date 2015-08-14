/*
 * Analysis.c
 *
 *  Created on: Jul 7, 2015
 *      Author: root
 */

#include "Analysis.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "arraylist.h"
#include "Constants.h"
#include "Controller.h"
#include "DataSet.h"
#include "generics.h"
#include "Matrix.h"
#include "Vector.h"

double* analysis_get_diff_trace(CalibratedDataList* data);

CalibratedDataList analysis_calibrate(list(JoinedData) data) {
	CalibratedData* cals = malloc(data.size * sizeof(*cals));
	int i = 0;
	for (i = 0; i < data.size; i++) {
		JoinedData joined = data.values[i];
		Matrix th = cntrl_get_theta(joined.mag);
		PolarVector mag = vector_toPolar(cntrl_get_orientation(th));
		Vector acc = cntrl_get_adjusted_accel(th, joined.acl);
		CalibratedData cal = { .t = joined.t, .acl = acc, .gyr = joined.gyr,
				.mag = mag };
		cals[i] = cal;
	}
	CalibratedDataList set = { .is_normalized = 0, .is_smoothed = 0, .len =
			data.size, .values = cals };
	//printf("DATA CALIBRATED: %d Lines\n", data.len);
	return set;
}

void analysis_smooth(CalibratedDataList* data) {
	double* runningTotal = malloc(data->len * sizeof(double));
	data->len--;
	if (data->is_smoothed)
		return;
	int column;
	for (column = 0; column <= LAST_CALIBRATED_COLUMN; column++) {
		double coldt(int i) {
			return (data->values[i + 1].t - data->values[i].t)
					* (*dataset_column_get_field(&data->values[i], column));
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
//			double original = *dataset_column_get_field(&data->values[i],
//					column);
//			double new = segmentColDT / segmentDT;
//			printf(
//					"Column %d, Location %d of %d; Bot %d, Top %d, SegmentColDt %f, SegmentDT %f; Original %f, Result %f\n",
//					column, i, data->len, bottomOfInterval, topOfInterval,
//					segmentColDT, segmentDT, original, new);
			*dataset_column_get_field(&data->values[i], column) = segmentColDT
					/ segmentDT;
		}
	}
	data->is_smoothed = 1;
	free(runningTotal);
}

void analysis_normalize(CalibratedDataList* data) {
	if (data->is_normalized)
		return;
	if (!data->is_smoothed)
		analysis_smooth(data);
	int column;
	for (column = 0; column <= LAST_CALIBRATED_COLUMN; column++) {
		//printf("Normalizing %d\n", column);
		int i = 0;
		i = 0;
		double sum = 0;
		for (i = 0; i < data->len; i++) {
			sum += *dataset_column_get_field(&(data->values[i]), column);
		}
		double mu = sum / data->len;
		double var = 0;
		for (i = 0; i < data->len; i++) {
			*dataset_column_get_field(&(data->values[i]), column) -= mu;
			double dev = *dataset_column_get_field(&(data->values[i]), column);
			var += dev * dev;
		}
		double sigma = sqrt(var);
		if (sigma == 0)
			continue;
		for (i = 0; i < data->len; i++) {
			*dataset_column_get_field(&(data->values[i]), column) /= sigma;
		}
	}
	data->is_normalized = 1;
}

list(NDS)* analysis_split_data(CalibratedDataList* data, double jumpConstant,
		double minimumDuration) {
	double* diffTrace = analysis_get_diff_trace(data);
	double threshold;
	{
		int cmp_double(const void *x, const void *y) {
			double xx = *(double*) x, yy = *(double*) y;
			if (xx < yy)
				return -1;
			if (xx > yy)
				return 1;
			return 0;
		}
		// will find threshhold
		double* sorted = malloc(sizeof(double) * data->len);
		memcpy(sorted, diffTrace, sizeof(double) * data->len);
		qsort(sorted, data->len, sizeof(double), cmp_double);
		threshold = sorted[data->len / 10] * jumpConstant;
		free(sorted);
	}
	list(NDS)* list = list_new_NDS();
	int startingState = diffTrace[0] > threshold;
	int lastState = startingState;
	int endPrev = 0;
	int i;
	for (i = 0; i < data->len; i++) {
		int thisState = diffTrace[i] > threshold;
		if (i != data->len - 1) {
			// otherwise, no matter what, pop it on
			if (thisState && lastState) { // continue moving
				continue;
			} else if (!thisState && !lastState) { // continue resting
				continue;
			}
		}
		NDS seg = { .data = *data, .ind_start = endPrev, .ind_end = i };
		list_add_NDS(list, seg);
		endPrev = i;
		lastState = thisState;
	}
	free(diffTrace);
	list(NDS) *filteredForLength = list_new_NDS();
	// filter the NDS list
	for (i = 0; i < list->size; i++) {
		if (i == 0 || dataset_nds_duration(list->values[i]) > minimumDuration) {
			list_add_NDS(filteredForLength, list->values[i]);
			continue;
		}
		if (i != list->size - 1)
			i++;
		filteredForLength->values[filteredForLength->size - 1].ind_end =
				list->values[i].ind_end;
	}
	list(NDS) *onlyMovement = list_new_NDS();
	// add twos from the first moving chunk.
	// if 0 -> startingState == 1, then i%2 == 0 -> moving
	// otherwise, i%2 != 0 -> moving
	for (i = startingState ? 0 : 1; i < filteredForLength->size; i += 2) {
		list_add_NDS(onlyMovement, filteredForLength->values[i]);
	}
	free(list);
	free(filteredForLength);
	return onlyMovement;
}

Trial analysis_peak_find(NDS data) {
	//	printf("Data smoothed: %d lines\n", data->len);
	int column;
	list(Peak) *ps[9];
	for (column = 0; column <= LAST_CALIBRATED_COLUMN; column++) {
		ps[column] = list_new_Peak();
		//		printf("Column %d\n", column);
		double valueI(int i) {
			return *dataset_column_get_field(&(data.data.values[i]), column);
		}
		int plusminus = smoothing_radius_index(&data.data);
		//		printf("PLUS MINUS INTERVAL %d\n", plusminus);
		int pmFromStart = data.ind_start + plusminus;
		int pmFromEnd = data.ind_end - 1 - plusminus;
		int i;
		for (i = data.ind_start; i < data.ind_end; i++) {
			int bottomOfInterval =
					i < pmFromStart ? data.ind_start : i - plusminus;
			int topOfInterval =
					i >= pmFromEnd ? data.ind_end - 1 : i + plusminus;
			int j;
			double val = valueI(i);
			int canBeMax = 1, canBeMin = 1;
			for (j = bottomOfInterval;
					j <= topOfInterval && (canBeMin || canBeMax); j++) {
				if (j == i)
					continue;
				double curVal = valueI(j);
				if (val <= curVal)
					canBeMax = 0;
				if (val >= curVal)
					canBeMin = 0;
			}
			if (canBeMax || canBeMin) {
				Peak current = { .is_positive_peak = canBeMax, .t =
						data.data.values[i].t, .value = val };
				list_add_Peak(ps[column], current);
			}
		}
	}
	Trial t = { .data = data };
	int i;
	for (i = 0; i < 9; i++)
		t.cols[i] = ps[i];
	return t;
}

list(Trial)* analysis_peak_find_all(list(NDS)* data) {
	list(Trial)* tl = list_new_Trial();
	int seg;
	for (seg = 0; seg < data->size; seg++) {
		Trial trial = analysis_peak_find(data->values[seg]);
		list_add_Trial(tl, trial);
	}
	return tl;
}

void analysis_scale_by_peaks(list(NDS)* data){
	int n_consistent_peaks = 0;
//	int *consistent_peaks = find_consistent_peaks(data, n_consistent_peaks);
}

/**
 * Calculates the sum over every row of the square of the difference of each
 * element and the element in the next column.
 */
double* analysis_get_diff_trace(CalibratedDataList* data) {
	if (!data->is_normalized)
		analysis_normalize(data);
	CalibratedDataList diffs =
			{ .is_normalized = 0, .is_smoothed = 1, .len = data->len - 1,
					.values = malloc(data->len * sizeof(CalibratedData)) };
	int i;
	for (i = 0; i < diffs.len; i++) {
		int col;
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			*dataset_column_get_field(&diffs.values[i], col) =
					*dataset_column_get_field(&data->values[i + 1], col)
							- *dataset_column_get_field(&data->values[i], col);
		}
	}
	analysis_normalize(&diffs);
	double* sqNormDiffs = calloc(diffs.len, sizeof(double));
	for (i = 0; i < diffs.len; i++) {
		int col;
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			double normDiff = *dataset_column_get_field(&diffs.values[i], col);
			sqNormDiffs[i] += normDiff * normDiff;
		}
	}
	return sqNormDiffs;
}
