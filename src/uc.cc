/*
 * uc.cc
 *
 *  Created on: Apr 11, 2015
 *      Author: rferreira
 */

#include "uc.h"

using namespace std;

UC_def UC;

int dispatchTable(FetchedInstruction ir) {
	//Tabela de Despacho para o ciclo 1
	return 0;
}

int dispatchTable2(FetchedInstruction ir) {
	//Tabela de Despacho para o ciclo 2
	return 0;
}

int nextCycle(FetchedInstruction ir, int cycle) {
	int newCycle = 0;
	if ((cycle == 0) || (cycle == 3) || (cycle == 6))
		newCycle = cycle + 1;
	else if (cycle == 1)
		newCycle = dispatchTable(ir);
	else if (cycle == 2)
		newCycle = dispatchTable2(ir);

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

		UC.cycle = nextCycle(UC.job.controlSignals, UC.cycle);
		setControlSignals(&(UC.job), UC.cycle);

		UC_update();
		sem_post(&clock_free);
	}
	pthread_exit(0);
}
