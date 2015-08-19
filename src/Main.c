/*
 * Main.c
 *
 *  Created on: Aug 18, 2015
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>

#include "Constants.h"
#include "Controller.h"
#include "IO.h"
#include "list.h"
#include "processor_curve_definer.h"
#include "Utils.h"

int main() {
	list(JoinedData) calibration =
			io_read_joined_dataset(
					"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/calibration.csv");
	cntrl_calibrate(calibration);
	free(calibration.values);
	char* dir_string =
			"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/data";
	void process_dir(char* dir) {
		process_content_folder(dir, DEFAULT_PARAMETERS);
	}
	foreach_in_dir(dir_string, process_dir);
//	char* path =
//			"/home/kavi/Dropbox/workspaces/C/Magnetometer Processor/data/doorknob-good";
//	process_content_folder(path);
	printf("Completed Successfully\n");
	return 0;
}
