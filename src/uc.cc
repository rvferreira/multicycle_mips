/*
 * uc.cc
 *
 *  Created on: Apr 11, 2015
 *      Author: rferreira
 */

#include "uc.h"

using namespace std;

UC_def UC;

int dispatchTable() {
	//Tabela de Despacho para o ciclo 1
	int cycle;
	switch (IR&separa_cop) {
		// instruction type-r
		case 0x00000000: {
			cycle = 6;
			break;
		}
		// instruction beq
		case 0x10000000: {
			cycle = 8; 
			break;
		}
		// instruction jump
		case 0x08000000: {
			cycle = 9;
			break;
		}
		//  instruction lw
		case 0x8c000000: {
			cycle = 2;
			break;
		}
		// instruction sw
		case 0xac000000:{
			cycle = 2;
			break;
		}
	}

	return cycle;
}

int dispatchTable2() {
	//Tabela de Despacho para o ciclo 2
	int cycle;
	switch (IR&separa_cop){
		
		// instruction lw
		case 0x8c000000: {
			cycle = 3;
			break;
		}

		// instruction sw
		case 0xac000000: {
			cycle = 5; 
			break;
		}
	}

	return cycle; 
}

int nextCycle(int cycle) {
	int newCycle = 0;
	if ((cycle == 0) || (cycle == 3) || (cycle == 6))
		newCycle = cycle + 1;
	else if (cycle == 1)
		newCycle = dispatchTable();
	else if (cycle == 2)
		newCycle = dispatchTable2();

	if (debugMode){
		sem_wait(&printSync);
		cout << PC << ": Entering cycle " << newCycle << endl;
		sem_post(&printSync);
	}

	return newCycle;
}

void UC_update(){
	sem_post(&UC_mux_memAddress);
	sem_post(&UC_mux_WriteRegIR);
	sem_post(&UC_mux_WriteDataIR);
	sem_post(&UC_mux_ALUA);
	sem_post(&UC_mux_ALUB);
	sem_post(&UC_mux_PC);
}

void *uc_thread(void* thread_id) {
	UC.cycle = -1;
	while (1) {
		sem_wait(&clock_updated);
		sem_wait(&UC_free);

		UC.cycle = nextCycle(UC.cycle);
		setControlSignals(&(UC.job), UC.cycle);

		UC_update();
		sem_post(&clock_free);
	}
	pthread_exit(0);
}
