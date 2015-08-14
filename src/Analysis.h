/*
 * Analysis.h
 *
 *  Created on: Jul 7, 2015
 *      Author: root
 */

#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include "IO.h"
#include "Vector.h"
#include "DataSet.h"

/**
 * Calibrates sensor data against the recorded magnetometer readings
 * taken during the calibration phase. Will not free the original data
 * set. TODO this function may need to be checked to ensure that the
 * physics it uses is correct.
 */
CalibratedDataList analysis_calibrate(list(JoinedData) data);
/**
 * Smooths sensor data by replacing the data at any given point with
 * the average of the data taken in a neighborhood of the data, which
 * is estimated from the first few data points to be approximately
 * the width of the declared time radius in Constants.h
 */
void analysis_smooth(CalibratedDataList* data);
/**
 * Normalizes sensor data by subtracting out the mean and dividing out
 * the standard deviation (producing all zeroes if SD = 0), smoothing
 * it first if necessary.
 */
void analysis_normalize(CalibratedDataList* data);
/**
 * Splits the given data set into segments, each of which represents
 * a single major action of interest.
 */
list(NDS)* analysis_split_data(CalibratedDataList* data, double jumpConstant,
		double minimumDuration);
/**
 * Finds peaks, using a similar index-interval as analysis_smooth.
 */
list(Trial)* analysis_peak_find_all(list(NDS)* data);

/**
 * Scales the given trials' timestamps by their most consistent peaks
 * so that the trials' timestamps are correlated by the time within a
 * motion. This will allow the comparison of "time" positions that
 * correspond to points in an action rather than in absolute time.
 */
void analysis_scale_by_peaks(list(NDS)* data);

#endif /* ANALYSIS_H_ */
