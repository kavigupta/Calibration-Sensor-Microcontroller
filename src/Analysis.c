/*
 * Analysis.c
 *
 *  Created on: Jul 7, 2015
 *      Author: root
 */

#include "Analysis.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Controller.h"

TimePolarVector* magnetometerData(DataSet data) {
	TimePolarVector * polars = malloc((data.len + 1) * sizeof(TimePolarVector));
	int i = 0;
	for (i = 0; i < data.len; i++) {
		TimePolarVector tpv = { .t = data.values[i].t, .pos = vector_toPolar(
				orientation(Theta(data.values[i].mag))) };
		memcpy(&(polars[i]), &tpv, sizeof(TimePolarVector));
	}
	TimePolarVector terminate = { .t = -1 };
	memcpy(&(polars[data.len]), &terminate, sizeof(TimePolarVector));
	return polars;
}

