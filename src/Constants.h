/*
 * Constants.h
 *
 *  Created on: Aug 11, 2015
 *      Author: root
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "DataSet.h"

#define SMOOTHING_RADIUS 0.4
#define JUMP_CONSTANT 20

#define MINIMUM_TRIAL_DURATION 0.4
#define TRIALS_REMOVED_FROM_EITHER_END 2

#define COLS_USED_FOR_PEAK_DET 1
#define REMOVE_NONSTANDARD_COLS 1

#define SAMPLES_PER_SEGMENT 2

int smoothing_radius_index(CalibratedDataList* data);

#endif /* CONSTANTS_H_ */
