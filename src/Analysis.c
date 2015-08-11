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

#include "Controller.h"
#include "DataSet.h"
#include "Matrix.h"
#include "Vector.h"

#define CALL_ON_ALL(MACRO, DATA) \
	MACRO(DATA, acl, x); \
	MACRO(DATA, acl, y); \
	MACRO(DATA, acl, z); \
	MACRO(DATA, gyr, x); \
	MACRO(DATA, gyr, y); \
	MACRO(DATA, gyr, z); \
	MACRO(DATA, mag, phi); \
	MACRO(DATA, mag, theta); \
	MACRO(DATA, mag, r); \


#define NORMALIZE(DATA, FIELD, DIM) \
	{\
		int i = 0;\
		double sum, var, mu, sigma;\
		i = 0;\
		sum = 0;\
		for(i = 0; i < DATA.len; i++){\
			sum += DATA.values[i].FIELD.DIM;\
		}\
		mu = sum / DATA.len;\
		var = 0;\
		for(i = 0; i<DATA.len; i++){\
			DATA.values[i].FIELD.DIM -= mu;\
			var += DATA.values[i].FIELD.DIM * DATA.values[i].FIELD.DIM;\
		}\
		sigma = sqrt(var);\
		for(i = 0; i<DATA.len; i++){\
			DATA.values[i].FIELD.DIM /= sigma;\
		}\
	}

#define DT_GAP 0.1

#define SMOOTH(DATA, FIELD, DIM)\
	{\
		double runningSum = DATA.values[0].FIELD.DIM;\
		int numMeasure = 1;\
		int last = 0;\
		int i = 1;\
		for(i = 1; i < DATA.len; i++){\
			numMeasure++; \
			runningSum += DATA.values[i].FIELD.DIM;\
			DATA.values[i].FIELD.DIM = runningSum / numMeasure;\
			while(DATA.values[i].t - DATA.values[last].t > DT_GAP){\
				runningSum -= DATA.values[last].FIELD.DIM;\
				numMeasure --;\
				last++;\
			}\
		}\
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

void normalize_calibrated_data(CalibratedDataSet data) {
	if (data.is_normalized)
		return;
	CALL_ON_ALL(NORMALIZE, data);
	data.is_normalized = 1;
}

void smooth_calibrated_data(CalibratedDataSet data) {
	if (data.is_smoothed)
		return;
	if (!data.is_normalized)
		normalize_calibrated_data(data);
	CALL_ON_ALL(SMOOTH, data);
}

