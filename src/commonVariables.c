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

/*Variáveis comuns utilizadas para passar o conteúdo de um registrador
 * para outro entre as diferentes threads (são temporários até que a
 * o valor possa ser atribuído aos registradores (PC, MDR,...) no ciclo correto */

int mux_memoryAdress_output;
int memory_output;
int mux_ALUA_output;
int mux_ALUB_output;
int ALU_output;
int ALU_zero_output;
int ssl_ALUB_output;
int ssl_muxPC_output;
int signExtend_output;
int readData1;
int readData2;
int mux_writeReg_output;
int mux_writeData_output;
int mux_PC_output;
int ALUControl_output;
