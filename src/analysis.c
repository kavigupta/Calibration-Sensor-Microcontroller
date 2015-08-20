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

int processor_matches(CurveDefinition curve, NDS t) {
	// a list of possibilities for the given trial
	Trial tr;
	{
		list(NDS) *nds_pass = list_new_NDS();
		list(Trial) *poss = analysis_peak_find_all(nds_pass, 0);
		if (poss->size < 1) {
			list_free_NDS(nds_pass);
			list_free_Trial(poss);
			return 0;
		}
		tr = poss->values[0];
		list_free_NDS(nds_pass);
		list_free_Trial(poss);
	}
	int i;
	list(list(int)) *offsets = list_new_list__int();
	{
		list(int) *empty = list_new_int();
		list_add_list__int(offsets, *empty);
		free(empty);
	}
	for (i = 0; i < curve.calibration_columns->size; i++) {
		list(list(int)) *offsets_next = list_new_list__int();
		int col = curve.calibration_columns->values[i];
		list(int) *expected = &curve.calibration_signatures->values[i];
		list(Peak) *actual = tr.cols[col];
		if (actual->size < expected->size) {
			offsets->size = 0;
			break;
		}
		int off = 0;
		for (off = 0; off <= actual->size - expected->size; off++) {
			int works = 1;
			int peak;
			for (peak = 0; peak < expected->size; peak++) {
				if (actual->values[peak + off].is_positive_peak
						!= expected->values[peak]) {
					works = 0;
					break;
				}
			}
			if (!works)
				continue;
			{
				// determine if peak span is less than half of total span
				// If it is, the match is probably spurious.
				double peak_span = actual->values[off + expected->size].t
						- actual->values[off].t;
				double total_span = tr.data.data.values[tr.data.ind_end - 1].t
						- tr.data.data.values[tr.data.ind_start].t;
				if (peak_span * 2 < total_span)
					continue;
			}
			int k;
			for (k = 0; k < offsets->size; k++) {
				list(int) *ne = list_clone_int(&offsets->values[k]);
				free(offsets->values[k].values);
				list_add_int(ne, off);
				list_add_list__int(offsets_next, *ne);
				free(ne);
			}
			free(offsets->values);
			*offsets = *offsets_next;
			free(offsets_next);
		}
	}
	list(Trial) *possible_trials = list_new_Trial();
	if (offsets->size == 0)
		return 0;
	for (i = 0; i < offsets->size; i++) {
		// A sequence of valid offsets.
		int start[LAST_CALIBRATED_COLUMN + 1];
		int end[LAST_CALIBRATED_COLUMN + 1];
		{
			int j;
			for (j = 0; j <= LAST_CALIBRATED_COLUMN; j++) {
				start[j] = 0;
				end[j] = tr.cols[j]->size;
			}
			for (j = 0; j < curve.calibration_columns->size; j++) {
				int col = curve.calibration_columns->values[j];
				start[col] = offsets->values[i].values[j];
				end[col] = start[col]
						+ curve.calibration_signatures->values[j].size;
			}
		}
		Trial tr_new;
		tr_new.data = tr.data;
		int col;
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			tr_new.cols[col] = list_new_Peak();
			int peak;
			for (peak = start[col]; peak < end[col]; peak++) {
				list_add_Peak(tr_new.cols[col],
						tr.cols[col]->values[peak - start[col]]);
			}
		}
		analysis_scale_trial_by_peaks(&tr_new, curve.calibration_columns);
		Samples tr_samp = analysis_sample_points(tr_new, curve.n_samples);
		double p_val = statistics_composite_p(curve.distributions, tr_samp.cols);
		list_add_Trial(possible_trials, tr_new);
	}
	// TODO unstub
	return 0;
}

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
