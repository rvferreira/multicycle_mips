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
 

#include "utils.h"

int ALUControl;

int nodClock = 0;
int debugMode = 0;

void convertTextMIPStoBinMIPS(FILE* origin, FILE* decoded) {
	fseek(origin, 0, SEEK_END);
	int size = (int) ftell(origin);
	rewind(origin);

	if (size) {
		char *buffer = (char *) malloc(sizeof(char) * size);
		fread(buffer, 1, size, origin);

		fputs(buffer, decoded);
		free(buffer);
		//TODO iterate through lines and convert
	}
	else {
		printf("Empty origin file \n");
		exit(0);
	}
}

void fetchJobFromFile(const char* filename, const char* noExtensionFilename) {
	FILE* origin = fopen(filename, "rb");
	bincode = fopen(noExtensionFilename, "wb+");

	convertTextMIPStoBinMIPS(origin, bincode);

	fclose(origin);
}

void simulateClockDelay() {
	long int i;
	if (!nodClock)
		for ( i = 0; i < CLOCK_DELAY_TIME*1000; i++){}
}

void disableClockDelay() {
	nodClock = 1;
}

int hexToInt(char c){
	if ((int) c < 58) return ((int) (c - 48));
	return (int) (c - 65 + 10);
}
