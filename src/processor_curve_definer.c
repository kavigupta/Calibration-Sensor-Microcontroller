/*
 * processor_afterthefact.c
 *
 *  Created on: Aug 18, 2015
 *      Author: root
 */

#include "processor_curve_definer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "analysis_conclusions.h"
#include "analysis_peakfind.h"
#include "analysis_preprocessing.h"
#include "analysis_segmentation.h"
#include "analysis_trial_separation.h"
#include "Constants.h"
#include "DataSet.h"
#include "IO.h"
#include "list.h"
#include "Utils.h"

typedef struct {
	list(Trial)* data;
	list(void_ptr)* to_free_on_exit;
} CleanableTrialList;

/**
 * Performs some processing on the data contained in the given folder:
 *
 * 1. smoothing
 * 2. normalization
 * 3. the separation of trials
 * 4. the removal of early and late trials
 *
 * and returns the list of trials along with a list of pointers to be
 * freed upon the freeing of the provided list of trials.
 */
static CleanableTrialList read_single_dataset(char* dir, char* c_readable);
/**
 * Reads all datasets from the given folder by calling read_single_dataset
 * repeatedly.
 */
static CleanableTrialList read_all_datasets(char* dir);

CleanableTrialList read_single_dataset(char* dir, char* c_readable) {
	list(JoinedData) joined = io_read_joined_dataset(c_readable);
	CalibratedDataList data = analysis_calibrate(joined);
	free(joined.values);
	analysis_smooth(&data);
	analysis_normalize(&data);
	list(NDS)* split = analysis_split_data(&data, JUMP_CONSTANT,
	MINIMUM_TRIAL_DURATION);
	list(Trial)* trials = analysis_peak_find_all(split,
	TRIALS_REMOVED_FROM_EITHER_END);
	char* normed = utils_concat(dir, "/normed.csv");
	io_write_calibrated_data(normed, data);
	free(normed);
	list_free_NDS(split);
	list(void_ptr)* to_free = list_new_void_ptr();
	list_add_void_ptr(to_free, data.values);
	CleanableTrialList ctl = { .data = trials, .to_free_on_exit = to_free };
	return ctl;
}

CleanableTrialList read_all_datasets(char* dir) {
	char* c_readable = utils_concat(dir, "/C-readable.csv");
	if (access(c_readable, F_OK) != -1) {
		CleanableTrialList ctl = read_single_dataset(dir, c_readable);
		free(c_readable);
		return ctl;
	}
	free(c_readable);
	CleanableTrialList general = { .data = list_new_Trial(), .to_free_on_exit =
			list_new_void_ptr() };
	void read_dir(char* lcl_dir) {
		char* cread = utils_concat(lcl_dir, "/C-readable.csv");
		if (access(cread, F_OK) == -1) {
			free(cread);
			return;
		}
		CleanableTrialList current = read_single_dataset(lcl_dir, cread);
		free(cread);
		int i;
		for (i = 0; i < current.data->size; i++) {
			list_add_Trial(general.data, current.data->values[i]);
		}
		list_free_Trial(current.data);
		for (i = 0; i < current.to_free_on_exit->size; i++) {
			list_add_void_ptr(general.to_free_on_exit,
					current.to_free_on_exit->values[i]);
		}
		list_free_void_ptr(current.to_free_on_exit);
	}
	foreach_in_dir(dir, read_dir);
	return general;
}

void process_content_folder(char* dir) {
	printf("Attempting to Calculate Peaks for %s\n", dir);
	CleanableTrialList read = read_all_datasets(dir);
	list(Trial)* trials = read.data;
	char* psplit = utils_concat(dir, "/data_split.csv");
	char* output = utils_concat(dir, "/peaks.csv");
	io_write_normalized_data_segment_list(psplit, output, trials);
	free(psplit);
	free(output);
	PeakScalingParameters params = analysis_scale_by_peaks(trials,
	COLS_USED_FOR_PEAK_DET,
	REMOVE_NONSTANDARD_COLS);
	char* psplit_stretched = utils_concat(dir, "/data_stretched.csv");
	io_write_normalized_data_segment_list(psplit_stretched, NULL, trials);
	free(psplit_stretched);
	CurveDefinition def = analysis_generate_match(trials,
			params.consistent_cols, params.used_signatures,
			params.n_peaks * SAMPLES_PER_SEGMENT);
	char* conclusions = utils_concat(dir, "/conclusions.csv");
	io_write_curve_definition(conclusions, def);
	free(conclusions);
	list_free_int(def.calibration_columns);
	int i;
	for (i = 0; i < def.calibration_signatures->size; i++) {
		free(def.calibration_signatures->values[i].values);
	}
	list_free_list__int(def.calibration_signatures);
	printf("Written all files\n");
	for (i = 0; i < trials->size; i++) {
		dataset_free_trial(trials->values[i]);
	}
	list_free_Trial(trials);
	for (i = 0; i < read.to_free_on_exit->size; i++) {
		free(read.to_free_on_exit->values[i]);
	}
	list_free_void_ptr(read.to_free_on_exit);
}
