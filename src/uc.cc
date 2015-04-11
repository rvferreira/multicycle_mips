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

	if (debugMode)
		sem_wait(&printSync);
	if (newCycle)
		cout << PC << ": Entering cycle " << newCycle << endl;
	else if (PC!=-1)
		cout << "Instruction finished" << endl << endl;
	;
	sem_post(&printSync);

	return newCycle;
}

void *uc_thread(void* thread_id) {
	UC.cycle = -1;
	while (1) {
		sem_wait(&clock_updated);
		sem_wait(&UC_free);

		UC.cycle = nextCycle(UC.job.controlSignals, UC.cycle);
		if (!UC.cycle) {
			PC++;
			if (!PC) cout << "PC has been incremented to " << PC << endl << endl;
		}
		setControlSignals(&(UC.job), UC.cycle);

		sem_post(&UC_updated);
		sem_post(&clock_free);
	}
	pthread_exit(0);
}
