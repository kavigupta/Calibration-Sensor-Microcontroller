/*
 * Analysis.h
 *
 *  Created on: Jul 7, 2015
 *      Author: root
 */

#ifndef ANALYSIS_SEGMENTATION_H_
#define ANALYSIS_SEGMENTATION_H_

#include "IO.h"
#include "Vector.h"
#include "DataSet.h"

typedef struct {
	int n_peaks;
	list(int)* consistent_cols;
	list(list(int))* used_signatures;
} PeakScalingParameters;

/**
 * Scales the given trials' timestamps by their most consistent peaks
 * so that the trials' timestamps are correlated by the time within a
 * motion. This will allow the comparison of "time" positions that
 * correspond to points in an action rather than in absolute time.
 *
 * ncols === number of columns to use
 *
 * Returns which peaks were used to arrive at its conclusion.
 */
PeakScalingParameters analysis_scale_by_peaks(list(Trial)* lTrials, int ncols,
		int reject_nonstandardly_patterned_peaks);

#endif /* ANALYSIS_SEGMENTATION_H_ */
