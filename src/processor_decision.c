/*
 * processor_decision.c
 *
 *  Created on: Aug 18, 2015
 *      Author: root
 */

#include "processor_decision.h"

#include "analysis_peakfind.h"
#include "DataSet.h"
#include "list.h"

int processor_matches(CurveDefinition curve, NDS t) {
	Trial tr;
	{
		list(NDS) *nds_pass = list_new_NDS();
		list(Trial) * trial_list = analysis_peak_find_all(nds_pass, 0);
		tr = trial_list->values[0];
		list_free_NDS(nds_pass);
		list_free_Trial(trial_list);
	}
	// TODO unstub
	return 0;
}
