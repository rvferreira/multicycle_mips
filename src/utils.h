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

#define SEPARATOR "===============================================\n"

/* Operation Constants */
#define CLOCK_DELAY_TIME 500
#define CYCLES_COUNT 5

struct FetchedInstruction{
	bool PCWriteCond,
		PCWrite,
		IorD,
		MemRead,
		MemWrite,
		MemToReg,
		IRWrite,
		PCSource,
		ALUOp,
		ALUSrcB,
		ALUSrcA,
		RegWrite,
		RegDst;
};

struct SyncedInstruction{
	FetchedInstruction controlSignals;
	sem_t PC_read;
};

typedef struct { char byte[4]; } dataBlock;

extern FILE* bincode;

/* General Purpose Functions */
void fetchJobFromFile(const char* filename, const char* noExtensionFilename);
void simulateClockDelay();
void disableClockDelay();

#endif /* UTILS_H_ */
