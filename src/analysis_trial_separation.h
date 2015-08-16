/*
 * analysis_trial_separation.h
 *
 *  Created on: Aug 15, 2015
 *      Author: root
 */

#ifndef ANALYSIS_TRIAL_SEPARATION_H_
#define ANALYSIS_TRIAL_SEPARATION_H_

#include "arraylist.h"
#include "DataSet.h"

/**
 * Splits the given data set into segments, each of which represents
 * a single major action of interest.
 */
list(NDS)* analysis_split_data(CalibratedDataList* data, double jumpConstant,
		double minimumDuration);

#endif /* ANALYSIS_TRIAL_SEPARATION_H_ */
