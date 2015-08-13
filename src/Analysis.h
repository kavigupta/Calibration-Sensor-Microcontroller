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

/**
 * Calibrates sensor data against the recorded magnetometer readings
 *  taken during the calibration phase.
 */
CalibratedDataList analysis_calibrate(JoinedDataList data);
/**
 * Smooths sensor data by replacing the data at any given point with
 * the average of the data taken in a neighborhood of the data, which
 * is estimated from the first few data points to be approximately
 * the width of the declared time radius in Constants.h
 */
void analysis_smooth(CalibratedDataList* data);
/**
 * Normalizes sensor data by subtracting out the mean and dividing out
 * the standard deviation (0 / 0 -> 0), smoothing it first if necessary.
 */
void analysis_normalize(CalibratedDataList* data);
/**
 * Finds peaks, using a similar index-interval as analysis_smooth.
 */
PeakList** analysis_peak_find(CalibratedDataList* data);

#endif /* ANALYSIS_H_ */
