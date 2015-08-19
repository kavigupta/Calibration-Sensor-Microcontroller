/*
 * processor_afterthefact.h
 *
 *  Created on: Aug 18, 2015
 *      Author: root
 */

#ifndef PROCESSOR_CURVE_DEFINER_H_
#define PROCESSOR_CURVE_DEFINER_H_

#include "analysis.h"

/**
 * Processes the content folder. If the folder does not contain a file
 * marked "/C-readable.csv", it's subfolders will be used as sources
 * of data.
 */
void process_content_folder(char* dir, CurveDefinitionParameters params);

#endif /* PROCESSOR_CURVE_DEFINER_H_ */
