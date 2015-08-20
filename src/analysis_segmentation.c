/*
 * Analysis.c
 *
 *  Created on: Jul 7, 2015
 *      Author: root
 */

#include "analysis_segmentation.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "analysis_peaktools.h"
#include "analysis_preprocessing.h"
#include "DataSet.h"
#include "list.h"
#include "Utils.h"

static void analysis_find_unique_signatures(list(Trial)* data, int col,
list(list(Peak))** signatures, list(int)** sigcounts) {
	*signatures = list_new_list__Peak();
	*sigcounts = list_new_int();
	int i = 0;
	for (i = 0; i < data->size; i++) {
		list(Peak)* peaks = data->values[i].cols[col];
		if (peaks->size == 0)
			continue;
		int j;
		int contained = 0;
		for (j = 0; j < (*signatures)->size; j++) {
			if (analysis_peaklists_same_pattern(&(*signatures)->values[j],
					peaks, 1, 0)) {
				// average out the time stamps and values with this one's
				list(Peak)* scaled = analysis_apply_peaks(peaks,
						&data->values[i].data, analysis_scale_within_NDS);
				analysis_add_peaklists(&(*signatures)->values[j], scaled);
				list_free_Peak(scaled);
				(*sigcounts)->values[j]++;
				contained = 1;
			}
		}
		if (!contained) {
			list(Peak)* scaled = analysis_apply_peaks(peaks,
					&data->values[i].data, analysis_scale_within_NDS);
			list_add_list__Peak(*signatures, *scaled);
			free(scaled);
			list_add_int(*sigcounts, 1);
		}
	}
	// divide chosen by it's sigcount (consistency) to get an average
	for (i = 0; i < (*signatures)->size; i++) {
		int sigcount = (*sigcounts)->values[i];
		int j;
		for (j = 0; j < (*signatures)->values[i].size; j++) {
			(*signatures)->values[i].values[j].t /= sigcount;
			(*signatures)->values[i].values[j].value /= sigcount;
		}
	}
}

static int analysis_peak_consistency(list(Trial)* data, int col,
list(Peak)* chosen) {
	list(list(Peak))* signatures;
	list(int)* sigcounts;
	analysis_find_unique_signatures(data, col, &signatures, &sigcounts);
	int consistency = 0;
	int most_consistent_trial = -1;
	int i;
	for (i = 0; i < sigcounts->size; i++) {
		if (consistency < sigcounts->values[i]) {
			consistency = sigcounts->values[i];
			most_consistent_trial = i;
		}
	}
	if (most_consistent_trial >= 0) {
		list(Peak)* clone = list_clone_Peak(
				&signatures->values[most_consistent_trial]);
		*chosen = *clone;
		free(clone);
	} else {
		list(Peak)* empty = list_new_Peak();
		*chosen = *empty;
		free(empty);
	}
	for (i = 0; i < signatures->size; i++) {
		free(signatures->values[i].values);
	}
	list_free_list__Peak(signatures);
	list_free_int(sigcounts);
	return consistency;
}

int analysis_coerce_peaks_for_single_column(Trial* tr, int col,
list(Peak)* standard, int reject_nonstandardly_patterned_peaks) {
	if (analysis_peaklists_same_pattern(standard, tr->cols[col], 1, 0)) {
		// no coersion necessary
		return 1;
	}
	if (tr->cols[col]->size <= standard->size) {
		printf("Lower size\n");
		if (reject_nonstandardly_patterned_peaks)
			return 0;
		// not enough peaks. This will now use the average peak
		// signature and unscale it
		list_free_Peak(tr->cols[col]);
		tr->cols[col] = analysis_apply_peaks(standard, &tr->data,
				analysis_unscale_within_NDS);
		return 1;
	}
	// actual coersion takes place here.
	// This will choose between different options with the
	// same signature and find the one with the closest peak
	// pattern
	int offset;
	double mindiff = DBL_MAX;
	int minoffset = -1;
	for (offset = 1; offset < tr->cols[col]->size - standard->size; offset++) {
//		printf("Offset = %d/%d\n", offset, actual->size - standard->size);
		int peakct;
		int matches_up_down_pattern = 1;
		double diff = 0;
		for (peakct = 0; peakct < standard->size; peakct++) {
			Peak* peak_actual = &tr->cols[col]->values[peakct];
			Peak* peak_standrd = &tr->cols[col]->values[peakct];
			if (peak_actual->is_positive_peak
					!= peak_standrd->is_positive_peak) {
				matches_up_down_pattern = 0;
				break;
			}
			//Calculate absolute logarithmic difference from average.
			diff += abs(log(peak_actual->t / peak_standrd->t))
					+ abs(log(peak_actual->value / peak_standrd->value));
		}
		if (!matches_up_down_pattern)
			continue;
		// up-down pattern matched.
		if (diff > mindiff)
			//better match found already
			continue;
		mindiff = diff;
		minoffset = offset;
	}
	if (minoffset < 0) {
		// no applicable peak signature (highly unlikely).
		if (reject_nonstandardly_patterned_peaks)
			return 0;
		// This will now use the average peak signature and unscale it
		list_free_Peak(tr->cols[col]);
		tr->cols[col] = analysis_apply_peaks(standard, &tr->data,
				analysis_unscale_within_NDS);
	} else {
		printf("Cleaning signature\n");
		// get a signature without the offending peaks.
		list(Peak) *clean = list_new_Peak();
		int i;
		for (i = minoffset; i < minoffset + standard->size; i++) {
			list_add_Peak(clean, tr->cols[col]->values[i]);
		}
		list_free_Peak(tr->cols[col]);
		tr->cols[col] = clean;
	}
	return 1;
}

static void analysis_coerce_peaks(list(Trial)* data,
list(Peak) chosen[LAST_CALIBRATED_COLUMN + 1],
list(int) *top, int reject_nonstandardly_patterned_peaks) {
	int i;
	for (i = 0; i < top->size; i++) {
		list(Trial) *appl = list_new_Trial();
		int col = top->values[i];
		list(Peak) *used = &chosen[col];
		int j;
		for (j = 0; j < data->size; j++) {
			int valid = analysis_coerce_peaks_for_single_column(
					&data->values[j], col, used,
					reject_nonstandardly_patterned_peaks);
			if (!valid) {
				dataset_free_trial(data->values[j]);
			} else {
				list_add_Trial(appl, data->values[j]);
			}
		}
		free(data->values);
		*data = *appl;
		free(appl);
	}
}

/**
 * Finds columns which contain the most consistent peaks.
 */
static PeakScalingParameters find_consistent_peak_columns(list(Trial)* data,
		int requested_quantity, int reject_nonstandardly_patterned_peaks) {
	int cols[LAST_CALIBRATED_COLUMN + 1];
	int consistencies[LAST_CALIBRATED_COLUMN + 1];
	int i;
	list(Peak) chosen[LAST_CALIBRATED_COLUMN + 1];
	for (i = 0; i <= LAST_CALIBRATED_COLUMN; i++) {
		cols[i] = i;
		consistencies[i] = analysis_peak_consistency(data, i, &chosen[i]);
	}
	int compare_cols_by_consistency(const void* _a, const void* _b) {
		const int* a = _a;
		const int* b = _b;
		if (consistencies[*a] > consistencies[*b])
			return -1;
		if (consistencies[*a] < consistencies[*b])
			return 1;
		return 0;
	}
	qsort(cols, LAST_CALIBRATED_COLUMN + 1, sizeof(int),
			compare_cols_by_consistency);
	list(int)* top = list_new_int();
	list(list(int))* sigs = list_new_list__int();
	int n_peaks = 0;
	for (i = 0; i < requested_quantity; i++) {
		list_add_int(top, cols[i]);
		list(int) *sig = list_new_int();
		int j;
		for (j = 0; j < chosen[cols[i]].size; j++) {
			list_add_int(sig, chosen[cols[i]].values[j].is_positive_peak);
			n_peaks++;
		}
		list_add_list__int(sigs, *sig);
		free(sig);
	}
	for (i = 0; i < top->size; i++) {
		printf("The %dth most consistent column is %d\n", i + 1,
				top->values[i]);
	}
	analysis_coerce_peaks(data, chosen, top,
			reject_nonstandardly_patterned_peaks);
	int j;
	for (j = 0; j <= LAST_CALIBRATED_COLUMN; j++) {
		free(chosen[j].values);
	}
	PeakScalingParameters psp = { .consistent_cols = top, .used_signatures =
			sigs, .n_peaks = n_peaks };
	return psp;
}

static list(double)* find_all_peak_times(list(int) *consistent_peaks, Trial* tr,
		NDS* data) {
	list(double) *all_peak_times = list_new_double();
	int i;
	for (i = 0; i < consistent_peaks->size; i++) {
		list(Peak) *current_peaks = (tr->cols)[consistent_peaks->values[i]];
		int peak;
		for (peak = 0; peak < current_peaks->size; peak++) {
			list_add_double(all_peak_times, current_peaks->values[peak].t);
		}
	}
	list_add_double(all_peak_times, data->data.values[data->ind_start].t);
	list_add_double(all_peak_times, data->data.values[data->ind_end - 1].t);
	qsort(all_peak_times->values, all_peak_times->size, sizeof(double),
			cmp_double);
	list(double) *all_filtered_peak_times = list_new_double();
	for (i = 0; i < all_peak_times->size; i++) {
		if (i != all_peak_times->size - 1
				&& all_peak_times->values[i] == all_peak_times->values[i + 1])
			continue;
		list_add_double(all_filtered_peak_times, all_peak_times->values[i]);
	}
	list_free_double(all_peak_times);
	return all_filtered_peak_times;

}

void analysis_scale_trial_by_peaks(Trial* tr, list(int)* consistent_cols) {
	NDS* data = &(tr->data);
	list(double) *all_peak_times = find_all_peak_times(consistent_cols, tr,
			data);
	double calc_segdt(int seg) {
		return all_peak_times->values[seg + 1] - all_peak_times->values[seg];
	}
	void segdt_loop(double *(loc_of_t)(int i), int num_i) {
		int seg = 0;
		double segdt = calc_segdt(seg);
		int i;
		for (i = 0; i < num_i; i++) {
			double* t = loc_of_t(i);
			while (seg + 1 < all_peak_times->size
					&& *t >= all_peak_times->values[seg + 1]) {
				seg++;
				segdt = calc_segdt(seg);
			}
			*t = ((*t - all_peak_times->values[seg]) / segdt + (double) seg)
					/ (double) (all_peak_times->size - 1);
		}
	}
	double *loc_of_t_in_NDS(int i) {
		return &data->data.values[i + data->ind_start].t;
	}
	segdt_loop(loc_of_t_in_NDS, data->ind_end - data->ind_start);
	int col;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		double *loc_of_t_in_Peak(int i) {
			return &tr->cols[col]->values[i].t;
		}
		segdt_loop(loc_of_t_in_Peak, tr->cols[col]->size);
	}
	list_free_double(all_peak_times);
	CalibratedDataList ref = dataset_nds_to_cdl(tr->data);
	ref.is_normalized = 0;
	analysis_normalize(&ref);
}

PeakScalingParameters analysis_scale_by_peaks(list(Trial)* lTrials, int ncols,
		int reject_nonstandardly_patterned_peaks) {
	PeakScalingParameters params = find_consistent_peak_columns(lTrials, ncols,
			reject_nonstandardly_patterned_peaks);
	int nTrial;
	for (nTrial = 0; nTrial < lTrials->size; nTrial++) {
		Trial* tr = &(lTrials->values[nTrial]);
		analysis_scale_trial_by_peaks(tr, params.consistent_cols);
	}
	return params;
}
