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

sem_t instructions_updated, instructions_free;

// sem_updated means that its information is ready to be read
// sem_free means that it's available for writing
// syncs mutexes
sem_t PC_updated, PC_free, mux_memoryAdress_updated, mux_memoryAdress_free,
		clockedMemory_updated, clockedMemory_free, instructionRegister_updated,
		instructionRegister_free, mux_WriteRegIR_updated, mux_WriteRegIR_free,
		mux_WriteDataIR_updated, mux_WriteDataIR_free, signExtend_updated,
		signExtend_free, shiftLeft2_updated, shiftLeft2_free, mux_ALUA_updated,
		mux_ALUA_free, ALU_updated, ALU_free, mux_ALUB_updated, mux_ALUB_free,
		mux_PC_updated, mux_PC_free;

pthread_t memory_handle, clockedMemory_handle, instructionRegister_handle,
		mux_memoryAdress_handle, mux_WriteRegIR_handle, mux_WriteDataIR_handle,
		signExtend_handle, shiftLeft2_handle, mux_ALUA_handle, ALU_handle,
		mux_ALUB_handle, mux_PC_handle, and_PC_handle, or_pc_handle;

void setControlSignals(SyncedInstruction *job, dataBlock instructionToFetch) {
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

void createAndEnqueueJob(bool isNop) { //TODO isnop implementation
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
	if (debugMode) cout << "PC has been initiated in " << PC << endl;
	sem_post(&PC_updated);
	pthread_exit(0);
}

void *mux_memoryAdress(void *thread_id) {
	while (1) {
		sem_wait(&PC_updated);
		sem_wait(&mux_memoryAdress_free);

		if (debugMode) cout << "1" << endl;

		sem_post(&mux_memoryAdress_updated);
		sem_post(&PC_free);
	}
	pthread_exit(0);
}

void *clockedMemoryAccess(void *thread_id) {
	while (1) {
		//simulateClockDelay();
		sem_wait(&mux_memoryAdress_updated);
		sem_wait(&clockedMemory_free);

		if (debugMode) cout << "2" << endl;
		//mux0 createAndEnqueueJob(false);
		//mux1 sw

		sem_post(&clockedMemory_updated);
		sem_post(&mux_memoryAdress_free);
	}
	pthread_exit(0);
}

void *instructionRegister(void *thread_id) {
	while (1) {
		sem_wait(&clockedMemory_updated);
		sem_wait(&instructionRegister_free);

		if (debugMode) cout << "3" << endl;

		sem_post(&instructionRegister_updated);
		sem_post(&clockedMemory_free);
	}
	pthread_exit(0);
}

void *mux_WriteRegIR(void *thread_id) {
	while(1){
		sem_wait(&instructionRegister_updated);
		sem_wait(&mux_WriteRegIR_free);

		if (debugMode) cout << "4" << endl;

		sem_post(&mux_WriteRegIR_updated);
		sem_post(&instructionRegister_free);
	}
	pthread_exit(0);
}

void *mux_WriteDataIR(void *thread_id) {
	/*while(1){
		sem_wait(&anterior_updated);
		sem_wait(&proprio_free);

		sem_post(&proprio_updated);
		sem_post(&anterior_free);
	}*/
	pthread_exit(0);
}

void *mux_signExtend(void *thread_id) {
	/*while(1){
		sem_wait(&anterior_updated);
		sem_wait(&proprio_free);

		sem_post(&proprio_updated);
		sem_post(&anterior_free);
	}*/
	pthread_exit(0);
}

void *shiftLeft2(void *thread_id) {
	/*while(1){
		sem_wait(&anterior_updated);
		sem_wait(&proprio_free);

		sem_post(&proprio_updated);
		sem_post(&anterior_free);
	}*/
	pthread_exit(0);
}

void *mux_ALUA(void *thread_id) {
	while(1){
		sem_wait(&mux_WriteRegIR_updated);
		sem_wait(&mux_ALUA_free);

		if (debugMode) cout << "5" << endl;

		sem_post(&mux_ALUA_updated);
		sem_post(&mux_WriteRegIR_free);
	}
	pthread_exit(0);
}

void *ALU(void *thread_id) {
	while(1){
		sem_wait(&mux_ALUA_updated);
		sem_wait(&ALU_free);

		if (debugMode) cout << "6" << endl;

		sem_post(&ALU_updated);
		sem_post(&mux_ALUA_free);

	}
	pthread_exit(0);
}

void *mux_ALUB(void *thread_id) {
	/*while(1){
		sem_wait(&anterior_updated);
		sem_wait(&proprio_free);

		sem_post(&proprio_updated);
		sem_post(&anterior_free);
	}*/
	pthread_exit(0);
}

void *mux_PC(void *thread_id) {
	while(1){
		sem_wait(&ALU_updated);
		sem_wait(&PC_free);

		simulateClockDelay();
		PC++;
		if (debugMode) cout << "Done! PC incremented to " << PC << endl;

		sem_post(&PC_updated);
		sem_post(&ALU_free);
	}
	pthread_exit(0);
}

void *and_PC(void *thread_id) {
	/*while(1){
		sem_wait(&anterior_updated);
		sem_wait(&proprio_free);

		sem_post(&proprio_updated);
		sem_post(&anterior_free);
	}*/
	pthread_exit(0);
}

void *or_PC(void *thread_id) {
	/*while(1){
		sem_wait(&anterior_updated);
		sem_wait(&proprio_free);

		sem_post(&proprio_updated);
		sem_post(&anterior_free);
	}*/
	pthread_exit(0);
}

void semaphores_init() {
	sem_init(&PC_updated, 0, 0);
	sem_init(&PC_free, 0, 1);

	sem_init(&mux_memoryAdress_updated, 0, 0);
	sem_init(&mux_memoryAdress_free, 0, 1);

	sem_init(&clockedMemory_updated, 0, 0);
	sem_init(&clockedMemory_free, 0, 1);

	sem_init(&instructionRegister_updated, 0, 0);
	sem_init(&instructionRegister_free, 0, 1);

	sem_init(&mux_WriteRegIR_updated, 0, 0);
	sem_init(&mux_WriteRegIR_free, 0, 1);

	sem_init(&mux_WriteDataIR_updated, 0, 0);
	sem_init(&mux_WriteDataIR_free, 0, 1);

	sem_init(&signExtend_updated, 0, 0);
	sem_init(&signExtend_free, 0, 1);

	sem_init(&shiftLeft2_updated, 0, 0);
	sem_init(&shiftLeft2_free, 0, 1);

	sem_init(&mux_ALUA_updated, 0, 0);
	sem_init(&mux_ALUA_free, 0, 1);

	sem_init(&ALU_updated, 0, 0);
	sem_init(&ALU_free, 0, 1);

	sem_init(&mux_ALUB_updated, 0, 0);
	sem_init(&mux_ALUB_free, 0, 1);

	sem_init(&mux_PC_updated, 0, 0);
	sem_init(&mux_PC_free, 0, 1);
}

void resourcesInit() {
	semaphores_init();

	sem_init(&instructions_updated, 0, 0);
	sem_init(&instructions_free, 0, CYCLES_COUNT);

	if (pthread_create(&memory_handle, 0, memory, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Memory");
		exit(0);
	}
	if (pthread_create(&clockedMemory_handle, 0, clockedMemoryAccess, NULL) != 0) {
		//TODO incorporate clocked memory into memory
		cout << THREAD_INIT_FAIL("Clocked Memory");
		exit(0);
	}
	if (pthread_create(&instructionRegister_handle, 0, instructionRegister, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Instruction Register");
		exit(0);
	}
	if (pthread_create(&mux_memoryAdress_handle, 0, mux_memoryAdress, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxMemoryAddress");
		exit(0);
	}
	if (pthread_create(&mux_WriteRegIR_handle, 0, mux_WriteRegIR, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxWriteRegIR");
		exit(0);
	}
	if (pthread_create(&mux_WriteDataIR_handle, 0, mux_WriteDataIR, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxWriteDataIR");
		exit(0);
	}
	if (pthread_create(&signExtend_handle, 0, mux_signExtend, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxSign Extend");
		exit(0);
	}
	if (pthread_create(&shiftLeft2_handle, 0, shiftLeft2, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Shiftleft2");
		exit(0);
	}
	if (pthread_create(&mux_ALUA_handle, 0, mux_ALUA, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxALUA");
		exit(0);
	}
	if (pthread_create(&ALU_handle, 0, ALU, NULL) != 0) {
		cout << THREAD_INIT_FAIL("ALU");
		exit(0);
	}
	if (pthread_create(&mux_ALUB_handle, 0, mux_ALUB, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxALUB");
		exit(0);
	}
	if (pthread_create(&mux_PC_handle, 0, mux_PC, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxPC");
		exit(0);
	}
	if (pthread_create(&and_PC_handle, 0, and_PC, NULL) != 0) {
		cout << THREAD_INIT_FAIL("AND PC");
		exit(0);
	}
	if (pthread_create(&or_pc_handle, 0, or_PC, NULL) != 0) {
		cout << THREAD_INIT_FAIL("OR PC");
		exit(0);
	}
}
