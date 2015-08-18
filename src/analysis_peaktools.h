/*
 * analysis_peaktools.h
 *
 *  Created on: Aug 16, 2015
 *      Author: root
 */

#ifndef ANALYSIS_PEAKTOOLS_H_
#define ANALYSIS_PEAKTOOLS_H_

#include "arraylist.h"
#include "DataSet.h"

/**
 * Checks whether the given PeakLists have the same pattern (up/down) or not.
 *
 * If same_length is true, then the lengths must be the same and the offset
 * parameter is ignored.
 *
 * The offset parameter is used if same_length is false. This function will
 * then check whether the the peak positivity/negativity first[i]
 * == second[i + offset] for i in [0, first.size]
 */
int analysis_peaklists_same_pattern(list(Peak)* a, list(Peak)* b,
		int same_length, int offset);
/**
 * Scales the given time index from an absolute time to a proportion between
 * the beginning and ending time.
 */
double analysis_scale_within_NDS(NDS *nds, double t);
/**
 * Scales the given time index from a proportion between the beginning and
 * ending time to an absolute time.
 */
double analysis_unscale_within_NDS(NDS *nds, double scaled);
/**
 * Applies the given function, which takes an NDS pointer and a type of time
 * index and returns another type of time index to every time in a list of
 * peaks, returning
 */
list(Peak)* analysis_apply_peaks(list(Peak)* original, NDS *nds,
		double (to_apply)(NDS *nds, double scaled));
/**
 * Adds the second peaklist into the first as a Vector, where each of the
 * second's peaks' values and times are added into the first's.
 */
void analysis_add_peaklists(list(Peak)* addInto, list(Peak)* toAdd);

#endif /* ANALYSIS_PEAKTOOLS_H_ */
