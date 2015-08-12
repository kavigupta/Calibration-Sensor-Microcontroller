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

/**
 * Reads a JoinedDataSet into memory from a file.
 */
JoinedDataSet io_read_joined_dataset(char* path);
/**
 * Reads a csv containing 10 floats into a Double10 union, which can represent
 * a JoinedData or CalibratedData struct.
 */
Double10 io_read_line_as_Double10(FILE* file);
/**
 * Writes a Double10 to a file as 10 data objects
 */
void io_write_line_as_Double10(FILE* file, Double10 entry);
/**
 * Writes the given joined data set to a file with headers
 */
void io_write_joined_data(char* path, JoinedDataSet data);
/**
 * Writes the given calibrated data set to a file with headers
 */
void io_write_calibrated_data(char* path, CalibratedDataSet data);
/**
 * Writes a set of peaks to a file with headers
 */
void io_write_peaks(char* path, PeakSet* peaks);
#endif /*_IO_H*/
