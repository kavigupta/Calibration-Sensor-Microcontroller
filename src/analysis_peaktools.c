/*
 * analysis_peaktools.c
 *
 *  Created on: Aug 16, 2015
 *      Author: root
 */

#include "analysis_peaktools.h"

#include "arraylist.h"
#include "DataSet.h"

int analysis_peaklists_same_pattern(list(Peak)* a, list(Peak)* b,
		int same_length, int offset) {
	if (same_length && a->size != b->size)
		return 0;
	if (same_length)
		offset = 0;
	else if (offset < 0 || offset + a->size > b->size)
		return 0;
	int i;
	for (i = 0; i < a->size; i++) {
		if (a->values[i].is_positive_peak
				!= b->values[i + offset].is_positive_peak)
			return 0;
	}
	return 1;
}

double analysis_scale_within_NDS(NDS *nds, double t) {
	double start = nds->data.values[nds->ind_start].t;
	double end = nds->data.values[nds->ind_end].t;
	return (t - start) / (end - start);
}

double analysis_unscale_within_NDS(NDS *nds, double scaled) {
	double start = nds->data.values[nds->ind_start].t;
	double end = nds->data.values[nds->ind_end].t;
	return scaled * (end - start) + start;
}

list(Peak)* analysis_apply_peaks(list(Peak)* original, NDS *nds,
		double (to_apply)(NDS *nds, double t)) {
	list(Peak) *scaled = list_new_Peak();
	int peak;
	for (peak = 0; peak < original->size; peak++) {
		Peak at_i = original->values[peak];
		at_i.t = to_apply(nds, at_i.t);
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
