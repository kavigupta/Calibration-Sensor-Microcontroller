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

void analyze_normalize_column(int column, CalibratedDataSet* data);
void analyze_smooth_column(int column, CalibratedDataSet* data);
void analyze_find_peaks(int column, CalibratedDataSet* data, PeakSet* peaks);
CalibratedDataSet calibrate_joined_data(JoinedDataSet data);
void normalize_calibrated_data(CalibratedDataSet* data);
void smooth_calibrated_data(CalibratedDataSet* data);
PeakSet* find_peaks_in_calibrated_data(CalibratedDataSet* data);

#endif /* ANALYSIS_H_ */
