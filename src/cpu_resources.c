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

sem_t 	UC_free,							//UC livre para realizar o próximo ciclo
		UC_mux_memAddress,					//mux_memAddress liberado depois de um novo ciclo acionado
		UC_mux_WriteRegIR,					//mux_WriteRegIR liberado depois de um novo ciclo acionado
		UC_mux_WriteDataIR,					//mux_WriteDataIR liberado depois de um novo ciclo acionado
		UC_mux_ALUA,						//mux_ALUA liberado depois de um novo ciclo acionado
		UC_mux_ALUB,						//mux_ALUB liberado depois de um novo ciclo acionado
		UC_mux_PC,							//mux_PC liberado depois de um novo ciclo acionado
		invalid_opcode;						//

/*Todos os semáforos "free" são utilizados para que o próprio componente saiba se ele mesmo
 * está disponível para escrita.*/
sem_t 	clock_free, clock_updated,									//
		PC_updated, PC_free,										//PC updated para mux_memAddress
		mux_memoryAdress_updated, mux_memoryAdress_free,			//mux_memoryAdress udpated para clockedMemoryaccess
		clockedMemory_updated, clockedMemory_free, 					//clockedMemory udpated para instructionsRegister
		MDR_updated, MDR_free,										//MDR updated para mux_WriteDataIR
		instructionRegister_updated, instructionRegister_free,		//instructionRegister updated para mux_WriteRegIR
		mux_WriteRegIR_updated, mux_WriteRegIR_free, 				//mux_WriteRegIR updated para registers
		mux_WriteDataIR_updated, mux_WriteDataIR_free,				//mux_WriteDataIR updated para registers
		registers_updated_0, registers_updated_1,					//registers_updated_0 e registers_updated_1 updated para ALUA e ALUB
		registers_free_0, registers_free_1,
		IR_0_free, IR_0_updated, IR_1_free, IR_1_updated,			//IR_0 updated para signExtend, IR_1 updated para shiftLeft2_muxPC
		IR_2_free, IR_2_updated,									//IR_2 updated para ALUControl
		signExtend_updated, signExtend_free,						//signExtend updated para mux_ALUB
		shiftLeft2_muxPC_updated, shiftLeft2_muxPC_free,			//shiftLeft2_muxPC updated para mux_PC
		shiftLeft2_muxALUB_updated, shiftLeft2_muxALUB_free, 		//shiftLeft2_muxALUB updated para mux_ALUB
		mux_ALUA_updated, mux_ALUA_free, 							//mux_ALUA updated para ALU
		mux_ALUB_updated, mux_ALUB_free, 							//mux_ALUB updated para ALU
		ALUControl_updated, ALUControl_free, 						//ALUControl updated para ALU
		ALU_updated, ALU_free, 										//ALU_updated updated para mux_PC
		mux_PC_updated, mux_PC_free;								//mux_PC updated para ports_PC

sem_t printSync;

pthread_t uc_handle, memory_handle, clockedMemory_handle,
		instructionRegister_handle, mux_memoryAdress_handle,
		mux_WriteRegIR_handle, mux_WriteDataIR_handle, signExtend_handle,
		shiftLeft2_muxPC_handle, shiftLeft2_muxALUB_handle, mux_ALUA_handle,
		ALUControl_handle, ALU_handle, mux_ALUB_handle, mux_PC_handle, ports_PC_handle,
		registers_handle;

/*******************************************************************************
*	NOME:		createAndEnqueueJob
*	FUNÇÃO:		Atribui os sinais de controle na estrutura SyncedInstruction
*
*	DESCRIÇÃO:	Cria um novo job e seta os sinais de controle para iniciar o
*	ciclo 0.
*
*	RETORNO:	void
*******************************************************************************/
void createAndEnqueueJob(int isNop) {
	SyncedInstruction *newJob;
	setControlSignals(newJob, 0);
}

/*******************************************************************************
*	NOME:		memory_load
*	FUNÇÃO:		Carrega as instruções, inicializa a memória, os registradores e
*				PC
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *memory_load(void *thread_id) {
	/*memory load*/
	fseek(bincode, 0, SEEK_END);
	int size = (ftell(bincode) - 1) / 8;
	printf("%s We have found %d lines of code.  \n %s \n",SEPARATOR,size,SEPARATOR);
			
	rewind(bincode);
	
	if (!size) {
		printf("Oops, there's nothing to be executed. \n");
		exit(0);
	}

	memorySize = size + 128;
	memoryBank = (int *) malloc(sizeof(int) * (memorySize));

	int buffer;
	char readChar;

	printf("Memory Initial State: \n\n");
	int i;
	for (i = 0; i < size; i++) {

		buffer = 0x00000000;
		int j;	
		for (j = 0; j < 8; j++) {
			readChar = (char) fgetc(bincode);
			if (readChar == '\n') {
				j--;
				continue;
			}

			buffer = (buffer << 4) | hexToInt(readChar);
		}

		memoryBank[i] = buffer;
 		printf("%#.8x \n",(memoryBank[i] & 0xFFFFFFFF)); 
		
	}

	
	printf(SEPARATOR);
	fclose(bincode);

	/* registers load */
	
	for (i = 0; i < 32; i++){
		registersBank[i] = i;
	}

	for (i = size; i < memorySize; i++){
		memoryBank[i] = 0;
	}

	/*Execution init*/
	PC = 0;
	if (debugMode)
		printf("PC has been initiated \n");
	sem_post(&PC_updated);
	sem_post(&PC_updated);
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		refreshBuffers
*	FUNÇÃO:		Coloca nos registradores os valores do último ciclo que estavam
*				na variável auxiliar
*
*	RETORNO:	void
*******************************************************************************/
void refreshBuffers(){
	MDR = memory_output;
	A = readData1;
	B = readData2;
	AluOut = ALU_output;
}

/*******************************************************************************
*	NOME:		mux_memoryAddress
*	FUNÇÃO:		Thread do componente mux_memoryAddress com todas as suas operações.
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *mux_memoryAdress(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_memAddress);
		sem_wait(&mux_memoryAdress_free);
		sem_wait(&PC_updated);


		if (!UC.job.controlSignals.IorD) {
			mux_memoryAdress_output = PC / 4;
			if (debugMode) {
				sem_wait(&printSync);
				printf("%d : Mux to Memory Address has received PC as %d \n",PC,mux_memoryAdress_output);
				sem_post(&printSync);
			}
		}

		else if (UC.job.controlSignals.IorD) {
			mux_memoryAdress_output = AluOut;
			if (debugMode) {
				sem_wait(&printSync);
				printf("%d: Mux to Memory Address has received AluOut as %d \n",PC,mux_memoryAdress_output);
				sem_post(&printSync);
			}
		}

		sem_post(&PC_free);
		sem_post(&mux_memoryAdress_updated);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		clockedMemoryAccess
*	FUNÇÃO:		Thread do componente MemoryAccess com todas as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *clockedMemoryAccess(void *thread_id) {
	while (1) {
		sem_wait(&mux_memoryAdress_updated);
		sem_wait(&clockedMemory_free);

		if (UC.job.controlSignals.MemRead == 1) {
			memory_output = memoryBank[mux_memoryAdress_output];
		}
		if (UC.job.controlSignals.MemWrite == 1) {
			memoryBank[mux_memoryAdress_output] = B;
		}

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d : Memory is accessing the Address %d received from Mux to Memory Address \n",PC, mux_memoryAdress_output);
			sem_post(&printSync);
		}

		sem_post(&clockedMemory_updated);
		sem_post(&mux_memoryAdress_free);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		instructionRegister
*	FUNÇÃO:		Thread do componente instructionRegister com todas as suas
*				operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *instructionRegister(void *thread_id) {
	while (1) {

		sem_wait(&clockedMemory_updated);
		sem_wait(&instructionRegister_free);
		sem_wait(&IR_0_free);
		sem_wait(&IR_1_free);
		sem_post(&IR_2_free);

		if (UC.job.controlSignals.IRWrite == 1) {
			IR = memory_output;
		}

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: IR has received Memory Data \n",PC); 
			printf("%d: resulting in %#.8x \n",PC,IR); 
			sem_post(&printSync);
		}

		sem_post(&instructionRegister_updated);
		sem_wait(&MDR_free);

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: MDR has received Memory Data \n",PC);
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

/*******************************************************************************
*	NOME:		mux_WriteRegIR
*	FUNÇÃO:		Thread do componente mux_WriteRegIR com todas as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *mux_WriteRegIR(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_WriteRegIR);
		sem_wait(&instructionRegister_updated);
		sem_wait(&mux_WriteRegIR_free);

		if (UC.job.controlSignals.RegDst == 0){
			mux_writeReg_output = (int) ((IR & separa_rt) >> 16);
		}

		else if (UC.job.controlSignals.RegDst == 1){
			mux_writeReg_output = (int) ((IR & separa_rd) >> 11);
		}

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: Mux Write to Register received the bits from IR \n",PC);
			sem_post(&printSync);
		}

		sem_post(&mux_WriteRegIR_updated);
		sem_post(&instructionRegister_free);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		mux_WriteDataIR
*	FUNÇÃO:		Thread do componente mux_WriteDataIR com todas as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *mux_WriteDataIR(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_WriteDataIR);
		sem_wait(&MDR_updated);
		sem_wait(&mux_WriteDataIR_free);

		if (UC.job.controlSignals.MemToReg == 0){
			mux_writeData_output = AluOut;
		}

		if (UC.job.controlSignals.MemToReg == 1){
			mux_writeData_output = MDR;
		}

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: Mux Write Data received data from MDR\n",PC);
			sem_post(&printSync);
		}

		sem_post(&mux_WriteDataIR_updated);
		sem_post(&MDR_free);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		registers
*	FUNÇÃO:		Thread do componente registers com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *registers(void *thread_id) {
	while (1) {
		sem_wait(&mux_WriteRegIR_updated);
		sem_wait(&mux_WriteDataIR_updated);
		sem_wait(&registers_free_0);
		sem_wait(&registers_free_1);

		readData1 = registersBank[(int) ((IR & separa_rs) >> 21)];
		readData2 = registersBank[(int) ((IR & separa_rt) >> 16)];

		if (UC.job.controlSignals.RegWrite == 1){
			registersBank[mux_writeReg_output] = mux_writeData_output;
			if (debugMode) {
				sem_wait(&printSync);
				printf("    Word  %d wrote on register %d \n",mux_writeData_output,mux_writeReg_output);
				sem_post(&printSync);
			}
		}

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: Registers Bank being accessed \n",PC);
			printf("     A has received %d \n", A);
			printf("     B has received %d \n", B);
			sem_post(&printSync);
		}

		sem_post(&registers_updated_0);
		sem_post(&registers_updated_1);
		sem_post(&mux_WriteDataIR_free);
		sem_post(&mux_WriteRegIR_free);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		signExtend
*	FUNÇÃO:		Thread do componente signExtend com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *signExtend(void *thread_id) {
	while (1) {

		sem_wait(&IR_0_updated);
		sem_wait(&signExtend_free);

		signExtend_output = (int) (IR & 0x0000FFFF);
		if (IR & 0x00008000) signExtend_output |= 0xFFFF0000;

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: Sign Extend has operated \n", PC);
			printf("    resulting in %#.8x \n", signExtend_output);
			sem_post(&printSync);
		}

		sem_post(&signExtend_updated);
		sem_post(&IR_0_free);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		shiftLeft2_muxPC
*	FUNÇÃO:		Thread do componente shiftLeft2_muxPC com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *shiftLeft2_muxPC(void *thread_id) {
	while (1) {
		sem_wait(&IR_1_updated);
		sem_wait(&shiftLeft2_muxPC_free);

		ssl_muxPC_output = (IR&0x03FFFFFF) << 2;

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: Shift Left 2 at mux PC has operated \n", PC);
			sem_post(&printSync);
		}

		sem_post(&shiftLeft2_muxPC_updated);
		sem_post(&IR_1_free);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		shiftLeft2_muxALUB
*	FUNÇÃO:		Thread do componente shiftLeft2_muxALUB com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *shiftLeft2_muxALUB(void *thread_id) {
	while (1) {
		sem_wait(&signExtend_updated);
		sem_wait(&shiftLeft2_muxALUB_free);

		ssl_ALUB_output = signExtend_output << 2;

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: Shift Left 2 at mux ALUB has operated \n", PC);
			sem_post(&printSync);
		}

		sem_post(&shiftLeft2_muxALUB_updated);
		sem_post(&signExtend_free);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		mux_ALUA
*	FUNÇÃO:		Thread do componente mux_ALUA com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *mux_ALUA(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_ALUA);
		sem_wait(&mux_ALUA_free);
		sem_wait(&PC_updated);
		sem_wait(&registers_updated_0);

		if (UC.job.controlSignals.ALUSrcA == 0) {
			mux_ALUA_output = PC;
		}
		else if (UC.job.controlSignals.ALUSrcA == 1) {
			mux_ALUA_output = A;
		}

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: Buffer A from ALU has received the data from the Registers \n", PC);
			sem_post(&printSync);
		}

		sem_post(&registers_free_0);
		sem_post(&PC_free);
		sem_post(&mux_ALUA_updated);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		mux_ALUB
*	FUNÇÃO:		Thread do componente mux_ALUB com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *mux_ALUB(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_ALUB);
		sem_wait(&registers_updated_1);
		sem_wait(&shiftLeft2_muxALUB_updated);
		sem_wait(&mux_ALUB_free);

		if (UC.job.controlSignals.ALUSrcB0 == 0
				&& UC.job.controlSignals.ALUSrcB1 == 0) {
			mux_ALUB_output = B;
		}

		if (UC.job.controlSignals.ALUSrcB0 == 0
				&& UC.job.controlSignals.ALUSrcB1 == 1) {
			mux_ALUB_output = 4;
		}

		else if (UC.job.controlSignals.ALUSrcB0 == 1
				&& UC.job.controlSignals.ALUSrcB1 == 0) {
			mux_ALUB_output = signExtend_output;
		}

		else if (UC.job.controlSignals.ALUSrcB0 == 1
				&& UC.job.controlSignals.ALUSrcB1 == 1) {
			mux_ALUB_output = ssl_ALUB_output;
		}

		if (debugMode) {
			sem_wait(&printSync);
			printf("%d: Buffer B from ALU has received the data from the Registers \n", PC);
			sem_post(&printSync);
		}

		sem_post(&mux_ALUB_updated);
		sem_post(&shiftLeft2_muxALUB_free);
		sem_post(&registers_free_1);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		ALUControl
*	FUNÇÃO:		Thread do componente ALUControl com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *ALUControl (void *thread_id){
	
	while (1){
		
		/* esperamos o IR ficar pronto e logo após liberamos o controle da ULA */
		sem_wait(&IR_2_updated);
		sem_wait(&ALUControl_free);

		// se ALUOP = 00, então é uma LW / SW 
		if (UC.job.controlSignals.ALUOp0 == 0
				&& UC.job.controlSignals.ALUOp1 == 0) {
			// código de operação para soma
			ALUControl_output = 2;
		}	

		//se ALUOP = 01, então é uma BEQ
		if (UC.job.controlSignals.ALUOp0 == 0
				&& UC.job.controlSignals.ALUOp1 == 1) {
			// código da operação para subtração
			ALUControl_output = 6;
		}

		//se ALUOP = 10, então é uma instrução tipo-R
		if (UC.job.controlSignals.ALUOp0 == 1
				&& UC.job.controlSignals.ALUOp1 == 0) {

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

/*******************************************************************************
*	NOME:		ALU
*	FUNÇÃO:		Thread do componente ALU com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
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
				printf("%d: ALU is summing up %d and %d \n", PC, mux_ALUA_output,mux_ALUB_output);
				sem_post(&printSync);
			}
		}

		//se o código de operação for 6 é uma subtração
		if (ALUControl_output == 6) {
			ALU_output = mux_ALUA_output - mux_ALUB_output;

			if (debugMode) {
				sem_wait(&printSync);
				printf("%d: ALU is subtracting up %d and %d \n", PC, mux_ALUA_output,mux_ALUB_output);
				sem_post(&printSync);
			}
		}

		// se o código de operação for 0 é um AND
		if (ALUControl_output == 0) {
			ALU_output = mux_ALUA_output & mux_ALUB_output;
			if (debugMode) {
				sem_wait(&printSync);
				printf("%d: ALU is operating logical AND %d and %d \n", PC, mux_ALUA_output,mux_ALUB_output);
				sem_post(&printSync);
			}
		}

		// se o código de operação for 1 é um OR
		if (ALUControl_output == 1) {
			ALU_output = mux_ALUA_output | mux_ALUB_output;
			if (debugMode) {
				sem_wait(&printSync);
				printf("%d: ALU is operating logical OR %d and %d \n", PC, mux_ALUA_output,mux_ALUB_output);
				sem_post(&printSync);
			}
		}

		// se o código de operação for 7 é um SLT
		if (ALUControl_output == 7) {
			ALU_output =  mux_ALUA_output < mux_ALUB_output ? 1 : 0;
			if (debugMode) {
				sem_wait(&printSync);
				printf("%d: ALU is operating logical SLT %d and %d \n", PC, mux_ALUA_output,mux_ALUB_output);
				sem_post(&printSync);
			}
		}

		if (ALU_output == 0) ALU_zero_output = 1;
		else ALU_zero_output = 0;

		sem_post(&ALU_updated);
		sem_post(&ALUControl_free);
		sem_post(&mux_ALUA_free);
		sem_post(&mux_ALUB_free);

	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		mux_PC
*	FUNÇÃO:		Thread do componente mux_PC com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *mux_PC(void *thread_id) {
	while (1) {
		sem_wait(&UC_mux_PC);
		sem_wait(&mux_PC_free);
		sem_wait(&ALU_updated);
		sem_wait(&shiftLeft2_muxPC_updated);


		if (UC.job.controlSignals.PCSource0 == 0
				&& UC.job.controlSignals.PCSource1 == 0) {
			mux_PC_output = ALU_output;
		}

		if (UC.job.controlSignals.PCSource0 == 0
				&& UC.job.controlSignals.PCSource1 == 1) {
			mux_PC_output = AluOut;
		}

		if (UC.job.controlSignals.PCSource0 == 1
				&& UC.job.controlSignals.PCSource1 == 0) {
			mux_PC_output = (0x0FFFFFFF&ssl_muxPC_output)|(0xF0000000&PC);
		}

		if (debugMode) {
			sem_wait(&printSync);
			printf("Cycle finished!\n\n");
			sem_post(&printSync);
		}


		sem_post(&shiftLeft2_muxPC_free);
		sem_post(&ALU_free);
		sem_post(&mux_PC_updated);
	}
	pthread_exit(0);
}

/*******************************************************************************
*	NOME:		ports_PC
*	FUNÇÃO:		Thread do componente mux_PC com toda as suas operações
*
*			Tipo					Descrição
*     			--------			-----------
*			thread_id*				Identificação da thread
*
*	RETORNO:	void
*******************************************************************************/
void *ports_PC(void *thread_id) {
	while(1){
		sem_wait(&mux_PC_updated);
		sem_wait(&PC_free);
		sem_wait(&PC_free);

		if ((ALU_zero_output && UC.job.controlSignals.PCWriteCond) || UC.job.controlSignals.PCWrite){
			PC = mux_PC_output;
			if (debugMode){
				sem_wait(&printSync);
				printf("PC updated to %d !! =) \n\n", PC);
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

/*******************************************************************************
*	NOME:		semaphores_init
*	FUNÇÃO:		Inicialização dos semáforos
*
*	RETORNO:	void
*******************************************************************************/
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

/*******************************************************************************
*	NOME:		resourcesInir
*	FUNÇÃO:		Inicialização de todos so recursos (semáforos e threads)
*
*	RETORNO:	void
*******************************************************************************/
void resourcesInit() {
	semaphores_init();

	if (pthread_create(&memory_handle, 0, memory_load, NULL) != 0) {
		printf(THREAD_INIT_FAIL("Memory Load"));
		exit(0);
	}
	if (pthread_create(&clockedMemory_handle, 0, clockedMemoryAccess, NULL)
			!= 0) {
		printf(THREAD_INIT_FAIL("Clocked Memory"));
		exit(0);
	}
	if (pthread_create(&instructionRegister_handle, 0, instructionRegister,
	NULL) != 0) {
		printf(THREAD_INIT_FAIL("Instruction Register"));
		exit(0);
	}
	if (pthread_create(&mux_memoryAdress_handle, 0, mux_memoryAdress, NULL)
			!= 0) {
		printf(THREAD_INIT_FAIL("MuxMemoryAddress"));
		exit(0);
	}
	if (pthread_create(&mux_WriteRegIR_handle, 0, mux_WriteRegIR, NULL) != 0) {
		printf(THREAD_INIT_FAIL("MuxWriteRegIR"));
		exit(0);
	}
	if (pthread_create(&mux_WriteDataIR_handle, 0, mux_WriteDataIR, NULL)
			!= 0) {
		printf(THREAD_INIT_FAIL("MuxWriteDataIR"));
		exit(0);
	}
	if (pthread_create(&signExtend_handle, 0, signExtend, NULL) != 0) {
		printf(THREAD_INIT_FAIL("MuxSign Extend"));
		exit(0);
	}
	if (pthread_create(&shiftLeft2_muxPC_handle, 0, shiftLeft2_muxPC, NULL)
			!= 0) {
		printf(THREAD_INIT_FAIL("Shiftleft2 at Mux PC"));
		exit(0);
	}
	if (pthread_create(&shiftLeft2_muxPC_handle, 0, shiftLeft2_muxALUB, NULL)
			!= 0) {
		printf(THREAD_INIT_FAIL("Shiftleft2 at Mux ALUB"));
		exit(0);
	}
	if (pthread_create(&registers_handle, 0, registers, NULL) != 0) {
		printf(THREAD_INIT_FAIL("Registers"));
		exit(0);
	}
	if (pthread_create(&mux_ALUA_handle, 0, mux_ALUA, NULL) != 0) {
		printf(THREAD_INIT_FAIL("MuxALUA"));
		exit(0);
	}
	if (pthread_create(&ALUControl_handle, 0, ALUControl, NULL) != 0) {
		printf(THREAD_INIT_FAIL("ALUControl"));
		exit(0);
	}
	if (pthread_create(&ALU_handle, 0, ALU, NULL) != 0) {
		printf(THREAD_INIT_FAIL("ALU"));
		exit(0);
	}
	if (pthread_create(&mux_ALUB_handle, 0, mux_ALUB, NULL) != 0) {
		printf(THREAD_INIT_FAIL("MuxALUB"));
		exit(0);
	}
	if (pthread_create(&mux_PC_handle, 0, mux_PC, NULL) != 0) {
		printf(THREAD_INIT_FAIL("MuxPC"));
		exit(0);
	}
	if (pthread_create(&ports_PC_handle, 0, ports_PC, NULL) != 0) {
		printf(THREAD_INIT_FAIL("AND and OR for PC"));
		exit(0);
	}

}
