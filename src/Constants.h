/*
 * Constants.h
 *
 *  Created on: Aug 11, 2015
 *      Author: root
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define SMOOTHING_RADIUS 0.4

int radius_index(CalibratedDataSet* data) {
	int plusminus = 0;
	double initialT = data->values[0].t;
	while (plusminus < data->len
			&& (data->values[plusminus].t - initialT < SMOOTHING_RADIUS))
		plusminus++;
	return plusminus;
}

#endif /* CONSTANTS_H_ */
