/*
 * cpu_resources.cc
 *
 *  Created on: Apr 3, 2015
 *      Author: rferreira
 */

#include "control_signals.h"

void setControlSignalsBuffer(dataBlock instructionToFetch, int buffer, int cycle){
	char aux = instructionToFetch.byte[0];
	aux = (instructionToFetch.byte[0] & (char)0xFC);

	if (cycle == 1){

	}
	else if (cycle == 2){

	}
	else if (cycle == 3){

	}
	else if (cycle == 4){

	}
	else if (cycle == 5){

	}

}

void setControlSignals(SyncedInstruction *job, dataBlock instructionToFetch) {
	char buffer = instructionToFetch.byte[0];
	buffer = (instructionToFetch.byte[0] & (char)0xFC);

	//When it is "Don't care", the signal will be set to 0 for simplicity
	//R-type instruction: add, sub, and, or, slt.
	if (buffer == 0x00){
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
		//If instruction is an ADD, ALUCtrl = 001
		if (instructionToFetch.byte[3] == 0x20){
			job->controlSignalsALU.ALUCtrl0 = 0;
			job->controlSignalsALU.ALUCtrl1 = 0;
			job->controlSignalsALU.ALUCtrl2 = 1;
		}
		//If instruction is a SUB, ALUCtrl = 010
		else if (instructionToFetch.byte[3] == 0x22){
			job->controlSignalsALU.ALUCtrl0 = 0;
			job->controlSignalsALU.ALUCtrl1 = 1;
			job->controlSignalsALU.ALUCtrl2 = 0;
		}
		//If instruction is an AND, ALUCtrl = 011
		else if (instructionToFetch.byte[3] == 0x24){
			job->controlSignalsALU.ALUCtrl0 = 0;
			job->controlSignalsALU.ALUCtrl1 = 1;
			job->controlSignalsALU.ALUCtrl2 = 1;
		}
		//If instruction is an OR, ALUCtrl = 100
		else if (instructionToFetch.byte[3] == 0x1f){
			job->controlSignalsALU.ALUCtrl0 = 1;
			job->controlSignalsALU.ALUCtrl1 = 0;
			job->controlSignalsALU.ALUCtrl2 = 0;
		}
		//If instruction is a SLT, ALUCtrl = 101
		else if (instructionToFetch.byte[3] == 0x2a){
			job->controlSignalsALU.ALUCtrl0 = 1;
			job->controlSignalsALU.ALUCtrl1 = 0;
			job->controlSignalsALU.ALUCtrl2 = 1;
		}
	}
	//LW instruction.
	else if (buffer == 0x8c){
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
		job->controlSignalsALU.ALUCtrl0 = 0;
		job->controlSignalsALU.ALUCtrl1 = 0;
		job->controlSignalsALU.ALUCtrl2 = 0;
		}
	//SW instruction
	else if (buffer == 0xac){
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
		job->controlSignalsALU.ALUCtrl0 = 0;
		job->controlSignalsALU.ALUCtrl1 = 0;
		job->controlSignalsALU.ALUCtrl2 = 0;
	}
	//BEQ (branch on equal) instruction
	else if (buffer == 0x10){
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
		job->controlSignalsALU.ALUCtrl0 = 0;
		job->controlSignalsALU.ALUCtrl1 = 0;
		job->controlSignalsALU.ALUCtrl2 = 0;
	}
	//Jump instruction
	else if (buffer == 0x08){
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
		job->controlSignalsALU.ALUCtrl0 = 0;
		job->controlSignalsALU.ALUCtrl1 = 0;
		job->controlSignalsALU.ALUCtrl2 = 0;
	}
}
