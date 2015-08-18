/*
 * analysis_whole_data_processing.c
 *
 *  Created on: Aug 15, 2015
 *      Author: root
 */

#include <math.h>
#include <stdlib.h>

#include "Constants.h"
#include "Controller.h"
#include "DataSet.h"
#include "list.h"
#include "Matrix.h"
#include "Vector.h"


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
		int i;
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
