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
 

#include "uc.h"

using namespace std;

UC_def UC;

//Tabela de Despacho para o ciclo 1
int dispatchTable() {
	
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
		case 0xFC000000:{
			cycle = -1;
			sem_post(&invalid_opcode);
		}
	}

	return cycle;
}

//Tabela de Despacho para o ciclo 2
int dispatchTable2() {
	
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
		if (newCycle == -1) cout << "Terminate instruction found. Hasta la vista, Baby." << endl << endl;
		else cout << PC << ": Entering cycle " << newCycle << endl;
		sem_post(&printSync);
	}

	return newCycle;
}

void UC_update(){
	refreshBuffers();
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

		if (UC.cycle != -1) UC_update();
		sem_post(&clock_free);
	}
	pthread_exit(0);
}

int processInput(int argc, char** argv, const char *option, const char *filename) {
    char  *s1, *s2;
    if (argc > 1) {
        strcat(s1,argv[1]);
        filename = s1;
    }
    if (argc > 2) {
        strcat(s2,argv[2]);
    }

    if (s1[0] == '-') {
        option = s1;
        filename = s2;
    }
    else if (s2[0] == '-') {
        option = s2;
        filename = s1;
    }

    if (argc < 2 || argc > 3) {
        return 0;
    }

    const char *options[3];

    options[0] = "--nodclock";
    options[1] = "--debug";

    if (!strcmp(option, "--help")) {
        printf("%s \n", HELP);
        exit(1);
    }
    return 1;
}