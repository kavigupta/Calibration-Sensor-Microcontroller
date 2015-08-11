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

JoinedDataSet read(char* path) {
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("File does not exist!");
		exit(1);
	}
	int len = 0;
	JoinedData *data = malloc(10 * sizeof(JoinedData));
	int capacity = 10;
	char buffer[1000];
	fgets(buffer, 1000, file);
	while (1) {
		JoinedData line = readLine(file);
		if (line.t < 0)
			break;
		memcpy(&(data[len]), &line, sizeof(JoinedData));
		len++;
		if (len == capacity) {
			capacity = capacity * 3 / 2;
			data = realloc(data, capacity * sizeof(JoinedData));
		}
	}
	JoinedDataSet ds = { .values = data, .len = len };
	return ds;
}
JoinedData readLine(FILE* file) {
	double t, magx, magy, magz, gyrx, gyry, gyrz, aclx, acly, aclz;
	int code = fscanf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", &t,
			&magx, &magy, &magz, &gyrx, &gyry, &gyrz, &aclx, &acly, &aclz);
	if (code != 10) {
		JoinedData data = { .t = -1 };
		return data;
	}
	JoinedData data = { .t = t, .mag = { .x = magx, .y = magy, .z = magz }, .gyr =
			{ .x = gyrx, .y = gyry, .z = gyrz }, .acl = { .x = aclx, .y = acly,
			.z = aclz } };
	return data;
}
void writeLine(FILE* file, JoinedData entry) {
	fprintf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", entry.t,
			entry.mag.x, entry.mag.y, entry.mag.z, entry.gyr.x, entry.gyr.y,
			entry.gyr.z, entry.acl.x, entry.acl.y, entry.acl.z);
}
void write(char* path, JoinedDataSet data) {
	int i;
	FILE* file = fopen(path, "w");
	fprintf(file, "Time,mx,my,mz,gx,gy,gz,aclx,acly,aclz");
	for (i = 0; i < data.len; i++) {
		writeLine(file, data.values[i]);
	}
}
