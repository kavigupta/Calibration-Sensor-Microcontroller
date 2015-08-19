/*
 * Constants.h
 *
 *  Created on: Aug 11, 2015
 *      Author: root
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "analysis.h"
#include "DataSet.h"

#define SMOOTHING_RADIUS 0.4

extern const CurveDefinitionParameters DEFAULT_PARAMETERS;

int smoothing_radius_index(CalibratedDataList* data);

#endif /* CONSTANTS_H_ */
