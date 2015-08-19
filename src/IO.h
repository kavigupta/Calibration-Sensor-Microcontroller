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

#define io_safe_fprintf(path, ...) 	if (path) fprintf(path, ##__VA_ARGS__);

/**
 * Reads a JoinedDataSet into memory from a file.
 */
list(JoinedData) io_read_joined_dataset(char* path);
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
/**
 * Writes a curve definition to a file, in the format
 *
 * Samples Per Trial
 * <SAMPLES PER TRIAL>
 * Column	Signature Length	Signature...
 * <COL1>	<SIGLEN1>			<SIG EL 1>	<SIG EL 2>	<SIG EL 3>
 * etc.
 * aclx mu, aclx sigma,	acly mu,	acly sigma (etc.)
 * <ACLX mu>	<ACLX SIGMA>	(etc.)
 * (etc. for <SAMPLES PER TRIAL> columns)
 *
 */
void io_write_curve_definition(char* path, CurveDefinition cd);
/**
 * Reads a curve definition from the given file, in the format specified above
 */
CurveDefinition io_read_curve_definition(char* path);
#endif /*_IO_H*/
