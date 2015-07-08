/*
 * IO.h
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */

#include <stdio.h>

#include "DataSet.h"

#ifndef _IO_H
#define _IO_H


RawDataSet read(char* path);
RawData readLine(FILE* file);
void writeLine(FILE* file, RawData entry);
void write(char* path, RawDataSet data);

#endif /*_IO_H*/
