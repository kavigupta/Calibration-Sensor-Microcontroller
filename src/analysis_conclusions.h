/*
 * analysis_conclusions.h
 *
 *  Created on: Aug 18, 2015
 *      Author: root
 */

#ifndef ANALYSIS_CONCLUSIONS_H_
#define ANALYSIS_CONCLUSIONS_H_

#include "DataSet.h"
#include "list.h"

typedef struct {
	int n_samples;
	double *cols[LAST_CALIBRATED_COLUMN + 1];
} Samples;
import_header(list, Samples);

/**
 * Generates a match given the list of trials, a list of which columns are
 * consistent, a list of the signatures corresponding to those columns,
 * and how many samples are requested.
 */
CurveDefinition analysis_generate_match(list(Trial)* trials,
list(int)* consistent_cols,
list(list(int))* signatures, int n_samples);

/**
 * Samples the given curve by taking n_samples points at even "t"
 * intervals
 */
Samples analysis_sample_points(Trial trial, int n_samples);

#endif /* ANALYSIS_CONCLUSIONS_H_ */
