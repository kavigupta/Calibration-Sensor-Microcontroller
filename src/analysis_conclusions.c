/*
 * analysis_conclusions.c
 *
 *  Created on: Aug 18, 2015
 *      Author: root
 */

#include "analysis_conclusions.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "DataSet.h"
#include "generics.h"
#include "list.h"

typedef struct {
	int n_samples;
	double *cols[LAST_CALIBRATED_COLUMN + 1];
} Samples;
import_header(list, Samples);
import_body(list, Samples);

/**
 * Samples the given curve by taking n_samples points at even "t"
 * intervals
 */
static Samples analysis_sample_points(Trial trial, int n_samples);

CurveDefinition analysis_generate_match(list(Trial)* trials,
list(int)* consistent_cols,
list(list(int))* signatures, int n_samples) {
	CurveDefinition match;
	match.n_samples = n_samples;
	match.calibration_columns = consistent_cols;
	match.calibration_signatures = signatures;
	int col;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		match.distributions[col] = list_new_Distribution();
	}
	list(Samples) *all_samples = list_new_Samples();
	int nTrial;
	for (nTrial = 0; nTrial < trials->size; nTrial++) {
		Samples samp = analysis_sample_points(trials->values[nTrial],
				n_samples);
		list_add_Samples(all_samples, samp);
	}
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		int point;
		for (point = 0; point < n_samples; point++) {
			double sum = 0;
			for (nTrial = 0; nTrial < all_samples->size; nTrial++) {
				if (col == 0)
					printf("%f\t",
							all_samples->values[nTrial].cols[col][point]);
				sum += all_samples->values[nTrial].cols[col][point];
			}
			if (col == 0)
				printf("\n");
			double mu = sum / n_samples;
			double var = 0;
			for (nTrial = 0; nTrial < all_samples->size; nTrial++) {
				all_samples->values[nTrial].cols[col][point] -= mu;
				var += all_samples->values[nTrial].cols[col][point]
						* all_samples->values[nTrial].cols[col][point];
			}
			Distribution dist =
					{ .mu = mu, .sigma = sqrt(var / (n_samples - 1)) };
			list_add_Distribution(match.distributions[col], dist);
		}
	}
	int i;
	for (i = 0; i < all_samples->size; i++) {
		int col;
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			free(all_samples->values[i].cols[col]);
		}
	}
	list_free_Samples(all_samples);
	return match;
}

static Samples analysis_sample_points(Trial trial, int n_samples) {
	double *samples[LAST_CALIBRATED_COLUMN + 1];
	int col;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		samples[col] = calloc(n_samples, sizeof(double));
		CalibratedData* values = trial.data.data.values;
		int cur_sample = 0;
		int i;
		// loop to replace the time indeces with values
		for (i = trial.data.ind_start; i < trial.data.ind_end; i++) {
			int br = 0;
			while (values[i].t * n_samples <= cur_sample) {
				i++;
				if (i >= trial.data.ind_end) {
					br = 1;
					break;
				}
			}
			if (br)
				break;
			double t1 = values[i - 1].t, t2 = values[i].t;
			double v1 = *dataset_column_get_field(&values[i - 1], col);
			double v2 = *dataset_column_get_field(&values[i], col);
			double m = (v2 - v1) / (t2 - t1);
			// (v - v1) = m (t - t1)
			double t = (double) cur_sample / (double) n_samples;
			assert(cur_sample < n_samples);
			samples[col][cur_sample] = v1 + m * (t - t1);
			cur_sample++;
		}
	}
	Samples smp;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		smp.cols[col] = samples[col];
	}
	smp.n_samples = n_samples;
	return smp;
}
