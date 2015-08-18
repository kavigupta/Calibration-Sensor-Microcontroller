/*
 * Test.c
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include <assert.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "analysis_peakfind.h"
#include "analysis_preprocessing.h"
#include "analysis_segmentation.h"
#include "analysis_trial_separation.h"
#include "Controller.h"
#include "DataSet.h"
#include "IO.h"
#include "list.h"
#include "Matrix.h"
#include "Utils.h"
#include "Vector.h"

void printEquality(Vector a, Vector b) {
	vector_print(a);
	printf("=");
	vector_print(b);
	printf("\n");
}

void testMatrix() {
	Matrix test0 = { .v11 = 1, .v12 = 2, .v13 = 3, .v21 = 4, .v22 = 5, .v23 = 6,
			.v31 = 7, .v32 = 8, .v33 = 9 };
	Matrix test5 = { .v11 = 123.1, .v12 = 2, .v13 = 3, .v21 = 4, .v22 = 5,
			.v23 = 6, .v31 = 7, .v32 = 8, .v33 = 9 };
	Matrix test1 = { .v11 = 30, .v12 = 36, .v13 = 42, .v21 = 66, .v22 = 81,
			.v23 = 96, .v31 = 102, .v32 = 126, .v33 = 150 };
	Vector test2 = { .x = 23.5, .y = -23.4, .z = 2.3 };
	Vector test3 = { -40.8, -123.6, -206.4 };
	Vector test4 = { 5114.04, 4756.56, -3859.32 };
	assert(matrix_equals(IDENTITY, matrix_multiply(IDENTITY, IDENTITY)));
	assert(matrix_equals(test0, matrix_multiply(test0, IDENTITY)));
	assert(matrix_equals(test1, matrix_multiply(test0, test0)));
	assert(vector_equals(test2, matrix_apply(IDENTITY, test2)));
	matrix_print(matrix_inverse(test5));
	assert(matrix_equals(matrix_inverse(IDENTITY), IDENTITY));
	printEquality(matrix_apply(test1, test2), test3);
	assert(
			vector_equals(vector_cross(VECTOR_UNIT_X, VECTOR_UNIT_Y),
					VECTOR_UNIT_Z));
	printEquality(vector_cross(test2, test3), test4);
	printEquality(
			matrix_apply(matrix_rotation(VECTOR_UNIT_X, test2), VECTOR_UNIT_X),
			vector_unit(test2));
	printf("Tests Passed!\n");
}

void test45dgup() {
	list(JoinedData) calibration =
			io_read_joined_dataset(
					"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/calibration.csv");
	list(JoinedData) dgup45 =
			io_read_joined_dataset(
					"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/45dgup.csv");
	cntrl_calibrate(calibration);
	Vector calib = averageMag(calibration);
	Vector mag45 = averageMag(dgup45);
	vector_print(calib);
	vector_print(mag45);
	printf("\n");
	printf("%f\n",
			acos(vector_dot(vector_unit(calib), vector_unit(mag45))) * 180.
					/ 3.1416);
	Vector displacement = cntrl_get_orientation(cntrl_get_theta(mag45));
	vector_print(displacement);
	printf("\n%f\n", vector_mag(displacement));
}

void peaks(char* dir) {
	printf("Attempting to Calculate Peaks for %s\n", dir);
	char* c_readable = utils_concat(dir, "/C-readable.csv");
	list(JoinedData) joined = io_read_joined_dataset(c_readable);
	free(c_readable);
	CalibratedDataList data = analysis_calibrate(joined);
	analysis_smooth(&data);
	analysis_normalize(&data);
	list(NDS)* split = analysis_split_data(&data, 20, .4);
	list(Trial)* trials = analysis_peak_find_all(split);
	char* normed = utils_concat(dir, "/normed.csv");
	io_write_calibrated_data(normed, data);
	free(normed);
	char* psplit = utils_concat(dir, "/peak_split.csv");
	char* output = utils_concat(dir, "/output.csv");
	io_write_normalized_data_segment_list(psplit, output, trials);
	free(psplit);
	free(output);
	analysis_scale_by_peaks(trials, 2);
	char* psplit_stretched = utils_concat(dir, "/split_stretched.csv");
	char* output_stretched = utils_concat(dir, "/output_stretched.csv");
	io_write_normalized_data_segment_list(psplit_stretched, output_stretched,
			trials);
	free(psplit_stretched);
	free(output_stretched);
	printf("Written all files\n");
	int i;
	for (i = 0; i < trials->size; i++) {
		int j;
		for (j = 0; j <= LAST_CALIBRATED_COLUMN; j++) {
			list_free_Peak(trials->values[i].cols[j]);
		}
	}
	list_free_NDS(split);
	list_free_Trial(trials);
	free(data.values);
	free(joined.values);
}

void peaksInDir(char* dir_string) {
	char* suffix = "/C-readable.csv";
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(dir_string)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name[0] == '.')
				continue;
			char * file = malloc(
					strlen(dir_string) + strlen(ent->d_name) + strlen(suffix)
							+ 2);
			file[0] = '\0';
			strcat(file, dir_string);
			strcat(file, "/");
			strcat(file, ent->d_name);
			peaks(file);
		}
		closedir(dir);
	} else {
		perror("");
		exit(-1);
	}
}

int main() {
	list(JoinedData) calibration =
			io_read_joined_dataset(
					"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/calibration.csv");
	cntrl_calibrate(calibration);
	free(calibration.values);
//	char* dir_string =
//			"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/data";
//	peaksInDir(dir_string);
	char* path =
			"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/data/doorknob-good";
	char* pathcpy = malloc(strlen(path) + 1);
	strcpy(pathcpy, path);
	peaks(pathcpy);
	free(pathcpy);
	printf("Completed Successfully");
	return 0;
}

