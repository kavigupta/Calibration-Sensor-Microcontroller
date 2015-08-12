/*
 * IO.c
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include "IO.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DataSet.h"

JoinedDataSet io_read_joined_dataset(char* path) {
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
	JoinedDataSet ds = { .values = data, .len = len };
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
void io_write_joined_data(char* path, JoinedDataSet data) {
	int i;
	FILE* file = fopen(path, "w");
	fprintf(file, "Time,aclx,acly,aclz,gyrx,gyry,gyrz,magx,magy,magz");
	for (i = 0; i < data.len; i++) {
		Double10 line = { .joined = data.values[i] };
		io_write_line_as_Double10(file, line);
	}
}
void io_write_calibrated_data(char* path, CalibratedDataSet data) {
	printf("Writing to file: %s\n", path);
	int i;
	FILE* file = fopen(path, "w");
	fprintf(file, "Time,aclx,acly,aclz,gx,gy,gz,mr,mtheta,mphi");
	for (i = 0; i < data.len; i++) {
		Double10 line = { .calibrated = data.values[i] };
		io_write_line_as_Double10(file, line);
	}
}
void io_write_peaks(char* path, PeakSet* peaks) {
	int i;
	FILE* file = fopen(path, "w");
	fprintf(file, "Time,Column,Value,Is Positive Peak\n");
	printf("%d peaks->\n", peaks->size);
	for (i = 0; i < peaks->size; i++) {
		fprintf(file, "%f,%s,%f,%d\n", peaks->values[i].t,
				dataset_column_render(peaks->values[i].in_what), peaks->values[i].value,
				peaks->values[i].is_positive_peak);
	}
	fclose(file);
}
