/*
 * analysis_preprocessing.h
 *
 *  Created on: Aug 15, 2015
 *      Author: root
 */

#ifndef ANALYSIS_PREPROCESSING_H_
#define ANALYSIS_PREPROCESSING_H_

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


#endif /* ANALYSIS_PREPROCESSING_H_ */
