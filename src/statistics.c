/*
 * statistics.c
 *
 *  Created on: Aug 19, 2015
 *      Author: root
 */

#include "statistics.h"

#include <math.h>

#include "DataSet.h"
#include "list.h"

double statistics_p_value(Distribution dist, double x) {
	double z = (x - dist.mu) / dist.sigma;
	double erfp1 = 1.0 + erf(z / sqrt(2));
	return erfp1 / 2.0;
}

double statistics_composite_p(
list(Distribution) *distr[LAST_CALIBRATED_COLUMN + 1],
list(double) *values[LAST_CALIBRATED_COLUMN + 1]) {
	double chi_sq_over_2 = 0;
	int df_over_2 = 0;
	int col;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		int i;
		for (i = 0; i < distr[col]->size; i++) {
			double p_val = statistics_p_value(distr[col]->values[i],
					values[col]->values[i]);
			chi_sq_over_2 += -log(p_val);
			df_over_2++;
		}
	}
	return 1
			- incomplete_gamma(df_over_2, chi_sq_over_2, 10000)
					/ tgamma(df_over_2);
}

double incomplete_gamma(double s, double x, int n) {
	// integral from 0 to x of exp(-t + (s-1)(log t)) dt
	double dt = x / n;
	double integral;
	int i;
	for (i = 0; i < n; i++) {
		double t = dt * i;
		integral += exp(-t + (s - 1) * log(t)) * dt;
	}
	return integral;
}
