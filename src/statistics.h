/*
 * statistics.h
 *
 *  Created on: Aug 19, 2015
 *      Author: root
 */

#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <math.h>

#include "DataSet.h"

/**
 * Returns the p-value of the given value against the given
 * distribution.
 */
double statistics_p_value(Distribution dist, double x);

#endif /* STATISTICS_H_ */
