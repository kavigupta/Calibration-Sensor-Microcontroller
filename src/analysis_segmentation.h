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
/**
 * Scales the given trial with the peaks it has in the consistent columns provided
 */
void analysis_scale_trial_by_peaks(Trial* tr, list(int)* consistent_cols);
/**
 * Coerces peaks for the given trial into the standard signature shown.
 */
int analysis_coerce_peaks_for_single_column(Trial* tr, int col,
list(Peak)* standard, int reject_nonstandardly_patterned_peaks);

#endif /* ANALYSIS_SEGMENTATION_H_ */
