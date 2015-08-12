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

JoinedDataSet io_read_joined_dataset(char* path);
Double10 io_read_line_as_Double10(FILE* file);
void io_write_line_as_Double10(FILE* file, Double10 entry);
void io_write_joined_data(char* path, JoinedDataSet data);
void io_write_calibrated_data(char* path, CalibratedDataSet data);
void io_write_peaks(char* path, PeakSet* peaks);
#endif /*_IO_H*/
