/*
 * uc.h
 *
 *  Created on: Apr 11, 2015
 *      Author: rferreira
 */

#ifndef UC_H_
#define UC_H_

#include "cpu_resources.h"

struct UC_def {
	SyncedInstruction job;
	int cycle;
};

extern UC_def UC;


#endif /* UC_H_ */
