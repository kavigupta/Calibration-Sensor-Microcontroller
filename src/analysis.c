/*
 * analysis.c
 *
 *  Created on: Aug 19, 2015
 *      Author: root
 */

#include "analysis.h"

#include <stdlib.h>

#include "analysis_conclusions.h"
#include "analysis_peakfind.h"
#include "analysis_preprocessing.h"
#include "analysis_segmentation.h"
#include "analysis_trial_separation.h"
#include "list.h"

CurveDefinitionTrace analysis_define_curve(list(list(JoinedData)) joined,
		CurveDefinitionParameters params) {
	CurveDefinitionTrace trace;
	trace.to_free = list_new_void_ptr();
	list(Trial) *trials = list_new_Trial();
	int i;
	for (i = 0; i < joined.size; i++) {
		CalibratedDataList cdl = analysis_calibrate(joined.values[i]);
		list_add_void_ptr(trace.to_free, cdl.values);
		analysis_normalize(&cdl);
		analysis_smooth(&cdl);
		list(NDS)* split = analysis_split_data(&cdl, params.jump_constant,
				params.min_duration);
		list(Trial)* trials_for_this_file = analysis_peak_find_all(split,
				params.to_be_removed);
		list_free_NDS(split);
		list_add_all_Trial(trials, trials_for_this_file);
		list_free_Trial(trials_for_this_file);
	}
	trace.trials = trials;
	PeakScalingParameters psp = analysis_scale_by_peaks(trials,
			params.n_cols_to_use, params.reject_nonstandardly_patterned_peaks);
	for (i = 0; i < trials->size; i++) {
		int col;
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			list_add_void_ptr(trace.to_free,
					trials->values[i].cols[col]->values);
			list_add_void_ptr(trace.to_free, trials->values[i].cols[col]);
		}
	}
	list_add_void_ptr(trace.to_free, trials->values);
	list_add_void_ptr(trace.to_free, trials);
	CurveDefinition cd = analysis_generate_match(trials, psp.consistent_cols,
			psp.used_signatures, psp.n_peaks * params.samples_per_peak);
	trace.def = cd;
	list_add_void_ptr(trace.to_free, cd.calibration_columns->values);
	list_add_void_ptr(trace.to_free, cd.calibration_columns);
	for (i = 0; i < cd.calibration_signatures->size; i++) {
		list_add_void_ptr(trace.to_free,
				cd.calibration_signatures->values[i].values);
	}
	for (i = 0; i <= LAST_CALIBRATED_COLUMN; i++) {
		list_add_void_ptr(trace.to_free, cd.distributions[i]->values);
		list_add_void_ptr(trace.to_free, cd.distributions[i]);
	}
	list_add_void_ptr(trace.to_free, cd.calibration_signatures->values);
	list_add_void_ptr(trace.to_free, cd.calibration_signatures);
	return trace;
}
