/*
 * cpu_resources.cc
 *
 *  Created on: Mar 30, 2015
 *      Author: rferreira
 */

#include "cpu_resources.h"
#include <iomanip>

using namespace std;

FILE *bincode;
int *memoryBank;

int PC;
int MDR;

/* component level sync mutexes
 *
 * sem_component_updated means that its information is ready to be read from the next component
 * sem_component_free means that it's available for writing
 *
 * */

sem_t clock_free, clock_updated;

sem_t PC_updated, PC_free, mux_memoryAdress_updated, mux_memoryAdress_free,
		clockedMemory_updated, clockedMemory_free, MDR_updated, MDR_free,
		instructionRegister_updated, instructionRegister_free,
		mux_WriteRegIR_updated, mux_WriteRegIR_free, mux_WriteDataIR_updated,
		mux_WriteDataIR_free, registers_updated_0, registers_updated_1,
		registers_free_0, registers_free_1, IR_0_free, IR_0_updated, IR_1_free,
		IR_1_updated, signExtend_updated, signExtend_free,
		shiftLeft2_muxPC_updated, shiftLeft2_muxPC_free,
		shiftLeft2_muxALUB_updated, shiftLeft2_muxALUB_free, mux_ALUA_updated,
		mux_ALUA_free, ALU_updated, ALU_free, ALUOut_updated, ALUOut_free,
		mux_ALUB_updated, mux_ALUB_free, mux_PC_updated, mux_PC_free;

sem_t printSync;

pthread_t memory_handle, clockedMemory_handle, instructionRegister_handle,
		mux_memoryAdress_handle, mux_WriteRegIR_handle, mux_WriteDataIR_handle,
		signExtend_handle, shiftLeft2_muxPC_handle, shiftLeft2_muxALUB_handle,
		mux_ALUA_handle, ALU_handle, mux_ALUB_handle, mux_PC_handle,
		and_PC_handle, or_pc_handle, registers_handle;

void createAndEnqueueJob(bool isNop) { //TODO isnop implementation
	SyncedInstruction *newJob = new SyncedInstruction;
	//setControlSignals(newJob, 0);
}

void *memory_load(void *thread_id) {
	/*memory load*/
	fseek(bincode, 0, SEEK_END);
	int size = (ftell(bincode) - 1) / 8;
	cout << SEPARATOR << "We have found " << size << " lines of code." << endl
			<< SEPARATOR;
	rewind(bincode);
	if (!size) {
		cout << "Oops, there's nothing to be executed." << endl;
		exit(0);
	}

	memoryBank = (int *) malloc(sizeof(int) * size);
	int buffer;
	char readChar;

	std::cout << "Memory Initial State:" << std::endl << std::endl;
	for (int i = 0; i < size; i++) {

		buffer = 0x00000000;

		for (int j = 0; j < 8; j++) {
			readChar = (char) fgetc(bincode);
			if (readChar == '\n'){
				j--;
				continue;
			}

			buffer = (buffer << 4) | hexToInt(readChar);
		}

		memoryBank[i] = buffer;
		cout << "0x";
		cout << setfill('0') << setw(8) << uppercase << hex << ((memoryBank[i] & 0xFFFFFFFF)>>0);
		cout << dec;
		cout << endl;
	}
	std::cout << SEPARATOR;
	fclose(bincode);
	/*Execution init*/
	PC = 0;
	if (debugMode)
		cout << "PC has been initiated in " << PC << endl;
	sem_post(&PC_updated);
	pthread_exit(0);
}

void *mux_memoryAdress(void *thread_id) {
	while (1) {
		sem_wait(&PC_updated);
		sem_wait(&mux_memoryAdress_free);

		if (debugMode)
			cout << PC << ": Mux to Memory Address has received PC" << endl;

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

		if (debugMode)
			cout << PC
					<< ": Memory has received the Adress from Mux to Memory Address"
					<< endl;

		sem_post(&clockedMemory_updated);
		sem_post(&mux_memoryAdress_free);
	}
	pthread_exit(0);
}

void *instructionRegister(void *thread_id) {
	while (1) {

		sem_wait(&clockedMemory_updated);
		sem_wait(&instructionRegister_free);
		sem_wait(&IR_0_free);
		sem_wait(&IR_1_free);

		if (debugMode)
			cout << PC << ": IR has received Memory Data" << endl;

		sem_post(&instructionRegister_updated);
		sem_wait(&MDR_free);

		if (debugMode)
			cout << PC << ": MDR has received Memory Data" << endl;

		sem_post(&IR_0_updated);
		sem_post(&IR_1_updated);
		sem_post(&MDR_updated);
		sem_post(&clockedMemory_free);
	}
	pthread_exit(0);
}

void *mux_WriteRegIR(void *thread_id) {
	while (1) {
		sem_wait(&instructionRegister_updated);
		sem_wait(&mux_WriteRegIR_free);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Mux Write to Register received the bits from IR"
					<< endl;
			sem_post(&printSync);
		}

		sem_post(&mux_WriteRegIR_updated);
		sem_post(&instructionRegister_free);
	}
	pthread_exit(0);
}

void *mux_WriteDataIR(void *thread_id) {
	while (1) {
		sem_wait(&MDR_updated);
		sem_wait(&mux_WriteDataIR_free);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Mux Write Data received data from MDR" << endl;
			sem_post(&printSync);
		}

		sem_post(&mux_WriteDataIR_updated);
		sem_post(&MDR_free);
	}
	pthread_exit(0);
}

void *registers(void *thread_id) {
	while (1) {
		sem_wait(&mux_WriteRegIR_updated);
		sem_wait(&mux_WriteDataIR_updated);
		sem_wait(&registers_free_0);
		sem_wait(&registers_free_1);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Registers Bank being accessed" << endl;
			sem_post(&printSync);
		}

		sem_post(&registers_updated_0);
		sem_post(&registers_updated_1);
		sem_post(&mux_WriteDataIR_free);
		sem_post(&mux_WriteRegIR_free);
	}
	pthread_exit(0);
}

void *mux_signExtend(void *thread_id) {
	while (1) {
		sem_wait(&IR_0_updated);
		sem_wait(&signExtend_free);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Sign Extend has operated" << endl;
			sem_post(&printSync);
		}

		sem_post(&signExtend_updated);
		sem_post(&IR_0_free);
	}
	pthread_exit(0);
}

void *shiftLeft2_muxPC(void *thread_id) {
	while (1) {
		sem_wait(&IR_1_updated);
		sem_wait(&shiftLeft2_muxPC_free);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Shift Left 2 at mux PC has operated" << endl;
			sem_post(&printSync);
		}

		sem_post(&shiftLeft2_muxPC_updated);
		sem_post(&IR_1_free);
	}
	pthread_exit(0);
}

void *shiftLeft2_muxALUB(void *thread_id) {
	while (1) {
		sem_wait(&signExtend_updated);
		sem_wait(&shiftLeft2_muxALUB_free);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Shift Left 2 at mux ALUB has operated" << endl;
			sem_post(&printSync);
		}

		sem_post(&shiftLeft2_muxALUB_updated);
		sem_post(&signExtend_free);
	}
	pthread_exit(0);
}

void *mux_ALUA(void *thread_id) {
	while (1) {
		sem_wait(&registers_updated_0);
		sem_wait(&mux_ALUA_free);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC
					<< ": Buffer A from ALU has received the data from the Registers"
					<< endl;
			sem_post(&printSync);
		}

		sem_post(&mux_ALUA_updated);
		sem_post(&registers_free_0);
	}
	pthread_exit(0);
}

void *mux_ALUB(void *thread_id) {
	while (1) {
		sem_wait(&registers_updated_1);
		sem_wait(&mux_ALUB_free);
		sem_wait(&shiftLeft2_muxALUB_updated);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC
					<< ": Buffer B from ALU has received the data from the Registers"
					<< endl;
			sem_post(&printSync);
		}

		sem_post(&mux_ALUB_updated);
		sem_post(&shiftLeft2_muxALUB_free);
		sem_post(&registers_free_1);
	}
	pthread_exit(0);
}

void *ALU(void *thread_id) {
	while (1) {
		sem_wait(&mux_ALUA_updated);
		sem_wait(&mux_ALUB_updated);
		sem_wait(&ALU_free);

		if (debugMode)
			sem_wait(&printSync);
			cout << PC << ": ALU has received data from buffers" << endl;
			sem_post(&printSync);

		sem_post(&ALU_updated);

		sem_wait(&ALUOut_free);

		if (debugMode)
			cout << PC << ": ALUOut has been loaded" << endl;

		sem_post(&ALUOut_updated);

		sem_post(&mux_ALUA_free);
		sem_post(&mux_ALUB_free);

	}
	pthread_exit(0);
}

void *mux_PC(void *thread_id) {
	while (1) {
		sem_wait(&ALU_updated);
		sem_wait(&ALUOut_updated);
		sem_wait(&shiftLeft2_muxPC_updated);
		sem_wait(&PC_free);

		PC++;
		if (debugMode)
			cout << "Done! PC incremented to " << PC << endl << endl;

		sem_wait(&clock_updated);
		sem_post(&clock_free);

		sem_post(&PC_updated);
		sem_post(&shiftLeft2_muxPC_free);
		sem_post(&ALU_free);
		sem_post(&ALUOut_free);
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
	sem_init(&clock_updated, 0, 0);
	sem_init(&clock_free, 0, 1);

	sem_init(&PC_updated, 0, 0);
	sem_init(&PC_free, 0, 0);

	sem_init(&mux_memoryAdress_updated, 0, 0);
	sem_init(&mux_memoryAdress_free, 0, 1);

	sem_init(&clockedMemory_updated, 0, 0);
	sem_init(&clockedMemory_free, 0, 1);

	sem_init(&MDR_updated, 0, 0);
	sem_init(&MDR_free, 0, 1);

	sem_init(&instructionRegister_updated, 0, 0);
	sem_init(&instructionRegister_free, 0, 1);

	sem_init(&mux_WriteRegIR_updated, 0, 0);
	sem_init(&mux_WriteRegIR_free, 0, 1);

	sem_init(&mux_WriteDataIR_updated, 0, 0);
	sem_init(&mux_WriteDataIR_free, 0, 1);

	sem_init(&IR_0_free, 0, 1);
	sem_init(&IR_0_updated, 0, 0);

	sem_init(&IR_1_free, 0, 1);
	sem_init(&IR_1_updated, 0, 0);

	sem_init(&registers_updated_0, 0, 0);
	sem_init(&registers_updated_1, 0, 0);

	sem_init(&registers_free_0, 0, 1);
	sem_init(&registers_free_1, 0, 1);

	sem_init(&signExtend_updated, 0, 0);
	sem_init(&signExtend_free, 0, 1);

	sem_init(&shiftLeft2_muxPC_updated, 0, 0);
	sem_init(&shiftLeft2_muxPC_free, 0, 1);

	sem_init(&shiftLeft2_muxALUB_updated, 0, 0);
	sem_init(&shiftLeft2_muxALUB_free, 0, 1);

	sem_init(&mux_ALUA_updated, 0, 0);
	sem_init(&mux_ALUA_free, 0, 1);

	sem_init(&mux_ALUB_updated, 0, 0);
	sem_init(&mux_ALUB_free, 0, 1);

	sem_init(&ALU_updated, 0, 0);
	sem_init(&ALU_free, 0, 1);

	sem_init(&ALUOut_updated, 0, 0);
	sem_init(&ALUOut_free, 0, 1);

	sem_init(&mux_PC_updated, 0, 0);
	sem_init(&mux_PC_free, 0, 1);

	sem_init(&printSync, 0, 1);
}

void resourcesInit() {
	semaphores_init();

//	sem_init(&instructions_updated, 0, 0);
//	sem_init(&instructions_free, 0, CYCLES_COUNT);

	if (pthread_create(&memory_handle, 0, memory_load, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Memory Load");
		exit(0);
	}
	if (pthread_create(&clockedMemory_handle, 0, clockedMemoryAccess, NULL)
			!= 0) {
		//TODO incorporate clocked memory into memory
		cout << THREAD_INIT_FAIL("Clocked Memory");
		exit(0);
	}
	if (pthread_create(&instructionRegister_handle, 0, instructionRegister,
	NULL) != 0) {
		cout << THREAD_INIT_FAIL("Instruction Register");
		exit(0);
	}
	if (pthread_create(&mux_memoryAdress_handle, 0, mux_memoryAdress, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("MuxMemoryAddress");
		exit(0);
	}
	if (pthread_create(&mux_WriteRegIR_handle, 0, mux_WriteRegIR, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxWriteRegIR");
		exit(0);
	}
	if (pthread_create(&mux_WriteDataIR_handle, 0, mux_WriteDataIR, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("MuxWriteDataIR");
		exit(0);
	}
	if (pthread_create(&signExtend_handle, 0, mux_signExtend, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxSign Extend");
		exit(0);
	}
	if (pthread_create(&shiftLeft2_muxPC_handle, 0, shiftLeft2_muxPC, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("Shiftleft2 at Mux PC");
		exit(0);
	}
	if (pthread_create(&shiftLeft2_muxPC_handle, 0, shiftLeft2_muxALUB, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("Shiftleft2 at Mux ALUB");
		exit(0);
	}
	if (pthread_create(&registers_handle, 0, registers, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Registers");
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
