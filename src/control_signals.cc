/*
 * cpu_resources.cc
 *
 *  Created on: Apr 3, 2015
 *      Author: rferreira
 */

#include "control_signals.h"

void controlSignalsAttrb(SyncedInstruction *job, int buffer) {

		job->controlSignals.PCWriteCond = (bool)(buffer & ativa_PCWriteCond);
		job->controlSignals.PCWrite = (bool)(buffer & ativa_PCWrite);
		job->controlSignals.IorD = (bool)(buffer & ativa_IorD);
		job->controlSignals.MemRead = (bool)(buffer & ativa_MemRead);
		job->controlSignals.MemWrite = (bool)(buffer & ativa_MemWrite);
		job->controlSignals.MemToReg = (bool)(buffer & ativa_MemtoReg);
		job->controlSignals.IRWrite = (bool)(buffer & ativa_IRWrite);
		job->controlSignals.PCSource0 = (bool)(buffer & ativa_PCSource0);
		job->controlSignals.PCSource1 = (bool)(buffer & ativa_PCSource1);
		job->controlSignals.ALUOp0 = (bool)(buffer & ativa_ALUOp0);
		job->controlSignals.ALUOp1 = (bool)(buffer & ativa_ALUOp1);
		job->controlSignals.ALUSrcB0 = (bool)(buffer & ativa_ALUSrcB0);
		job->controlSignals.ALUSrcB1 = (bool)(buffer & ativa_ALUSrcB1);
		job->controlSignals.ALUSrcA = (bool)(buffer & ativa_ALUSrcA);
		job->controlSignals.RegWrite = (bool)(buffer & ativa_RegWrite);
		job->controlSignals.RegDst = (bool)(buffer & ativa_RegDst);
}

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
		buffer &= desativa_ALUSrcA;
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
		buffer &= ativa_ALUSrcA;
		buffer &= desativa_ALUSrcB0;
		buffer &= desativa_ALUSrcB1;
		buffer &= ativa_ALUOp0;
		buffer &= desativa_ALUOp1;
	}
	else if (cycle == 7){
		buffer &= ativa_RegWrite;
		buffer &= ativa_RegDst;
		buffer &= desativa_MemtoReg;
	}
	else if (cycle == 8){
		buffer |= ativa_ALUSrcA;
		buffer &= desativa_ALUSrcB0;
		buffer &= desativa_ALUSrcB1;
		buffer &= desativa_ALUOp0;
		buffer &= desativa_ALUOp1;
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
