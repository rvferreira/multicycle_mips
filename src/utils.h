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
#define CLOCK_DELAY_TIME 200000
#define CYCLES_COUNT 5

struct ALUControl{
	bool ALUCtrl0,
		ALUCtrl1,
		ALUCtrl2;
};

struct FetchedInstruction{
	bool PCWriteCond,
		PCWrite,
		IorD,
		MemRead,
		MemWrite,
		MemToReg,
		IRWrite,
		PCSource0,
		PCSource1,
		ALUOp0,
		ALUOp1,
		ALUSrcB0,
		ALUSrcB1,
		ALUSrcA,
		RegWrite,
		RegDst;
};

struct SyncedInstruction{
	ALUControl controlSignalsALU;
	FetchedInstruction controlSignals;
	int PC;
};

typedef struct { char byte[4]; } dataBlock;

extern FILE* bincode;
extern bool debugMode;

/* General Purpose Functions */
void fetchJobFromFile(const char* filename, const char* noExtensionFilename);
void simulateClockDelay();
void disableClockDelay();

#endif /* UTILS_H_ */
