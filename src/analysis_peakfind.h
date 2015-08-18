/*
 * analysis_peakfind.h
 *
 *  Created on: Aug 16, 2015
 *      Author: root
 */

#ifndef ANALYSIS_PEAKFIND_H_
#define ANALYSIS_PEAKFIND_H_

#include "DataSet.h"

/**
 * Finds peaks, using a similar index-interval as analysis_smooth.
 *
 * This removes the first and last to_be_removed trials, because often ends are
 * unreliable
 */
list(Trial)* analysis_peak_find_all(list(NDS)* data, int to_be_removed);

#endif /* ANALYSIS_PEAKFIND_H_ */
