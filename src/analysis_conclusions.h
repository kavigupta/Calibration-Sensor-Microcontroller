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

/**
 * Generates a match given the list of trials, a list of which columns are
 * consistent, a list of the signatures corresponding to those columns,
 * and how many samples are requested.
 */
Match analysis_generate_match(list(Trial)* trials, list(int)* consistent_cols,
list(list(Peak))* signatures, int n_samples);

#endif /* ANALYSIS_CONCLUSIONS_H_ */
