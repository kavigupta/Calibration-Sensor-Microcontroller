/*
 PeakSet* ps = dataset_peakset_new();
 * IO.c
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include "IO.h"

#include <stdlib.h>

#include "list.h"

/**
 * Reads a csv containing 10 floats into a Double10 union, which can represent
 * a JoinedData or CalibratedData struct.
 */
static Double10 io_read_line_as_Double10(FILE* file);
/**
 * Writes a Double10 to a file as 10 data objects
 */
static void io_write_line_as_Double10(FILE* file, Double10 entry);
/**
 * Like io_safe_fopen, but returns null if path is null
 */
static FILE* io_safe_fopen(char* path, char* rw);
/**
 * Like fclose, but is a no-op if file is null.
 */
static void io_safe_fclose(FILE* file);
/**
 * Reads the buffer as a string
 */
static list(int) *io_read_csv_int(char* buff);

list(JoinedData) io_read_joined_dataset(char* path) {
	FILE* file = io_safe_fopen(path, "r");
	if (file == NULL) {
//		printf("File does not exist! %s", path);
		exit(1);
	}
	int len = 0;
	JoinedData *data = malloc(10 * sizeof(JoinedData));
	int capacity = 10;
	char buffer[1000];
	fgets(buffer, 1000, file);
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
static Double10 io_read_line_as_Double10(FILE* file) {
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
void io_write_joined_data(char* path, list(JoinedData) data) {
	int i;
	FILE* file = io_safe_fopen(path, "w");
	io_safe_fprintf(file, "Time,aclx,acly,aclz,gyrx,gyry,gyrz,magx,magy,magz");
	for (i = 0; i < data.size; i++) {
		Double10 line = { .joined = data.values[i] };
		io_write_line_as_Double10(file, line);
	}
}
void io_write_calibrated_data(char* path, CalibratedDataList data) {
	int i;
	FILE* file = io_safe_fopen(path, "w");
	io_safe_fprintf(file, "Time,aclx,acly,aclz,gx,gy,gz,mr,mtheta,mphi");
	for (i = 0; i < data.len; i++) {
		Double10 line = { .calibrated = data.values[i] };
		io_write_line_as_Double10(file, line);
	}
	io_safe_fclose(file);
}

void io_write_normalized_data_segment_list(char* path_data, char* path_peaks,
list(Trial)* listTrials) {
	FILE* file_data = io_safe_fopen(path_data, "w");
	FILE* file_peaks = io_safe_fopen(path_peaks, "w");
	io_safe_fprintf(file_data, "%s,", "Relative Time (to start of segment)");
	io_safe_fprintf(file_peaks, "%s,", "Relative Time (to start of segment)");
	int col;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		int seg;
		for (seg = 0; seg < listTrials->size; seg++) {
			io_safe_fprintf(file_data, "%s %d,", dataset_column_render(col),
					seg + 1);
			io_safe_fprintf(file_peaks,
					"%s %d - Value,%s %d - Is positive Peak,",
					dataset_column_render(col), seg + 1,
					dataset_column_render(col), seg + 1);
		}
	}
	io_safe_fprintf(file_data, "\n");
	io_safe_fprintf(file_peaks, "\n");
	int seg;
	for (seg = 0; seg < listTrials->size; seg++) {
		Trial currentTrial = listTrials->values[seg];
		NDS nds = currentTrial.data;
		int row;
		double initialT = nds.data.values[nds.ind_start].t;
		for (row = nds.ind_start; row < nds.ind_end; row++) {
			CalibratedData currentData = nds.data.values[row];
			io_safe_fprintf(file_data, "%f,", (currentData.t - initialT));
			for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
				int segSub;
				for (segSub = 0; segSub < listTrials->size; segSub++) {
					if (seg != segSub) {
						io_safe_fprintf(file_data, ",");
					} else
					io_safe_fprintf(file_data, "%f,",
							*dataset_column_get_field(&currentData, col));
				}
			}
			io_safe_fprintf(file_data, "\n");
		}
		int col;
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			list(Peak)* curPeaks = currentTrial.cols[col];
			for (row = 0; row < curPeaks->size; row++) {
				Peak curPeak = curPeaks->values[row];
				io_safe_fprintf(file_peaks, "%f,", (curPeak.t - initialT));
				int print_seg, print_col;
				for (print_col = 0; print_col <= LAST_CALIBRATED_COLUMN;
						print_col++) {
					for (print_seg = 0; print_seg < listTrials->size;
							print_seg++) {
						if (print_col != col || print_seg != seg) {
							io_safe_fprintf(file_peaks, ",,");
						} else
						io_safe_fprintf(file_peaks, "%f,%d,", curPeak.value,
								curPeak.is_positive_peak);
					}
				}
				io_safe_fprintf(file_peaks, "\n");
			}

		}
	}
	io_safe_fclose(file_data);
	io_safe_fclose(file_peaks);
}

void io_write_curve_definition(char* path, CurveDefinition cd) {
	FILE* file = fopen(path, "w");
	fprintf(file, "Samples per Trial\n%d\n", cd.n_samples);
	fprintf(file, "Column,Signature Length,Signature...\n");
	int i;
	for (i = 0; i < cd.calibration_columns->size; i++) {
		list(int) sig = cd.calibration_signatures->values[i];
		fprintf(file, "%d,", cd.calibration_columns->values[i]);
		int j;
		for (j = 0; j < sig.size; j++) {
			fprintf(file, "%d,", sig.values[j]);
		}
		fprintf(file, "\n");
	}
	int col;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		char* rendering = dataset_column_render(col);
		fprintf(file, "%s (mu), %s (sigma),", rendering, rendering);
	}
	fprintf(file, "\n");
	int j;
	for (j = 0; j < cd.n_samples; j++) {
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			fprintf(file, "%f,%f,", cd.distributions[col]->values[j].mu,
					cd.distributions[col]->values[j].sigma);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

CurveDefinition io_read_curve_definition(char* path) {
	FILE* file = fopen(path, "r");
	CurveDefinition cd;
	cd.calibration_columns = list_new_int();
	cd.calibration_signatures = list_new_list__int();
	int col;
	for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
		cd.distributions[col] = list_new_Distribution();
	}
	char buffer[10000];
	fgets(buffer, 10000, file); // ignore n_samp declaration
	fgets(buffer, 10000, file);
	sscanf(buffer, "%d", &cd.n_samples);
	fgets(buffer, 10000, file); // ignore sig declaration
	int i;
	for (i = 0;; i++) {
		fgets(buffer, 10000, file);
		list(int) *line = io_read_csv_int(buffer);
		if (line == NULL)
			break;
		list_add_int(cd.calibration_columns, line->values[0]);
		line->values++;
		line->capacity--;
		line->size--;
		list_add_list__int(cd.calibration_signatures, *line);
		free(line);
	}
	// already ignored the distr declaration
	int j;
	for (j = 0; j < cd.n_samples; j++) {
		fgets(buffer, 10000, file);
		for (col = 0; col <= LAST_CALIBRATED_COLUMN; col++) {
			Distribution ds = { };
			sscanf(buffer, "%lf,%lf,", &ds.mu, &ds.sigma);
			list_add_Distribution(cd.distributions[col], ds);
		}
		fprintf(file, "\n");
	}
	fclose(file);
	return cd;
}

static list(int) *io_read_csv_int(char* buff) {
	list(int)* values = list_new_int();
	while (1) {
		if (buff[0] == '\n')
			return values;
		if (buff[0] > '9' || buff[0] < '0') {
			list_free_int(values);
			return NULL;
		}
		long val = strtol(buff, &buff, 10);
		list_add_int(values, (int) val);
		buff++;
	}
}

static void io_write_line_as_Double10(FILE* file, Double10 line) {
	io_safe_fprintf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
			line.data[0], line.data[1], line.data[2], line.data[3],
			line.data[4], line.data[5], line.data[6], line.data[7],
			line.data[8], line.data[9]);
}
static FILE* io_safe_fopen(char* path, char* rw) {
	if (!path)
		return NULL;
	return fopen(path, rw);
}
static void io_safe_fclose(FILE* path) {
	if (!path)
		return;
	fclose(path);
}
