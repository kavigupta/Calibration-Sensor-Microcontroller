/*
 * Analysis.h
 *
 *  Created on: Jul 7, 2015
 *      Author: root
 */

#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include "IO.h"
#include "Vector.h"

typedef struct {
	PolarVector pos;
	double t;
} TimePolarVector;

TimePolarVector* magnetometerData(DataSet data);

#endif /* ANALYSIS_H_ */
