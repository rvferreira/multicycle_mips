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
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <string.h>

//Struct UCState{controlSignals}
//Struct ALUCState{controlSignals}

/* Operation Constants */
#define CLOCK_DELAY_TIME 1000
#define PIPELINE_SIZE 5

/* General Purpose Functions */
void fetchJobFromFile(const char* filename, const char* noExtensionFilename);
void simulateClockDelay();
void disableClockDelay();
//signExtend16to32()
//shiftLeft2()
//

#endif /* UTILS_H_ */
