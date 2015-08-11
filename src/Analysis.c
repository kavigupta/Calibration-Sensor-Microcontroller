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

#include "Constants.h"
#include "Controller.h"
#include "DataSet.h"
#include "Matrix.h"
#include "Vector.h"

/*Short for Dereference Offset Constant. Offsets the given pointer
 * by the given amount, and returns a LHS value for that position
 * with the given type*/
#define DOC(TYPE, POINTER, BYTES) \
		(*((TYPE*) ((char*)POINTER + BYTES)))

#define CALL_ON_ALL(FUNCTION, ...) \
	{\
		int i;\
		for(i = ACL_X; i <=MAG_R; i++){\
			FUNCTION(i, ##__VA_ARGS__);\
		}\
	}\

void analyze_normalize(int column, CalibratedDataSet* data) {
	int offset = offsetOf(column);
	int i = 0;
	double sum, var, mu, sigma;
	i = 0;
	sum = 0;
	for (i = 0; i < data->len; i++) {
		sum += DOC(double, &(data->values[i]), offset);
	}
	mu = sum / data->len;
	var = 0;
	for (i = 0; i < data->len; i++) {
		DOC(double, &(data->values[i]), offset) -= mu;
		double dev = DOC(double, &(data->values[i]), offset);
		var += dev * dev;
	}
	sigma = sqrt(var);
	for (i = 0; i < data->len; i++) {
		DOC(double, &(data->values[i]), offset) /= sigma;
	}
}

void analyze_smooth(int column, CalibratedDataSet* data) {
	int offset = offsetOf(column);
	double runningSum = DOC(double, &(data->values[0]), offset);
	int numMeasure = 1;
	int last = 0;
	int i = 1;
	for (i = 1; i < data->len; i++) {
		numMeasure++;
		runningSum += DOC(double, &(data->values[i]), offset);
		DOC(double, &(data->values[i]), offset) = runningSum / numMeasure;
		while (data->values[i].t - data->values[last].t > DT_GAP) {
			runningSum -= DOC(double, &(data->values[i]), offset);
			numMeasure--;
			last++;
		}
	}
}

void analyze_find_peaks(int column, CalibratedDataSet* data, PeakSet* peaks) {
	double valueI(int i) {
		return DOC(double, &data->values[i], offsetOf(column));
	}
	int increasing = valueI(1) > valueI(0);
	int i;
	for (i = 1; i < data->len; i++) {
		int inc = valueI(i + 1) > valueI(i);
		if (inc != increasing) {
			// is_positive_peak == is transitioning from / to \ == !inc
			Peak current = { .in_what = column, .is_positive_peak = !inc, .t =
					data->values[i].t, .value = valueI(i) };
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
	return set;
}

void normalize_calibrated_data(CalibratedDataSet* data) {
	if (data->is_normalized)
		return;
	CALL_ON_ALL(analyze_normalize, data);
	data->is_normalized = 1;
}

void smooth_calibrated_data(CalibratedDataSet* data) {
	if (data->is_smoothed)
		return;
	if (!data->is_normalized)
		normalize_calibrated_data(data);
	CALL_ON_ALL(analyze_smooth, data);
}

PeakSet find_peaks_in_calibrated_data(CalibratedDataSet* data) {
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
	PeakSet ps = peakset_new();
	CALL_ON_ALL(analyze_find_peaks, data, &ps);
	qsort(ps.values, ps.size, sizeof(Peak), cmp_Peak);
	return ps;
}
