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

#include "cpu_resources.h"

using namespace std;

FILE *bincode;
int *memoryBank, memorySize;
int registersBank[32];

int PC, MDR, IR, A, B, AluOut;

/* component level sync mutexes
 *
 * sem_component_updated means that its information is ready to be read from the next component
 * sem_component_free means that it's available for writing
 *
 * */

sem_t UC_free,			//
		UC_mux_memAddress,	//
		UC_mux_WriteRegIR,	//
		UC_mux_WriteDataIR,	//
		UC_mux_ALUA,		//
		UC_mux_ALUB,		//
		UC_mux_PC,			//
		invalid_opcode;

sem_t clock_free, clock_updated,									//
		PC_updated, PC_free,										//PC updated for mux_memAddress
		mux_memoryAdress_updated, mux_memoryAdress_free,			//
		clockedMemory_updated, clockedMemory_free, 					//
		MDR_updated, MDR_free,										//
		instructionRegister_updated, instructionRegister_free,		//
		mux_WriteRegIR_updated, mux_WriteRegIR_free, 				//
		mux_WriteDataIR_updated, mux_WriteDataIR_free,				//
		registers_updated_0, registers_updated_1,					//
		registers_free_0, registers_free_1, 						//
		IR_0_free, IR_0_updated, IR_1_free, IR_1_updated,	
		IR_2_free, IR_2_updated,									//
		signExtend_updated, signExtend_free,						//
		shiftLeft2_muxPC_updated, shiftLeft2_muxPC_free,			//
		shiftLeft2_muxALUB_updated, shiftLeft2_muxALUB_free, 		//
		mux_ALUA_updated, mux_ALUA_free, 							//
		mux_ALUB_updated, mux_ALUB_free, 							//
		ALUControl_updated, ALUControl_free, 						//
		ALU_updated, ALU_free, 										//
		mux_PC_updated, mux_PC_free;								//

sem_t printSync;

pthread_t uc_handle, memory_handle, clockedMemory_handle,
		instructionRegister_handle, mux_memoryAdress_handle,
		mux_WriteRegIR_handle, mux_WriteDataIR_handle, signExtend_handle,
		shiftLeft2_muxPC_handle, shiftLeft2_muxALUB_handle, mux_ALUA_handle,
		ALUControl_handle, ALU_handle, mux_ALUB_handle, mux_PC_handle, ports_PC_handle,
		registers_handle;

void createAndEnqueueJob(bool isNop) {
	SyncedInstruction *newJob = new SyncedInstruction;
	setControlSignals(newJob, 0);
}

void *memory_load(void *thread_id) {
	/*memory load*/
	fseek(bincode, 0, SEEK_END);
	int size = (ftell(bincode) - 1) / 8;
	cout << SEPARATOR << "We have found " << size << " lines of code." << endl
			<< SEPARATOR;
	rewind(bincode);
	if (!size) {
		cout << "Oops, there's nothing to be executed." << endl;
		exit(0);
	}

	memorySize = size + 128;
	memoryBank = (int *) malloc(sizeof(int) * (memorySize));

	int buffer;
	char readChar;

	std::cout << "Memory Initial State:" << std::endl << std::endl;
	for (int i = 0; i < size; i++) {

		buffer = 0x00000000;

		for (int j = 0; j < 8; j++) {
			readChar = (char) fgetc(bincode);
			if (readChar == '\n') {
				j--;
				continue;
			}

			buffer = (buffer << 4) | hexToInt(readChar);
		}

		memoryBank[i] = buffer;
		cout << "0x";
		cout << setfill('0') << setw(8) << uppercase << hex
				<< ((memoryBank[i] & 0xFFFFFFFF) >> 0);
		cout << dec;
		cout << endl;
	}
	std::cout << SEPARATOR;
	fclose(bincode);

	/* registers load */
	for (int i = 0; i < 32; i++){
		registersBank[i] = i;
	}

	for (int i = size; i < memorySize; i++){
		memoryBank[i] = 0;
	}

	/*Execution init*/
	PC = 0;
	if (debugMode)
		cout << "PC has been initiated" << endl;
	sem_post(&PC_updated);
	sem_post(&PC_updated);
	pthread_exit(0);
}

void refreshBuffers(){
	MDR = memory_output;
	A = readData1;
	B = readData2;
	AluOut = ALU_output;
}

void *mux_memoryAdress(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_memAddress);
		sem_wait(&mux_memoryAdress_free);
		sem_wait(&PC_updated);

		if (UC.job.controlSignals.IorD == false) {
			mux_memoryAdress_output = PC / 4;
			if (debugMode) {
				sem_wait(&printSync);
				cout << PC << ": Mux to Memory Address has received PC as " << mux_memoryAdress_output << endl;
				sem_post(&printSync);
			}
		}

		else if (UC.job.controlSignals.IorD == true) {
			mux_memoryAdress_output = AluOut;
			if (debugMode) {
				sem_wait(&printSync);
				cout << PC << ": Mux to Memory Address has received AluOut as " << mux_memoryAdress_output << endl;
				sem_post(&printSync);
			}
		}

		sem_post(&PC_free);
		sem_post(&mux_memoryAdress_updated);
	}
	pthread_exit(0);
}

void *clockedMemoryAccess(void *thread_id) {
	while (1) {
		sem_wait(&mux_memoryAdress_updated);
		sem_wait(&clockedMemory_free);

		if (UC.job.controlSignals.MemRead == true) {
			memory_output = memoryBank[mux_memoryAdress_output];
		}
		if (UC.job.controlSignals.MemWrite == true) {
			memoryBank[mux_memoryAdress_output] = B;
		}

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC
					<< ": Memory is accessing the Address " << mux_memoryAdress_output << " received from Mux to Memory Address"
					<< endl;
			sem_post(&printSync);
		}

		sem_post(&clockedMemory_updated);
		sem_post(&mux_memoryAdress_free);
	}
	pthread_exit(0);
}

void *instructionRegister(void *thread_id) {
	while (1) {

		sem_wait(&clockedMemory_updated);
		sem_wait(&instructionRegister_free);
		sem_wait(&IR_0_free);
		sem_wait(&IR_1_free);
		sem_post(&IR_2_free);

		if (UC.job.controlSignals.IRWrite == true) {
			IR = memory_output;
		}

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": IR has received Memory Data" << endl;
			cout << PC << ":     resulting in " << hex << IR << dec << endl;
			sem_post(&printSync);
		}

		sem_post(&instructionRegister_updated);
		sem_wait(&MDR_free);

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": MDR has received Memory Data" << endl;
			sem_post(&printSync);
		}

		sem_post(&IR_0_updated);
		sem_post(&IR_1_updated);
		sem_post(&IR_2_updated);
		sem_post(&MDR_updated);
		sem_post(&clockedMemory_free);
	}
	pthread_exit(0);
}

void *mux_WriteRegIR(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_WriteRegIR);
		sem_wait(&instructionRegister_updated);
		sem_wait(&mux_WriteRegIR_free);

		if (UC.job.controlSignals.RegDst == false){
			mux_writeReg_output = (int) ((IR & separa_rt) >> 16);
		}

		else if (UC.job.controlSignals.RegDst == true){
			mux_writeReg_output = (int) ((IR & separa_rd) >> 11);
		}

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Mux Write to Register received the bits from IR"
					<< endl;
			sem_post(&printSync);
		}

		sem_post(&mux_WriteRegIR_updated);
		sem_post(&instructionRegister_free);
	}
	pthread_exit(0);
}

void *mux_WriteDataIR(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_WriteDataIR);
		sem_wait(&MDR_updated);
		sem_wait(&mux_WriteDataIR_free);

		if (UC.job.controlSignals.MemToReg == false){
			mux_writeData_output = AluOut;
		}

		if (UC.job.controlSignals.MemToReg == true){
			mux_writeData_output = MDR;
		}

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Mux Write Data received data from MDR" << endl;
			sem_post(&printSync);
		}

		sem_post(&mux_WriteDataIR_updated);
		sem_post(&MDR_free);
	}
	pthread_exit(0);
}

void *registers(void *thread_id) {
	while (1) {
		sem_wait(&mux_WriteRegIR_updated);
		sem_wait(&mux_WriteDataIR_updated);
		sem_wait(&registers_free_0);
		sem_wait(&registers_free_1);

		readData1 = registersBank[(int) ((IR & separa_rs) >> 21)];
		readData2 = registersBank[(int) ((IR & separa_rt) >> 16)];

		if (UC.job.controlSignals.RegWrite == true){
			registersBank[mux_writeReg_output] = mux_writeData_output;
			if (debugMode) {
				sem_wait(&printSync);
				cout << "    Word " <<  mux_writeData_output << " wrote on register " << mux_writeReg_output << endl;
				sem_post(&printSync);
			}
		}

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Registers Bank being accessed" << endl;
			cout << "    A has received " << A << endl;
			cout << "    B has received " << B << endl;
			sem_post(&printSync);
		}

		sem_post(&registers_updated_0);
		sem_post(&registers_updated_1);
		sem_post(&mux_WriteDataIR_free);
		sem_post(&mux_WriteRegIR_free);
	}
	pthread_exit(0);
}

void *signExtend(void *thread_id) {
	while (1) {

		sem_wait(&IR_0_updated);
		sem_wait(&signExtend_free);

		signExtend_output = (int) (IR & 0x0000FFFF);
		if (IR & 0x00008000) signExtend_output |= 0xFFFF0000;

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Sign Extend has operated" << endl;
			cout << "    resulting in " << hex << signExtend_output << dec << endl;
			sem_post(&printSync);
		}

		sem_post(&signExtend_updated);
		sem_post(&IR_0_free);
	}
	pthread_exit(0);
}

void *shiftLeft2_muxPC(void *thread_id) {
	while (1) {
		sem_wait(&IR_1_updated);
		sem_wait(&shiftLeft2_muxPC_free);

		ssl_muxPC_output = (IR&0x03FFFFFF) << 2;

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Shift Left 2 at mux PC has operated" << endl;
			sem_post(&printSync);
		}

		sem_post(&shiftLeft2_muxPC_updated);
		sem_post(&IR_1_free);
	}
	pthread_exit(0);
}

void *shiftLeft2_muxALUB(void *thread_id) {
	while (1) {
		sem_wait(&signExtend_updated);
		sem_wait(&shiftLeft2_muxALUB_free);

		ssl_ALUB_output = signExtend_output << 2;

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC << ": Shift Left 2 at mux ALUB has operated" << endl;
			sem_post(&printSync);
		}

		sem_post(&shiftLeft2_muxALUB_updated);
		sem_post(&signExtend_free);
	}
	pthread_exit(0);
}

void *mux_ALUA(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_ALUA);
		sem_wait(&mux_ALUA_free);
		sem_wait(&PC_updated);
		sem_wait(&registers_updated_0);

		if (UC.job.controlSignals.ALUSrcA == false) {
			mux_ALUA_output = PC;
		}
		else if (UC.job.controlSignals.ALUSrcA == true) {
			mux_ALUA_output = A;
		}

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC
					<< ": Buffer A from ALU has received the data from the Registers"
					<< endl;
			sem_post(&printSync);
		}

		sem_post(&registers_free_0);
		sem_post(&PC_free);
		sem_post(&mux_ALUA_updated);
	}
	pthread_exit(0);
}

void *mux_ALUB(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_ALUB);
		sem_wait(&registers_updated_1);
		sem_wait(&shiftLeft2_muxALUB_updated);
		sem_wait(&mux_ALUB_free);

		if (UC.job.controlSignals.ALUSrcB0 == false
				&& UC.job.controlSignals.ALUSrcB1 == false) {
			mux_ALUB_output = B;
		}

		if (UC.job.controlSignals.ALUSrcB0 == false
				&& UC.job.controlSignals.ALUSrcB1 == true) {
			mux_ALUB_output = 4;
		}

		else if (UC.job.controlSignals.ALUSrcB0 == true
				&& UC.job.controlSignals.ALUSrcB1 == false) {
			mux_ALUB_output = signExtend_output;
		}

		else if (UC.job.controlSignals.ALUSrcB0 == true
				&& UC.job.controlSignals.ALUSrcB1 == true) {
			mux_ALUB_output = ssl_ALUB_output;
		}

		if (debugMode) {
			sem_wait(&printSync);
			cout << PC
					<< ": Buffer B from ALU has received the data from the Registers"
					<< endl;
			sem_post(&printSync);
		}

		sem_post(&mux_ALUB_updated);
		sem_post(&shiftLeft2_muxALUB_free);
		sem_post(&registers_free_1);
	}
	pthread_exit(0);
}

/* thread que representa o controle da ULA */
void *ALUControl (void *thread_id){
	
	while (1){
		
		/* esperamos o IR ficar pronto e logo após liberamos o controle da ULA */
		sem_wait(&IR_2_updated);
		sem_wait(&ALUControl_free);

		// se ALUOP = 00, então é uma LW / SW 
		if (UC.job.controlSignals.ALUOp0 == false
				&& UC.job.controlSignals.ALUOp1 == false) {
			// código de operação para soma
			ALUControl_output = 2;
		}	

		//se ALUOP = 01, então é uma BEQ
		if (UC.job.controlSignals.ALUOp0 == false
				&& UC.job.controlSignals.ALUOp1 == true) {
			// código da operação para subtração
			ALUControl_output = 6;
		}

		//se ALUOP = 10, então é uma instrução tipo-R
		if (UC.job.controlSignals.ALUOp0 == true
				&& UC.job.controlSignals.ALUOp1 == false) {

			// isolando os 4 últimos bits para descobrir que operação do tipo-R signfica
			switch (IR&0x0000000F){
				
				//se for 0 é uma ADD
				case 0:{
					// código da operação para soma
					ALUControl_output = 2;
					break;
				}				
				//se for 2 é uma SUB
				case 2:{
					// código da operação para subtração
					ALUControl_output = 6;
					break;
				}				
				// se for 4 é uma AND	
				case 4:{
					// código de operação para and
					ALUControl_output = 0;
					break;
				}				
				// se for 5 é uma OR 	
				case 5:{
					// código de operação para OR
					ALUControl_output = 1;
					break;	
				}
				// se for 10 é uma SLT	
				case 10:
					// código de operação para SLT
					ALUControl_output = 7;
					break;	
			}
		}

		sem_post(&ALUControl_updated);
		sem_post(&IR_2_free);
	}
	pthread_exit(0);
}


void *ALU(void *thread_id) {
	while (1) {
		sem_wait(&mux_ALUA_updated);
		sem_wait(&mux_ALUB_updated);
		sem_wait(&ALUControl_updated);
		sem_wait(&ALU_free);

		// se o código de operação for 2 é uma soma
		if (ALUControl_output == 2) {
			ALU_output = mux_ALUA_output + mux_ALUB_output;

			if (debugMode) {
				sem_wait(&printSync);
				cout << PC << ": ALU is summing up " << mux_ALUA_output << " and " << mux_ALUB_output << endl;
				sem_post(&printSync);
			}
		}

		//se o código de operação for 6 é uma subtração
		if (ALUControl_output == 6) {
			ALU_output = mux_ALUA_output - mux_ALUB_output;

			if (debugMode) {
				sem_wait(&printSync);
				cout << PC << ": ALU is subtracting " << mux_ALUA_output << " and " << mux_ALUB_output << endl;
				sem_post(&printSync);
			}
		}

		// se o código de operação for 0 é um AND
		if (ALUControl_output == 0) {
			ALU_output = mux_ALUA_output & mux_ALUB_output;
			if (debugMode) {
				sem_wait(&printSync);
				cout << PC << ": ALU is operating logical AND " << mux_ALUA_output << " and " << mux_ALUB_output << endl;
				sem_post(&printSync);
			}
		}

		// se o código de operação for 1 é um OR
		if (ALUControl_output == 1) {
			ALU_output = mux_ALUA_output | mux_ALUB_output;
			if (debugMode) {
				sem_wait(&printSync);
				cout << PC << ": ALU is operating logical OR " << mux_ALUA_output << " and " << mux_ALUB_output << endl;
				sem_post(&printSync);
			}
		}

		// se o código de operação for 7 é um SLT
		if (ALUControl_output == 7) {
			ALU_output =  mux_ALUA_output < mux_ALUB_output ? 1 : 0;
			if (debugMode) {
				sem_wait(&printSync);
				cout << PC << ": ALU is operating logical SLT " << mux_ALUA_output << " and " << mux_ALUB_output << endl;
				sem_post(&printSync);
			}
		}

		if (ALU_output == 0) ALU_zero_output = true;
		else ALU_zero_output = false;

		sem_post(&ALU_updated);
		sem_post(&ALUControl_free);
		sem_post(&mux_ALUA_free);
		sem_post(&mux_ALUB_free);

	}
	pthread_exit(0);
}

void *mux_PC(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_PC);
		sem_wait(&mux_PC_free);
		sem_wait(&ALU_updated);
		sem_wait(&shiftLeft2_muxPC_updated);


		if (UC.job.controlSignals.PCSource0 == false
				&& UC.job.controlSignals.PCSource1 == false) {
			mux_PC_output = ALU_output;
		}

		if (UC.job.controlSignals.PCSource0 == false
				&& UC.job.controlSignals.PCSource1 == true) {
			mux_PC_output = AluOut;
		}

		if (UC.job.controlSignals.PCSource0 == true
				&& UC.job.controlSignals.PCSource1 == false) {
			mux_PC_output = (0x0FFFFFFF&ssl_muxPC_output)|(0xF0000000&PC);
		}

		if (debugMode) {
			sem_wait(&printSync);
			cout << "Cycle finished!" << endl << endl;
			sem_post(&printSync);
		}


		sem_post(&shiftLeft2_muxPC_free);
		sem_post(&ALU_free);
		sem_post(&mux_PC_updated);
	}
	pthread_exit(0);
}

void *ports_PC(void *thread_id) {
	while(1){
		sem_wait(&mux_PC_updated);
		sem_wait(&PC_free);
		sem_wait(&PC_free);

		if ((ALU_zero_output && UC.job.controlSignals.PCWriteCond) || UC.job.controlSignals.PCWrite){
			PC = mux_PC_output;
			if (debugMode){
				sem_wait(&printSync);
				cout << "PC updated to "<< PC << "!! =)"<< endl << endl;
				sem_post(&printSync);
			}
		}

		sem_post(&PC_updated);
		sem_post(&PC_updated);
		sem_post(&mux_PC_free);
		sem_post(&UC_free);
	}
	pthread_exit(0);
}

void semaphores_init() {

	sem_init(&clock_updated, 0, 0);
	sem_init(&clock_free, 0, 1);

	sem_init(&UC_free, 0, 1);
	sem_init(&UC_mux_memAddress, 0, 0);
	sem_init(&UC_mux_WriteRegIR, 0, 0);
	sem_init(&UC_mux_WriteDataIR, 0, 0);
	sem_init(&UC_mux_ALUA, 0, 0);
	sem_init(&UC_mux_ALUB, 0, 0);
	sem_init(&UC_mux_PC, 0, 0);
	sem_init(&invalid_opcode, 0, 0);

	sem_init(&PC_updated, 0, 0);
	sem_init(&PC_free, 0, 0);

	sem_init(&mux_memoryAdress_updated, 0, 0);
	sem_init(&mux_memoryAdress_free, 0, 1);

	sem_init(&clockedMemory_updated, 0, 0);
	sem_init(&clockedMemory_free, 0, 1);

	sem_init(&MDR_updated, 0, 0);
	sem_init(&MDR_free, 0, 1);

	sem_init(&instructionRegister_updated, 0, 0);
	sem_init(&instructionRegister_free, 0, 1);

	sem_init(&mux_WriteRegIR_updated, 0, 0);
	sem_init(&mux_WriteRegIR_free, 0, 1);

	sem_init(&mux_WriteDataIR_updated, 0, 0);
	sem_init(&mux_WriteDataIR_free, 0, 1);

	sem_init(&IR_0_free, 0, 1);
	sem_init(&IR_0_updated, 0, 0);

	sem_init(&IR_1_free, 0, 1);
	sem_init(&IR_1_updated, 0, 0);

	sem_init(&IR_2_free, 0, 1);
	sem_init(&IR_2_updated, 0, 0);

	sem_init(&registers_updated_0, 0, 0);
	sem_init(&registers_updated_1, 0, 0);

	sem_init(&registers_free_0, 0, 1);
	sem_init(&registers_free_1, 0, 1);

	sem_init(&signExtend_updated, 0, 0);
	sem_init(&signExtend_free, 0, 1);

	sem_init(&shiftLeft2_muxPC_updated, 0, 0);
	sem_init(&shiftLeft2_muxPC_free, 0, 1);

	sem_init(&shiftLeft2_muxALUB_updated, 0, 0);
	sem_init(&shiftLeft2_muxALUB_free, 0, 1);

	sem_init(&mux_ALUA_updated, 0, 0);
	sem_init(&mux_ALUA_free, 0, 1);

	sem_init(&mux_ALUB_updated, 0, 0);
	sem_init(&mux_ALUB_free, 0, 1);

	sem_init(&ALUControl_updated, 0, 0);
	sem_init(&ALUControl_free, 0, 1);

	sem_init(&ALU_updated, 0, 0);
	sem_init(&ALU_free, 0, 1);

	sem_init(&mux_PC_updated, 0, 0);
	sem_init(&mux_PC_free, 0, 1);

	sem_init(&printSync, 0, 1);
}

void resourcesInit() {
	semaphores_init();

	if (pthread_create(&memory_handle, 0, memory_load, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Memory Load");
		exit(0);
	}
	if (pthread_create(&clockedMemory_handle, 0, clockedMemoryAccess, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("Clocked Memory");
		exit(0);
	}
	if (pthread_create(&instructionRegister_handle, 0, instructionRegister,
	NULL) != 0) {
		cout << THREAD_INIT_FAIL("Instruction Register");
		exit(0);
	}
	if (pthread_create(&mux_memoryAdress_handle, 0, mux_memoryAdress, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("MuxMemoryAddress");
		exit(0);
	}
	if (pthread_create(&mux_WriteRegIR_handle, 0, mux_WriteRegIR, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxWriteRegIR");
		exit(0);
	}
	if (pthread_create(&mux_WriteDataIR_handle, 0, mux_WriteDataIR, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("MuxWriteDataIR");
		exit(0);
	}
	if (pthread_create(&signExtend_handle, 0, signExtend, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxSign Extend");
		exit(0);
	}
	if (pthread_create(&shiftLeft2_muxPC_handle, 0, shiftLeft2_muxPC, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("Shiftleft2 at Mux PC");
		exit(0);
	}
	if (pthread_create(&shiftLeft2_muxPC_handle, 0, shiftLeft2_muxALUB, NULL)
			!= 0) {
		cout << THREAD_INIT_FAIL("Shiftleft2 at Mux ALUB");
		exit(0);
	}
	if (pthread_create(&registers_handle, 0, registers, NULL) != 0) {
		cout << THREAD_INIT_FAIL("Registers");
		exit(0);
	}
	if (pthread_create(&mux_ALUA_handle, 0, mux_ALUA, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxALUA");
		exit(0);
	}
	if (pthread_create(&ALUControl_handle, 0, ALUControl, NULL) != 0) {
		cout << THREAD_INIT_FAIL("ALUControl");
		exit(0);
	}
	if (pthread_create(&ALU_handle, 0, ALU, NULL) != 0) {
		cout << THREAD_INIT_FAIL("ALU");
		exit(0);
	}
	if (pthread_create(&mux_ALUB_handle, 0, mux_ALUB, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxALUB");
		exit(0);
	}
	if (pthread_create(&mux_PC_handle, 0, mux_PC, NULL) != 0) {
		cout << THREAD_INIT_FAIL("MuxPC");
		exit(0);
	}
	if (pthread_create(&ports_PC_handle, 0, ports_PC, NULL) != 0) {
		cout << THREAD_INIT_FAIL("AND and OR for PC");
		exit(0);
	}
}
