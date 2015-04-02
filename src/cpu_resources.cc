/*
 * cpu_resources.cc
 *
 *  Created on: Mar 30, 2015
 *      Author: rferreira
 */

#include "cpu_resources.h"

using namespace std;

FILE *bincode;
dataBlock *memoryBank;

struct UC {
	list<SyncedInstruction> jobs;
	list<SyncedInstruction>::iterator cycle[CYCLES_COUNT];
} UC;

int PC;
sem_t PC_updated, PC_free;

pthread_t memory_handle, clockedMemory_handle, mux_memoryAdress_handle,
		mux_WriteRegIR_handle, mux_WriteDataIR_handle, signExtend_handle,
		shiftLeft2_handle, mux_ALUA_handle, ALU_handle, mux_ALUB_handle, mux_PC_handle,
		and_PC_handle, or_pc_handle;

void setControlSignals (SyncedInstruction *job, dataBlock instructionToFetch){
	//TODO implement masks usage
	job->controlSignals.PCWriteCond = 0;
	job->controlSignals.PCWrite = 0;
	job->controlSignals.IorD = 0;
	job->controlSignals.MemRead = 0;
	job->controlSignals.MemWrite = 0;
	job->controlSignals.MemToReg = 0;
	job->controlSignals.IRWrite = 0;
	job->controlSignals.PCSource = 0;
	job->controlSignals.ALUOp = 0;
	job->controlSignals.ALUSrcB = 0;
	job->controlSignals.ALUSrcA = 0;
	job->controlSignals.RegWrite = 0;
	job->controlSignals.RegDst = 0;
}

void createAndEnqueueJob(bool isNop){ //TODO isnop implementation
	SyncedInstruction *newJob = new SyncedInstruction;
	setControlSignals(newJob, memoryBank[PC]);
	UC.jobs.push_back(*newJob);
}

void *memory(void *thread_id) {
	/*memory load*/
	fseek(bincode, 0, SEEK_END);
	int size = (ftell(bincode) - 1) / 4;
	cout << SEPARATOR << "We have found " << size << " lines of code." << endl
			<< SEPARATOR;
	rewind(bincode);
	if (!size) {
		cout << "Oops, there's nothing to be executed." << endl;
		exit(0);
	}

	memoryBank = (dataBlock *) malloc(sizeof(dataBlock) * size);
	dataBlock buffer;

	std::cout << "Memory Initial State:" << std::endl << std::endl;
	for (int i = 0; i < size; i++) {

		for (int j = 0; j < 4; j++) {
			buffer.byte[j] = (char) fgetc(bincode);
			if (buffer.byte[j] == '\n')
				j--;
		}

		memoryBank[i] = buffer;
		cout << memoryBank[i].byte << endl;
	}
	std::cout << SEPARATOR;
	fclose(bincode);
	/*Execution init*/
	PC = 0;
	sem_post(&PC_updated);
	pthread_exit(0);
}

void *clockedMemoryAccess(void *thread_id) {
	while(1){
		simulateClockDelay();
		sem_wait(&PC_updated);
		createAndEnqueueJob(false);
		sem_post(&PC_free);
	}
	pthread_exit(0);
}

void *mux_memoryAdress(void *thread_id){

	pthread_exit(0);
}

void *mux_WriteRegIR(void *thread_id){

	pthread_exit(0);
}

void *mux_WriteDataIR(void *thread_id){

	pthread_exit(0);
}

void *mux_signExtend(void *thread_id){

	pthread_exit(0);
}

void resourcesInit() {
	sem_init(&PC_updated, 0, 0);
	sem_init(&PC_free, 0, 0);
	if (pthread_create(&memory_handle, 0, memory, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Memory");
		exit(0);
	}
	if (pthread_create(&clockedMemory_handle, 0, clockedMemoryAccess, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Clock");
		exit(0);
	}
	if (pthread_create(&mux_memoryAdress_handle, 0, mux_memoryAdress, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Mux Memory Address");
		exit(0);
	}
	if (pthread_create(&mux_WriteRegIR_handle, 0, mux_WriteRegIR, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Mux WriteRegIR");
		exit(0);
	}
	if (pthread_create(&mux_WriteDataIR_handle, 0, mux_WriteDataIR, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Mux WriteDataIR");
		exit(0);
	}
	if (pthread_create(&signExtend_handle, 0, mux_signExtend, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Mux Sign Extend");
		exit(0);
	}
}
