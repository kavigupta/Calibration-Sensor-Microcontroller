/*
 * statistics.c
 *
 *  Created on: Aug 19, 2015
 *      Author: root
 */

#include "statistics.h"

#include <math.h>

#include "DataSet.h"

double statistics_p_value(Distribution dist, double x) {
	double z = (x - dist.mu) / dist.sigma;
	double erfp1 = 1.0 + erf(z / sqrt(2));
	return erfp1 / 2.0;
}
