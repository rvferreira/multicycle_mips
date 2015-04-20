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


#ifndef CPU_RESOURCES_H_
#define CPU_RESOURCES_H_
#define THREAD_INIT_FAIL(THREAD_NAME) "\
	Failure in the " THREAD_NAME " thread Initialization \n"

#include "utils.h"
#include "control_signals.h"
#include "commonVariables.h"
#include <list>
#include <iomanip>

struct UC_def {
	SyncedInstruction job;
	int cycle;
};

extern UC_def UC;

extern sem_t UC_free,		//
		UC_mux_memAddress,	//
		UC_mux_WriteRegIR,	//
		UC_mux_WriteDataIR,	//
		UC_mux_ALUA,		//
		UC_mux_ALUB,		//
		UC_mux_PC,			//
		invalid_opcode;

extern sem_t clock_free, clock_updated, printSync;

extern pthread_t uc_handle, memory_handle, clockedMemory_handle,
		instructionRegister_handle, mux_memoryAdress_handle,
		mux_WriteRegIR_handle, mux_WriteDataIR_handle, signExtend_handle,
		shiftLeft2_muxPC_handle, shiftLeft2_muxALUB_handle, mux_ALUA_handle,
		ALU_handle, ALUControl_handle, mux_ALUB_handle, mux_PC_handle, ports_PC_handle;

extern int *memoryBank, memorySize;
extern int PC, MDR, IR, A, B, AluOut;

void resourcesInit();
void refreshBuffers();

#endif /* CPU_RESOURCES_H_ */
