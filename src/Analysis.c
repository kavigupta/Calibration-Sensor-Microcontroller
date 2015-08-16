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

#include "arraylist.h"
#include "Constants.h"
#include "DataSet.h"
#include "Utils.h"

Trial analysis_peak_find(NDS data) {
	//	printf("Data smoothed: %d lines\n", data->len);
	int column;
	list(Peak) *ps[9];
	for (column = 0; column <= LAST_CALIBRATED_COLUMN; column++) {
		ps[column] = list_new_Peak();
		//		printf("Column %d\n", column);
		double valueI(int i) {
			return *dataset_column_get_field(&(data.data.values[i]), column);
		}
		int plusminus = smoothing_radius_index(&data.data);
		//		printf("PLUS MINUS INTERVAL %d\n", plusminus);
		int i;
		for (i = data.ind_start + plusminus; i < data.ind_end - plusminus;
				i++) {
			int bottomOfInterval = i - plusminus;
			int topOfInterval = i + plusminus;
			int j;
			double val = valueI(i);
			int canBeMax = 1, canBeMin = 1;
			for (j = bottomOfInterval;
					j <= topOfInterval && (canBeMin || canBeMax); j++) {
				if (j == i)
					continue;
				double curVal = valueI(j);
				if (val <= curVal)
					canBeMax = 0;
				if (val >= curVal)
					canBeMin = 0;
			}
			if (canBeMax || canBeMin) {
				Peak current = { .is_positive_peak = canBeMax, .t =
						data.data.values[i].t, .value = val };
				list_add_Peak(ps[column], current);
			}
		}
	}
	Trial t = { .data = data };
	int i;
	for (i = 0; i < 9; i++)
		t.cols[i] = ps[i];
	return t;
}

list(Trial)* analysis_peak_find_all(list(NDS)* data) {
	list(Trial)* tl = list_new_Trial();
	int seg;
	for (seg = 0; seg < data->size; seg++) {
		Trial trial = analysis_peak_find(data->values[seg]);
		list_add_Trial(tl, trial);
	}
	return tl;
}

int analysis_peaklists_same_pattern(list(Peak)* a, list(Peak)* b) {
	if (a->size != b->size)
		return 0;
	int i;
	for (i = 0; i < a->size; i++) {
		if (a->values[i].is_positive_peak != b->values[i].is_positive_peak)
			return 0;
	}
	return 1;
}

double analysis_scale_within_NDS(NDS nds, double t) {
	double start = nds.data.values[nds.ind_start].t;
	double end = nds.data.values[nds.ind_end].t;
	return (t - start) / (end - start);
}

double analysis_unscale_within_NDS(NDS nds, double scaled) {
	double start = nds.data.values[nds.ind_start].t;
	double end = nds.data.values[nds.ind_end].t;
	return scaled * (end - start) + start;
}

list(Peak)* analysis_scale_peaks(list(Peak)* original, NDS nds) {
	list(Peak) *scaled = list_new_Peak();
	// offsets this peak so that it's time index is
	// relative to the total duration of the action
	int peak;
	for (peak = 0; peak < original->size; peak++) {
		Peak at_i = original->values[peak];
		at_i.t = analysis_scale_within_NDS(nds, at_i.t);
		list_add_Peak(scaled, at_i);
	}
	return scaled;
}
list(Peak)* analysis_unscale_peaks(list(Peak)* original, NDS nds) {
	list(Peak) *scaled = list_new_Peak();
	// offsets this peak so that it's time index is
	// relative to the total duration of the action
	int peak;
	for (peak = 0; peak < original->size; peak++) {
		Peak at_i = original->values[peak];
		at_i.t = analysis_unscale_within_NDS(nds, at_i.t);
		list_add_Peak(scaled, at_i);
	}
	return scaled;
}

void analysis_add_peaklists(list(Peak)* addInto, list(Peak)* toAdd) {
	int i;
	for (i = 0; i < addInto->size; i++) {
		addInto->values[i].t += toAdd->values[i].t;
		addInto->values[i].value += toAdd->values[i].value;
	}
}

int analysis_peak_consistency(list(Trial)* data, int col, list(Peak)* chosen) {
	list(list(Peak))* signatures = list_new_list__Peak();
	list(int)* sigcounts = list_new_int();
	int i = 0;
	for (i = 0; i < data->size; i++) {
		list(Peak)* peaks = data->values[i].cols[col];
		if (peaks->size == 0) {
			continue;
		}
		int j;
		int contained = 0;
		for (j = 0; j < signatures->size; j++) {
			if (analysis_peaklists_same_pattern(&signatures->values[j],
					peaks)) {
				// average out the time stamps and values with this one's
				analysis_add_peaklists(&signatures->values[j],
						analysis_scale_peaks(peaks, data->values[i].data));
				sigcounts->values[j]++;
				contained = 1;
			}
		}
		if (!contained) {
//			printf("LB%d\n", signatures->size);
			list_add_list__Peak(signatures,
					*analysis_scale_peaks(peaks, data->values[i].data));
//			printf("LA%d\n", signatures->size);
			list_add_int(sigcounts, 1);
		}
	}
	int consistency = 0;
	int consi = -1;
	*chosen = *list_new_Peak();
	for (i = 0; i < sigcounts->size; i++) {
		if (consistency < sigcounts->values[i]) {
			consi = i;
			*chosen = signatures->values[i];
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
	list_free_list__Peak(signatures);
	list_free_int(sigcounts);
	return consistency;
}

void analysis_coerce_peaks_within_column(Trial* tr, int col,
list(Peak)* standard) {
	list(Peak) **actual = &tr->cols[col];
	if (analysis_peaklists_same_pattern(standard, *actual))
		// no coersion necessary
		return;
	printf("Coercing peaks for data col %d\n", col);
	if ((*actual)->size <= standard->size) {
		printf("Lower size\n");
		// not enough peaks. This will now use the average peak
		// signature and unscale it
		**actual = *analysis_unscale_peaks(standard, tr->data);
		return;
	}
	// actual coersion takes place here.
	// This will choose between different options with the
	// same signature and find the one with the closest peak
	// pattern
	int offset;
	double mindiff = DBL_MAX;
	int minoffset = -1;
	for (offset = 1; offset < (*actual)->size - standard->size; offset++) {
//		printf("Offset = %d/%d\n", offset, actual->size - standard->size);
		int peakct;
		int matches_up_down_pattern = 1;
		double diff = 0;
		for (peakct = 0; peakct < standard->size; peakct++) {
			Peak* peak_actual = &(*actual)->values[peakct];
			Peak* peak_standrd = &(*actual)->values[peakct];
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
		**actual = *analysis_unscale_peaks(standard, tr->data);
	} else {
		// get a signature without the offending peaks.
		list(Peak) *clean = list_new_Peak();
		int i;
		for (i = minoffset; i < minoffset + standard->size; i++) {
			list_add_Peak(clean, standard->values[i]);
		}
		free((*actual)->values);
		**actual = *clean;
		free(clean);
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
	analysis_coerce_peaks(data, chosen, top);
	int j;
	for (j = 0; j < data->size; j++) {
		printf("%d columns\n", data->values[j].cols[cols[0]]->size);
	}
	return top;
}

void analysis_scale_by_peaks(list(Trial)* lTrials, int ncols) {
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
			if (row->t >= all_peak_times->values[seg + 1]) {
				seg++;
				segdt = all_peak_times->values[seg + 1]
						- all_peak_times->values[seg];
			}
			row->t = ((row->t - all_peak_times->values[seg]) / segdt
					+ (double) seg) / (double) all_peak_times->size;
		}
		list_free_double(all_peak_times);
	}
	list_free_int(consistent_peaks);
}
