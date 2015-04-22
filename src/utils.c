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

/*******************************************************************************
*	NOME:		convertTextMIPStoBinMIPS
*	FUNÇÃO:		Converte o arquivo de texto da instrução para Binário
*
*	Obs.: A implementar conversão
*
*	RETORNO:	void
*******************************************************************************/
void convertTextMIPStoBinMIPS(FILE* origin, FILE* decoded) {
	fseek(origin, 0, SEEK_END);
	int size = (int) ftell(origin);
	rewind(origin);

	if (size) {
		char *buffer = (char *) malloc(sizeof(char) * size);
		fread(buffer, 1, size, origin);

		fputs(buffer, decoded);
		free(buffer);
	}
	else {
		printf("Empty origin file \n");
		exit(0);
	}
}

/*******************************************************************************
*	NOME:		fetchJobFromFile
*	FUNÇÃO:
*
*			Tipo				Descrição
*     			--------		-----------
*			const char*			arquivo onde estarão as instruções
*			const char*			arquivo onde serão colocadas as instruções em binário
*
*	RETORNO:	void
*******************************************************************************/
void fetchJobFromFile(const char* filename, const char* noExtensionFilename) {
	FILE* origin = fopen(filename, "rb");
	bincode = fopen(noExtensionFilename, "wb+");

	convertTextMIPStoBinMIPS(origin, bincode);

	fclose(origin);
}

/*******************************************************************************
*	NOME:		simulateClockDelay
*	FUNÇÃO:		Simula o clock da máquina
*
*	RETORNO:	void
*******************************************************************************/
void simulateClockDelay() {
	long int i;
	if (!nodClock)
		for ( i = 0; i < CLOCK_DELAY_TIME*1000; i++){}
}

/*******************************************************************************
*	NOME:		disableClockDelay
*	FUNÇÃO:		Desabilita o clock
*
*	RETORNO:	void
*******************************************************************************/
void disableClockDelay() {
	nodClock = 1;
}

/*******************************************************************************
*	NOME:		hexToInt
*	FUNÇÃO:		Converte de hexa para inteiro
*
*			Tipo					Descrição
*     			--------			-----------
*			char c					Caracter para conversão
*
*	RETORNO:	Int: valor convertido
*******************************************************************************/
int hexToInt(char c){
	if ((int) c < 58) return ((int) (c - 48));
	return (int) (c - 65 + 10);
}
