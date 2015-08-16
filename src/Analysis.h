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
 * Finds peaks, using a similar index-interval as analysis_smooth.
 */
list(Trial)* analysis_peak_find_all(list(NDS)* data);

/**
 * Scales the given trials' timestamps by their most consistent peaks
 * so that the trials' timestamps are correlated by the time within a
 * motion. This will allow the comparison of "time" positions that
 * correspond to points in an action rather than in absolute time.
 *
 * ncols === number of columns to use
 */
void analysis_scale_by_peaks(list(Trial)* data, int ncols);

#endif /* ANALYSIS_H_ */
