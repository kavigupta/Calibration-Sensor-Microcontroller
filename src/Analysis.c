/*
 * Analysis.c
 *
 *  Created on: Jul 7, 2015
 *      Author: root
 */

#include "Analysis.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "analysis_peaktools.h"
#include "arraylist.h"
#include "DataSet.h"
#include "Utils.h"

int analysis_peak_consistency(list(Trial)* data, int col, list(Peak)* chosen) {
	list(list(Peak))* signatures = list_new_list__Peak();
	list(int)* sigcounts = list_new_int();
	int i = 0;
	for (i = 0; i < data->size; i++) {
		list(Peak)* peaks = data->values[i].cols[col];
		if (peaks->size == 0)
			continue;
		int j;
		int contained = 0;
		for (j = 0; j < signatures->size; j++) {
			if (analysis_peaklists_same_pattern(&signatures->values[j], peaks,
					1, 0)) {
				// average out the time stamps and values with this one's
				list(Peak)* scaled = analysis_apply_peaks(peaks,
						&data->values[i].data, analysis_scale_within_NDS);
				analysis_add_peaklists(&signatures->values[j], scaled);
				list_free_Peak(scaled);
				sigcounts->values[j]++;
				contained = 1;
			}
		}
		if (!contained) {
			list(Peak)* scaled = analysis_apply_peaks(peaks,
					&data->values[i].data, analysis_scale_within_NDS);
			list_add_list__Peak(signatures, *scaled);
			free(scaled);
			list_add_int(sigcounts, 1);
		}
	}
	int consistency = 0;
	int consi = -1;
	list(Peak)* empty = list_new_Peak();
	*chosen = *empty;
	free(empty);
	for (i = 0; i < sigcounts->size; i++) {
		if (consistency < sigcounts->values[i]) {
			consi = i;
			list(Peak)* clone = list_clone_Peak(&signatures->values[i]);
			free(chosen->values);
			*chosen = *clone;
			free(clone);
			printf("Choosing size %d\n", chosen->size);
			consistency = sigcounts->values[i];
		}
	}
	if (consi != -1) {
		// divide chosen by it's sigcount (consistency) to get an average
		for (i = 0; i < chosen->size; i++) {
			chosen->values[i].t /= consistency;
			chosen->values[i].value /= consistency;
		}
		printf("Representative List for Column %d is (with %d adherents)\n",
				col, consistency);
		for (i = 0; i < signatures->values[consi].size; i++) {
			printf("\t%d\n",
					signatures->values[consi].values[i].is_positive_peak);
		}
	}
	for (i = 0; i < signatures->size; i++) {
		free(signatures->values[i].values);
	}
	list_free_list__Peak(signatures);
	list_free_int(sigcounts);
	return consistency;
}

void analysis_coerce_peaks_within_column(Trial* tr, int col,
list(Peak)* standard) {
	if (analysis_peaklists_same_pattern(standard, tr->cols[col], 1, 0))
		// no coersion necessary
		return;
	printf("Coercing peaks for data col %d\n", col);
	if (tr->cols[col]->size <= standard->size) {
		printf("Lower size\n");
		// not enough peaks. This will now use the average peak
		// signature and unscale it
		list_free_Peak(tr->cols[col]);
		tr->cols[col] = analysis_apply_peaks(standard, &tr->data,
				analysis_unscale_within_NDS);
		return;
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
		// no applicable peak signature (highly unlikely). This will now use
		// the average peak signature and unscale it
		list_free_Peak(tr->cols[col]);
		tr->cols[col] = analysis_apply_peaks(standard, &tr->data,
				analysis_unscale_within_NDS);
	} else {
		// get a signature without the offending peaks.
		list(Peak) *clean = list_new_Peak();
		int i;
		for (i = minoffset; i < minoffset + standard->size; i++) {
			list_add_Peak(clean, standard->values[i]);
		}
		list_free_Peak(tr->cols[col]);
		tr->cols[col] = clean;
	}
}

void analysis_coerce_peaks(list(Trial)* data,
list(Peak) chosen[LAST_CALIBRATED_COLUMN + 1],
list(int) *top) {
	int i;
	for (i = 0; i < top->size; i++) {
		int col = top->values[i];
		list(Peak) *used = &chosen[col];
		int j;
		for (j = 0; j < data->size; j++) {
			// TODO 2
			analysis_coerce_peaks_within_column(&data->values[j], col, used);
		}
	}
}

/**
 * Finds columns which contain the most consistent peaks.
 */
list(int)* find_consistent_peak_columns(list(Trial)* data,
		int requested_quantity) {
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
	for (i = 0; i < requested_quantity; i++) {
		printf("The %dth most consistent column is %d, with %d repeats each\n",
				i, cols[i], consistencies[cols[i]]);
		list_add_int(top, cols[i]);
	}
	for (i = 0; i <= LAST_CALIBRATED_COLUMN; i++) {
		printf("Col %d: Size %d\n", i, data->values[0].cols[i]->size);
	}
	// TODO 1
	analysis_coerce_peaks(data, chosen, top);
	int j;
	for (j = 0; j < data->size; j++) {
		printf("%d columns\n", data->values[j].cols[cols[0]]->size);
	}
	for (j = 0; j <= LAST_CALIBRATED_COLUMN; j++) {
		free(chosen[j].values);
	}
	return top;
}

void analysis_scale_by_peaks(list(Trial)* lTrials, int ncols) {
	// check no changes to cols in lTrials TODO 0
	list(int) *consistent_peaks = find_consistent_peak_columns(lTrials,
			ncols);
	int nTrial;
	for (nTrial = 0; nTrial < lTrials->size; nTrial++) {
		list(double) *all_peak_times = list_new_double();
		Trial* tr = &(lTrials->values[nTrial]);
		NDS* data = &(tr->data);
		int i;
		for (i = 0; i < consistent_peaks->size; i++) {
			list(Peak) *current_peaks =
					(tr->cols)[consistent_peaks->values[i]];
			printf("Current size = %d\n", current_peaks->size);
			int peak;
			for (peak = 0; peak < current_peaks->size; peak++) {
				list_add_double(all_peak_times, current_peaks->values[peak].t);
			}
		}
		list_add_double(all_peak_times, data->data.values[data->ind_start].t);
		list_add_double(all_peak_times, data->data.values[data->ind_end - 1].t);
		qsort(all_peak_times->values, all_peak_times->size, sizeof(double),
				cmp_double);
		int seg = 0;
		double segdt = all_peak_times->values[seg + 1]
				- all_peak_times->values[seg];
		for (i = data->ind_start; i < data->ind_end; i++) {
			CalibratedData *row = &data->data.values[i];
			if (seg + 1 < all_peak_times->size
					&& row->t >= all_peak_times->values[seg + 1]) {
				while (all_peak_times->values[seg + 1]
						== all_peak_times->values[seg])
					seg++;
				segdt = all_peak_times->values[seg + 1]
						- all_peak_times->values[seg];
			}
			row->t = ((row->t - all_peak_times->values[seg]) / segdt
					+ (double) seg) / (double) all_peak_times->size;
			if (isinf(row->t)) {
				printf(
						"IS inf row->t; all_peak_times = %d; seg = %d; segdt = %f\n",
						all_peak_times->size, seg, segdt);
			}
		}
		list_free_double(all_peak_times);
	}
	list_free_int(consistent_peaks);
}
