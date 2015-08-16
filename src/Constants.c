/*
 * Constants.c
 *
 *  Created on: Aug 15, 2015
 *      Author: root
 */

#include "Constants.h"


int smoothing_radius_index(CalibratedDataList* data) {
	int plusminus = 0;
	double initialT = data->values[0].t;
	while (plusminus < data->len
			&& (data->values[plusminus].t - initialT < SMOOTHING_RADIUS))
		plusminus++;
	return plusminus;
}
