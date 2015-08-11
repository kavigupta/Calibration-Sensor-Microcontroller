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


JoinedDataSet read(char* path);
JoinedData readLine(FILE* file);
void writeLine(FILE* file, JoinedData entry);
void write(char* path, JoinedDataSet data);

#endif /*_IO_H*/
