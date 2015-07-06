/*
 * IO.c
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include "IO.h"

#include <stdlib.h>
#include <string.h>

DataSet read(char* path) {
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("File does not exist!");
		exit(1);
	}
	int len = 0;
	Data *data = malloc(10 * sizeof(Data));
	int capacity = 10;
	char buffer[1000];
	fgets(buffer, 1000, file);
	while (1) {
		Data line = readLine(file);
		if (line.t < 0)
			break;
		memcpy(&(data[len]), &line, sizeof(Data));
		len++;
		if (len == capacity) {
			capacity = capacity * 3 / 2;
			data = realloc(data, capacity * sizeof(Data));
		}
	}
	DataSet ds = { .values = data, .len = len };
	return ds;
}
Data readLine(FILE* file) {
	double t, magx, magy, magz, gyrx, gyry, gyrz, aclx, acly, aclz;
	int code = fscanf(file, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf",
			&t, &magx, &magy, &magz, &gyrx, &gyry, &gyrz, &aclx, &acly, &aclz);
	if (code != 10) {
		Data data = { .t = -1 };
		return data;
	}
	Data data = { .t = t, .mag = { .x = magx, .y = magy, .z = magz }, .gyr = {
			.x = gyrx, .y = gyry, .z = gyrz }, .acl = { .x = aclx, .y = acly,
			.z = aclz } };
	return data;
}
void writeLine(FILE* file, Data entry) {
	fprintf(file, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", entry.t,
			entry.mag.x, entry.mag.y, entry.mag.z, entry.gyr.x, entry.gyr.y,
			entry.gyr.z, entry.acl.x, entry.acl.y, entry.acl.z);
}
void write(char* path, DataSet data) {
	int i;
	FILE* file = fopen(path, "w");
	fprintf(file, "Time\tMagX\tMagY\tMagZ\tGyrX\tGyrY\tGyrZ\tAclX\tAclY\tAclZ");
	for (i = 0; i < data.len; i++) {
		writeLine(file, data.values[i]);
	}
}

Vector averageAcl(DataSet data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.len - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].acl.x * dt;
		y += data.values[i].acl.y * dt;
		z += data.values[i].acl.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}

Vector averageMag(DataSet data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.len - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].mag.x * dt;
		y += data.values[i].mag.y * dt;
		z += data.values[i].mag.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}
Vector averageGyr(DataSet data) {
	double x = 0, y = 0, z = 0;
	double tsum = 0;
	int i;
	for (i = 0; i < data.len - 1; i++) {
		double dt = data.values[i + 1].t - data.values[i].t;
		x += data.values[i].gyr.x * dt;
		y += data.values[i].gyr.y * dt;
		z += data.values[i].gyr.z * dt;
		tsum += dt;
	}
	Vector avg = { .x = x / tsum, .y = y / tsum, .z = z / tsum };
	return avg;
}
