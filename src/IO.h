/*
 * IO.h
 *
 *  Created on: Jul 6, 2015
 *      Author: root
 */


#ifndef _IO_H
#define _IO_H

#include <stdio.h>
#include "DataSet.h"

/**
 * Reads a JoinedDataSet into memory from a file.
 */
list(JoinedData) io_read_joined_dataset(char* path);
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
void io_write_joined_data(char* path, list(JoinedData) data);
/**
 * Writes the given calibrated data set to a file with headers
 */
void io_write_calibrated_data(char* path, CalibratedDataList data);
/**
 * Writes a normalized data segment list to a file, adding columns for every trial in the list.
 */
void io_write_normalized_data_segment_list(char* path_data, char* path_peaks,
		list(Trial)* trials);
/**
 * Writes a set of peaks to a file with headers
 */
void io_write_peaks(char* path, list(Trial)* peaks);
#endif /*_IO_H*/
