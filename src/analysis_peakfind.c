#include "analysis_peakfind.h"

#include "Constants.h"
#include "DataSet.h"
#include "list.h"

/**
 * Finds peaks, in the given NDS and packages them into a trial.
 */
static Trial analysis_peak_find(NDS data);

list(Trial)* analysis_peak_find_all(list(NDS)* data, int to_be_removed) {
	list(Trial)* tl = list_new_Trial();
	int seg;
	for (seg = to_be_removed; seg < data->size - to_be_removed; seg++) {
		Trial trial = analysis_peak_find(data->values[seg]);
		list_add_Trial(tl, trial);
	}
	return tl;
}

static Trial analysis_peak_find(NDS data) {
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
