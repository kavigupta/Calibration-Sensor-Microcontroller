/*
 * analysis_trial_separation.c
 *
 *  Created on: Aug 15, 2015
 *      Author: root
 */

#include "analysis_trial_separation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analysis_preprocessing.h"
#include "DataSet.h"
#include "list.h"

/**
 * Calculates the sum over every row of the square of the difference of each
 * element and the element in the next column.
 *
 * Defined here so that it is a private method to this module
 */
double* analysis_get_diff_trace(CalibratedDataList* data);

list(NDS)* analysis_split_data(CalibratedDataList* data, double jumpConstant,
		double minimumDuration) {
	double* diffTrace = analysis_get_diff_trace(data);
	data->len--;
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
		if (i != data->len - 1) { // last element = next state will be different (i.e., nothing)
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
		if (i != list->size - 1) {
			i++;
			filteredForLength->values[filteredForLength->size - 1].ind_end =
					list->values[i].ind_end;
		}
	}
	list(NDS) *onlyMovement = list_new_NDS();
	// add twos from the first moving chunk.
	// if 0 -> startingState == 1, then i%2 == 0 -> moving
	// otherwise, i%2 != 0 -> moving
	for (i = startingState ? 0 : 1; i < filteredForLength->size; i += 2) {
		list_add_NDS(onlyMovement, filteredForLength->values[i]);
	}
	list_free_NDS(list);
	list_free_NDS(filteredForLength);
	return onlyMovement;
}

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
	free(diffs.values);
	return sqNormDiffs;
}
