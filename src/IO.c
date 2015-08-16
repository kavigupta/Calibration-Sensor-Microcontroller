/*
 PeakSet* ps = dataset_peakset_new();
 * IO.c
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include "IO.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "arraylist.h"
#include "DataSet.h"

list(JoinedData) io_read_joined_dataset(char* path) {
	FILE* file = fopen(path, "r");
	if (file == NULL) {
//		printf("File does not exist! %s", path);
		exit(1);
	}
	int len = 0;
	JoinedData *data = malloc(10 * sizeof(JoinedData));
	int capacity = 10;
	char buffer[1000];
	fgets(buffer, 1000, file);
	printf("Initial Contents: %s\n", buffer);
	while (1) {
		JoinedData line = io_read_line_as_Double10(file).joined;
		if (line.t < 0)
			break;
		data[len] = line;
		len++;
		if (len == capacity) {
			capacity = capacity * 3 / 2;
			data = realloc(data, capacity * sizeof(JoinedData));
		}
	}
	printf("File %s read! Length = %d\n", path, len);
	list(JoinedData) ds = { .values = data, .size = len };
	return ds;
}
Double10 io_read_line_as_Double10(FILE* file) {
	Double10 line = { };
	char buffer[2000];
	if (NULL != fgets(buffer, 2000, file)) {
		int code = sscanf(buffer, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
				line.data, line.data + 1, line.data + 2, line.data + 3,
				line.data + 4, line.data + 5, line.data + 6, line.data + 7,
				line.data + 8, line.data + 9);
		if (code == 10) {
			//	printf("At least one data point! %f\n", t);
			return line;
		}
	}
	// assign error code in "time" slot
	line.data[0] = -1;
	//		printf("NOT ENOUGH??? %d\n", code);
	return line;
}
void io_write_line_as_Double10(FILE* file, Double10 line) {
	fprintf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", line.data[0],
			line.data[1], line.data[2], line.data[3], line.data[4],
			line.data[5], line.data[6], line.data[7], line.data[8],
			line.data[9]);
}
void io_write_joined_data(char* path, list(JoinedData) data) {
	int i;
	FILE* file = fopen(path, "w");
	fprintf(file, "Time,aclx,acly,aclz,gyrx,gyry,gyrz,magx,magy,magz");
	for (i = 0; i < data.size; i++) {
		Double10 line = { .joined = data.values[i] };
		io_write_line_as_Double10(file, line);
	}
}
void io_write_calibrated_data(char* path, CalibratedDataList data) {
	printf("Writing to file: %s\n", path);
	int i;
	FILE* file = fopen(path, "w");
	fprintf(file, "Time,aclx,acly,aclz,gx,gy,gz,mr,mtheta,mphi");
	for (i = 0; i < data.len; i++) {
		Double10 line = { .calibrated = data.values[i] };
		io_write_line_as_Double10(file, line);
	}
	fclose(file);
}

void io_write_normalized_data_segment_list(char* path_data, char* path_peaks,
list(Trial)* listTrials) {
	FILE* file_data = fopen(path_data, "w");
	FILE* file_peaks = fopen(path_peaks, "w");
	fprintf(file_data, "%s,", "Relative Time (to start of segment)");
	fprintf(file_peaks, "%s,", "Relative Time (to start of segment)");
	int col;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		int seg;
		for (seg = 0; seg < listTrials->size; seg++) {
			fprintf(file_data, "%s %d,", dataset_column_render(col), seg + 1);
			fprintf(file_peaks, "%s %d - Value,%s %d - Is positive Peak,",
					dataset_column_render(col), seg + 1,
					dataset_column_render(col), seg + 1);
		}
	}
	fprintf(file_data, "\n");
	fprintf(file_peaks, "\n");
	int seg;
	for (seg = 0; seg < listTrials->size; seg++) {
		Trial currentTrial = listTrials->values[seg];
		NDS nds = currentTrial.data;
		int row;
		double initialT = nds.data.values[nds.ind_start].t;
		double finalT = nds.data.values[nds.ind_end - 1].t;
		double totalIntT = finalT - initialT;
		for (row = nds.ind_start; row < nds.ind_end; row++) {
			CalibratedData currentData = nds.data.values[row];
			if (isnan((currentData.t - initialT) / totalIntT)) {
				printf("Total int t == %f - %f\n", finalT, initialT);
			}
			fprintf(file_data, "%f,", (currentData.t - initialT) / totalIntT);
			for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
				int segSub;
				for (segSub = 0; segSub < listTrials->size; segSub++) {
					if (seg != segSub) {
						fprintf(file_data, ",");
					} else
						fprintf(file_data, "%f,",
								*dataset_column_get_field(&currentData, col));
				}
			}
			fprintf(file_data, "\n");
		}
		int col;
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			list(Peak)* curPeaks = currentTrial.cols[col];
			for (row = 0; row < curPeaks->size; row++) {
				Peak curPeak = curPeaks->values[row];
				fprintf(file_peaks, "%f,", (curPeak.t - initialT) / totalIntT);
				int print_seg, print_col;
				for (print_col = 0; print_col <= LAST_CALIBRATED_COLUMN;
						print_col++) {
					for (print_seg = 0; print_seg < listTrials->size;
							print_seg++) {
						if (print_col != col || print_seg != seg) {
							fprintf(file_peaks, ",,");
						} else
							fprintf(file_peaks, "%f,%d,", curPeak.value,
									curPeak.is_positive_peak);
					}
				}
				fprintf(file_peaks, "\n");
			}

		}
	}
	fclose(file_data);
	fclose(file_peaks);
}
