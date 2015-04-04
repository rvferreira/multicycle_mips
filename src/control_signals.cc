/*
 * cpu_resources.cc
 *
 *  Created on: Apr 3, 2015
 *      Author: rferreira
 */

#include "control_signals.h"

void setControlSignals(SyncedInstruction *job, dataBlock instructionToFetch) {
	//When it is "Don't care", the signal will be set to 0 for simplicity
	//R-type instruction: add, sub, and, or, slt.
	if (instructionToFetch.byte[0] == 0 && instructionToFetch.byte[1] == 0 &&
			instructionToFetch.byte[2] == 0 && instructionToFetch.byte[3] == 0){
		job->controlSignals.PCWriteCond = 0;
		job->controlSignals.PCWrite = 0;
		job->controlSignals.IorD = 0;
		job->controlSignals.MemRead = 0;
		job->controlSignals.MemWrite = 0;
		job->controlSignals.MemToReg = 0;
		job->controlSignals.IRWrite = 0;
		job->controlSignals.PCSource0 = 0;
		job->controlSignals.PCSource1 = 0;
		job->controlSignals.ALUOp0 = 1;
		job->controlSignals.ALUOp1 = 0;
		job->controlSignals.ALUSrcB0 = 0;
		job->controlSignals.ALUSrcB1 = 0;
		job->controlSignals.ALUSrcA = 1;
		job->controlSignals.RegWrite = 1;
		job->controlSignals.RegDst = 1;
	}
	//LW instruction.
	else if (instructionToFetch.byte[0] == 0 && instructionToFetch.byte[1] == 0 &&
				instructionToFetch.byte[2] == 0 && instructionToFetch.byte[3] == 0){
		job->controlSignals.PCWriteCond = 0;
		job->controlSignals.PCWrite = 0;
		job->controlSignals.IorD = 1;
		job->controlSignals.MemRead = 1;
		job->controlSignals.MemWrite = 0;
		job->controlSignals.MemToReg = 1;
		job->controlSignals.IRWrite = 0;
		job->controlSignals.PCSource0 = 0;
		job->controlSignals.PCSource1 = 0;
		job->controlSignals.ALUOp0 = 0;
		job->controlSignals.ALUOp1 = 0;
		job->controlSignals.ALUSrcB0 = 1;
		job->controlSignals.ALUSrcB1 = 0;
		job->controlSignals.ALUSrcA = 0;
		job->controlSignals.RegWrite = 1;
		job->controlSignals.RegDst = 0;
		}
	//SW instruction
	else if (instructionToFetch.byte[0] == 0 && instructionToFetch.byte[1] == 0 &&
				instructionToFetch.byte[2] == 0 && instructionToFetch.byte[3] == 0){
		job->controlSignals.PCWriteCond = 0;
		job->controlSignals.PCWrite = 0;
		job->controlSignals.IorD = 1;
		job->controlSignals.MemRead = 0;
		job->controlSignals.MemWrite = 1;
		job->controlSignals.MemToReg = 0;
		job->controlSignals.IRWrite = 0;
		job->controlSignals.PCSource0 = 0;
		job->controlSignals.PCSource1 = 0;
		job->controlSignals.ALUOp0 = 0;
		job->controlSignals.ALUOp1 = 0;
		job->controlSignals.ALUSrcB0 = 1;
		job->controlSignals.ALUSrcB1 = 0;
		job->controlSignals.ALUSrcA = 0;
		job->controlSignals.RegWrite = 0;
		job->controlSignals.RegDst = 0;
	}
	//BEQ (branch on equal) instruction
	else if (instructionToFetch.byte[0] == 0 && instructionToFetch.byte[1] == 0 &&
				instructionToFetch.byte[2] == 0 && instructionToFetch.byte[3] == 0){
		job->controlSignals.PCWriteCond = 1;
		job->controlSignals.PCWrite = 0;
		job->controlSignals.IorD = 0;
		job->controlSignals.MemRead = 0;
		job->controlSignals.MemWrite = 0;
		job->controlSignals.MemToReg = 0;
		job->controlSignals.IRWrite = 0;
		job->controlSignals.PCSource0 = 0;
		job->controlSignals.PCSource1 = 1;
		job->controlSignals.ALUOp0 = 0;
		job->controlSignals.ALUOp1 = 1;
		job->controlSignals.ALUSrcB0 = 0;
		job->controlSignals.ALUSrcB1 = 0;
		job->controlSignals.ALUSrcA = 1;
		job->controlSignals.RegWrite = 0;
		job->controlSignals.RegDst = 0;
	}
	//Jump instruction
	else if (instructionToFetch.byte[0] == 0 && instructionToFetch.byte[1] == 0 &&
				instructionToFetch.byte[2] == 0 && instructionToFetch.byte[3] == 0){
		job->controlSignals.PCWriteCond = 0;
		job->controlSignals.PCWrite = 1;
		job->controlSignals.IorD = 0;
		job->controlSignals.MemRead = 0;
		job->controlSignals.MemWrite = 0;
		job->controlSignals.MemToReg = 0;
		job->controlSignals.IRWrite = 0;
		job->controlSignals.PCSource0 = 1;
		job->controlSignals.PCSource1 = 0;
		job->controlSignals.ALUOp0 = 0;
		job->controlSignals.ALUOp1 = 0;
		job->controlSignals.ALUSrcB0 = 0;
		job->controlSignals.ALUSrcB1 = 0;
		job->controlSignals.ALUSrcA = 0;
		job->controlSignals.RegWrite = 0;
		job->controlSignals.RegDst = 0;
	}
}
