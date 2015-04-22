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

#include "control_signals.h"

/*******************************************************************************
*	NOME:		controlSignalsAttrb
*	FUNÇÃO:		Atribui os sinais de controle na estrutura SyncedInstruction
*
*			Tipo					Descrição
*     			--------			-----------
*			SyncedInstruction*		Struct onde se encontram os sinais de controle
*			buffer		 			Inteiro onde os sinais já tenham sido setados
*
*	DESCRIÇÃO:	Cada um dos sinais de controle é atribuído individualmente em
* 	uma estrutura SyncedInstructions. Ela recebe como parâmetro um buffer onde
* 	todos os sinais de controle do ciclo já foram testados. Desta forma, a função
* 	fará uma comparação do buffer com a máscara de cada sinal de controle e o
* 	atribuirá individualmente para 0 ou 1.
*
*	RETORNO:	void
*******************************************************************************/
void controlSignalsAttrb(SyncedInstruction *job, int buffer) {

		job->controlSignals.PCWriteCond = (int)(buffer & ativa_PCWriteCond) ? 1 : 0;
		job->controlSignals.PCWrite = (int)(buffer & ativa_PCWrite) ? 1 : 0;
		job->controlSignals.IorD = (int)(buffer & ativa_IorD) ? 1 : 0;
		job->controlSignals.MemRead = (int)(buffer & ativa_MemRead) ? 1 : 0;
		job->controlSignals.MemWrite = (int)(buffer & ativa_MemWrite) ? 1 : 0;
		job->controlSignals.MemToReg = (int)(buffer & ativa_MemtoReg) ? 1 : 0;
		job->controlSignals.IRWrite = (int)(buffer & ativa_IRWrite) ? 1 : 0;
		job->controlSignals.PCSource0 = (int)(buffer & ativa_PCSource0) ? 1 : 0;
		job->controlSignals.PCSource1 = (int)(buffer & ativa_PCSource1) ? 1 : 0;
		job->controlSignals.ALUOp0 = (int)(buffer & ativa_ALUOp0) ? 1 : 0;
		job->controlSignals.ALUOp1 = (int)(buffer & ativa_ALUOp1) ? 1 : 0;
		job->controlSignals.ALUSrcB0 = (int)(buffer & ativa_ALUSrcB0) ? 1 : 0;
		job->controlSignals.ALUSrcB1 = (int)(buffer & ativa_ALUSrcB1) ? 1 : 0;
		job->controlSignals.ALUSrcA = (int)(buffer & ativa_ALUSrcA) ? 1 : 0;
		job->controlSignals.RegWrite = (int)(buffer & ativa_RegWrite) ? 1 : 0;
		job->controlSignals.RegDst = (int)(buffer & ativa_RegDst) ? 1 : 0;
}

/*******************************************************************************
*	NOME:		setControlSignals
*	FUNÇÃO:		Atribui os sinais de controle
*
*			Tipo					Descrição
*     			--------			-----------
*			SyncedInstruction*		Struct onde se encontram os sinais de controle
*			cycle		 			Inteiro indicando em qual ciclo da CPU está
*
*	DESCRIÇÃO:	Tendo recebido como parâmetro o ciclo da CPU, atribuirá os sinais
*	de controle a um inteiro auxiliar buffer de acordo com o ciclo no qual ela se
*	encontra. A máscara é usada como auxiliar para fazer estas atribuições.
*
*	RETORNO:	void
*******************************************************************************/
void setControlSignals(SyncedInstruction *job, int cycle){

	int buffer = 0;

	if (cycle == 0){
		buffer &= desativa_IorD;
		buffer |= ativa_MemRead;
		buffer |= ativa_IRWrite;
		buffer &= desativa_ALUSrcA;
		buffer &= desativa_ALUSrcB0;
		buffer |= ativa_ALUSrcB1;
		buffer &= desativa_ALUOp0;
		buffer &= desativa_ALUOp1;
		buffer |= ativa_PCWrite;
	}
	else if (cycle == 1){
		buffer &= desativa_ALUSrcA;
		buffer |= ativa_ALUSrcB0;
		buffer |= ativa_ALUSrcB1;
		buffer &= desativa_ALUOp0;
		buffer &= desativa_ALUOp1;
	}
	else if (cycle == 2){
		buffer |= ativa_ALUSrcA;
		buffer |= ativa_ALUSrcB0;
		buffer &= desativa_ALUSrcB1;
		buffer &= desativa_ALUOp0;
		buffer &= desativa_ALUOp1;
	}
	else if (cycle == 3){
		buffer |= ativa_IorD;
		buffer |= ativa_MemRead;
	}
	else if (cycle == 4){
		buffer |= ativa_RegWrite;
		buffer &= desativa_RegDst;
		buffer |= ativa_MemtoReg;
	}
	else if (cycle == 5){
		buffer |= ativa_MemWrite;
		buffer |= ativa_IorD;
	}
	else if (cycle == 6){
		buffer |= ativa_ALUSrcA;
		buffer &= desativa_ALUSrcB0;
		buffer &= desativa_ALUSrcB1;
		buffer |= ativa_ALUOp0;
		buffer &= desativa_ALUOp1;
	}
	else if (cycle == 7){
		buffer |= ativa_RegWrite;
		buffer |= ativa_RegDst;
		buffer &= desativa_MemtoReg;
	}
	else if (cycle == 8){
		buffer |= ativa_ALUSrcA;
		buffer &= desativa_ALUSrcB0;
		buffer &= desativa_ALUSrcB1;
		buffer &= desativa_ALUOp0;
		buffer |= ativa_ALUOp1;
		buffer &= desativa_PCSource0;
		buffer |= ativa_PCSource1;
		buffer |= ativa_PCWriteCond;
	}
	else if (cycle == 9){
		buffer |= ativa_PCWrite;
		buffer |= ativa_PCSource0;
		buffer &= desativa_PCSource1;
	}

	controlSignalsAttrb(job, buffer);
}
