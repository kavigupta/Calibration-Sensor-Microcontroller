/*
 * statistics.h
 *
 *  Created on: Aug 19, 2015
 *      Author: root
 */

#ifndef STATISTICS_H_
#define STATISTICS_H_

#include "DataSet.h"
#include "list.h"

/**
 * Calculates the incomplete gamma function with the given number of steps
 */
double incomplete_gamma(double s, double x, int n);

/**
 * Returns the p-value of the given value against the given
 * distribution.
 */
double statistics_p_value(Distribution dist, double x);
double statistics_composite_p(
list(Distribution) *distr[LAST_CALIBRATED_COLUMN + 1],
list(double) *values[LAST_CALIBRATED_COLUMN + 1]);

#endif /* STATISTICS_H_ */
