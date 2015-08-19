/*
 * Constants.c
 *
 *  Created on: Aug 15, 2015
 *      Author: root
 */

#include "Constants.h"

const CurveDefinitionParameters DEFAULT_PARAMETERS = { .jump_constant = 20, .min_duration =
		.4, .to_be_removed = 2, .n_cols_to_use = 1, .samples_per_peak = 2,
		.reject_nonstandardly_patterned_peaks = 1 };

int smoothing_radius_index(CalibratedDataList* data) {
	int plusminus = 0;
	double initialT = data->values[0].t;
	while (plusminus < data->len
			&& (data->values[plusminus].t - initialT < SMOOTHING_RADIUS))
		plusminus++;
	return plusminus;
}
