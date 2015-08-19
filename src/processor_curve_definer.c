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

#include "analysis.h"
#include "DataSet.h"
#include "IO.h"
#include "list.h"
#include "Utils.h"

typedef struct {
	list(list(JoinedData))* data;
	list(void_ptr)* to_free_on_exit;
} CleanableJDList;

/**
 * Reads all datasets from the given folder.
 */
static list(list(JoinedData))* read_all_datasets(char* dir);

static list(list(JoinedData))* read_all_datasets(char* dir) {
	char* c_readable = utils_concat(dir, "/C-readable.csv");
	if (access(c_readable, F_OK) != -1) {
		list(JoinedData) ctl = io_read_joined_dataset(c_readable);
		free(c_readable);
		list(list(JoinedData)) *list = list_new_list__JoinedData();
		list_add_list__JoinedData(list, ctl);
		return list;
	}
	free(c_readable);
	list(list(JoinedData))* data = list_new_list__JoinedData();
	void read_dir(char* lcl_dir) {
		char* cread = utils_concat(lcl_dir, "/C-readable.csv");
		if (access(cread, F_OK) == -1) {
			free(cread);
			return;
		}
		list(JoinedData) ljd = io_read_joined_dataset(cread);
		free(cread);
		list_add_list__JoinedData(data, ljd);
	}
	foreach_in_dir(dir, read_dir);
	return data;
}

void process_content_folder(char* dir, CurveDefinitionParameters params) {
	printf("Attempting to Calculate Peaks for %s\n", dir);
	list(list(JoinedData)) *read = read_all_datasets(dir);
	CurveDefinitionTrace trace = analysis_define_curve(*read, params);
	int i;
	for (i = 0; i < read->size; i++) {
		free(read->values[i].values);
	}
	list_free_list__JoinedData(read);
	char* psplit = utils_concat(dir, "/data_stretched.csv");
	io_write_normalized_data_segment_list(psplit, NULL, trace.trials);
	free(psplit);
	char* conclusions = utils_concat(dir, "/conclusions.csv");
	io_write_curve_definition(conclusions, trace.def);
	free(conclusions);
	for (i = 0; i < trace.to_free->size; i++) {
		free(trace.to_free->values[i]);
	}
	list_free_void_ptr(trace.to_free);
	printf("Written all files\n");
}
