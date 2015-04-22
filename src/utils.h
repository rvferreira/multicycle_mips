/*	 **************************************************************************** /
 *	Trabalho de Sistemas Operacionais I - CPU MIPS Multiciclo com PThreads 
 *
 * Grupo 2
 *
 * Integrantes 						
 *		Andressa Andrião 			
 *		Jéssika Darambaris			
 *		Raphael Ferreira
 *
 * Professor 
 *		Paulo Sérgio Souza	
 ******************************************************************************* */
 

#ifndef UTILS_H_
#define UTILS_H_

#include <pthread.h>
#include <semaphore.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <string.h>
#include "mascara.h"

#define SEPARATOR "===============================================\n"

/* Operation Constants */
#define CLOCK_DELAY_TIME 200000
#define CYCLES_COUNT 5

/*Estrutura onde são guardados os sinais de controle*/
typedef struct FetchedInstruction{
	int PCWriteCond,
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
}FetchedInstruction;

typedef struct SyncedInstruction{
	FetchedInstruction controlSignals;
}SyncedInstruction;

typedef struct { char byte[4]; } dataBlock;

extern FILE* bincode;
extern int debugMode;

/* General Purpose Functions */
void fetchJobFromFile(const char* filename, const char* noExtensionFilename);
void simulateClockDelay();
void disableClockDelay();
int hexToInt(char c);

#endif /* UTILS_H_ */
