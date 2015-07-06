/*
 * IO.h
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include <stdio.h>

#include "Vector.h"

#ifndef _IO_H
#define _IO_H

typedef struct {
	double t;
	Vector acl, gyr, mag;
} Data;

typedef struct {
	int len;
	Data* values;
} DataSet;

DataSet read(char* path);
Data readLine(FILE* file);
void writeLine(FILE* file, Data entry);
void write(char* path, DataSet data);

Vector averageAcl(DataSet data);
Vector averageMag(DataSet data);
Vector averageGyr(DataSet data);

#endif /*_IO_H*/
