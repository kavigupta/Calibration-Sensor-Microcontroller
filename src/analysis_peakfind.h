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
 */
list(Trial)* analysis_peak_find_all(list(NDS)* data);

#endif /* ANALYSIS_PEAKFIND_H_ */
