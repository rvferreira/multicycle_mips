/*
 * utils.h
 *
 *  Created on: Mar 30, 2015
 *      Author: rferreira
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//Struct UCState{controlSignals}
//Struct ALUCState{controlSignals}

/* Operation Constants */
#define CLOCK_DELAY_TIME 1000

#define PIPELINE_SIZE 5

/* Operation user options */
bool nodClock = false;

/* General Purpose Functions */
//fetchJobFromFile()
void simulateClockDelay(){
	if (!nodClock) sleep(CLOCK_DELAY_TIME);
}
//signExtend16to32()
//shiftLeft2()
//

#endif /* UTILS_H_ */
