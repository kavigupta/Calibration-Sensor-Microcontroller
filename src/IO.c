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

RawDataSet read(char* path) {
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("File does not exist!");
		exit(1);
	}
	int len = 0;
	RawData *data = malloc(10 * sizeof(RawData));
	int capacity = 10;
	char buffer[1000];
	fgets(buffer, 1000, file);
	while (1) {
		RawData line = readLine(file);
		if (line.t < 0)
			break;
		memcpy(&(data[len]), &line, sizeof(RawData));
		len++;
		if (len == capacity) {
			capacity = capacity * 3 / 2;
			data = realloc(data, capacity * sizeof(RawData));
		}
	}
	RawDataSet ds = { .values = data, .len = len };
	return ds;
}
RawData readLine(FILE* file) {
	double t, magx, magy, magz, gyrx, gyry, gyrz, aclx, acly, aclz;
	int code = fscanf(file, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf",
			&t, &magx, &magy, &magz, &gyrx, &gyry, &gyrz, &aclx, &acly, &aclz);
	if (code != 10) {
		RawData data = { .t = -1 };
		return data;
	}
	RawData data = { .t = t, .mag = { .x = magx, .y = magy, .z = magz }, .gyr = {
			.x = gyrx, .y = gyry, .z = gyrz }, .acl = { .x = aclx, .y = acly,
			.z = aclz } };
	return data;
}
void writeLine(FILE* file, RawData entry) {
	fprintf(file, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", entry.t,
			entry.mag.x, entry.mag.y, entry.mag.z, entry.gyr.x, entry.gyr.y,
			entry.gyr.z, entry.acl.x, entry.acl.y, entry.acl.z);
}
void write(char* path, RawDataSet data) {
	int i;
	FILE* file = fopen(path, "w");
	fprintf(file, "Time\tMagX\tMagY\tMagZ\tGyrX\tGyrY\tGyrZ\tAclX\tAclY\tAclZ");
	for (i = 0; i < data.len; i++) {
		writeLine(file, data.values[i]);
	}
}
