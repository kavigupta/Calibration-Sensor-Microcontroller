/*
 * analysis.h
 *
 *  Created on: Aug 19, 2015
 *      Author: root
 */

#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include "DataSet.h"

typedef struct {
	double jump_constant;
	double min_duration;
	int to_be_removed;
	int n_cols_to_use;
	int reject_nonstandardly_patterned_peaks;
	int samples_per_peak;
} CurveDefinitionParameters;

typedef struct {
	CurveDefinition def;
	list(Trial) *trials;
	list(void_ptr) *to_free;
} CurveDefinitionTrace;

CurveDefinitionTrace analysis_define_curve(list(list(JoinedData)) joined,
		CurveDefinitionParameters params);

int processor_matches(CurveDefinition curve, NDS t);

#endif /* ANALYSIS_H_ */
