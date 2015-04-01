/*
    Trabalho desenvolvido para a disciplina SSC0610 - Organiza��o de Computadores Digitais I,
    ministrada pelo professor Paulo S�rgio Lopes de Souza, onde implementamos um simulador de
    processador mips de 32 bits.

    Membros participantes, em ordem de contribui��o para o c�digo:
    - Vitor Pinto Ribeiro (7961005)
    - Victor Palombo Silvano (7961012)
    - Arthur Evanil Zanelatto e Silva (7232756)
    .
    .
    - Wesley Vieira (7593968)
*/

//Fun��es utilizadas pela Unidade de Controle (UC), para setar bits de controle, em fun��o do estado atual.
void desc_controle();
void desc_desp1(int IR);
void desc_desp2(int IR);
short int desc_sc();

//Vari�veis utilizadas pela Unidade L�gica e Aritm�tica (ULA) em suas fun��es internas.
int resultado_ula;
char zero;
char overflow;

//Vari�veis utilizadas pela UC, para que, em fun��o destas, sejam setados os bits de controle
//necess�rios para ativar a fun��o desejada a cada ciclo.
char s;
char ctrl;

//Fun��o que representa a UC do processador, que, em fun��o do estado atual, seta o pr�ximo estado
//e os bits de controle respectivos
void UnidadeControle(int IR, short int* sc)
{
    //Condi��o de t�rmino do programa (memoria[PC]==0)
    if(IR==(int)0x00000000)
        loop = 0x00;

    else
    {
        //Condi��o de in�cio de programa
        if(IR == -1)
            s = 0x00;

        //Caso o programa j� esteja em execu��o, s�o utilizadas fun��es para atribuir um pr�ximo estado,
        //em fun��o do atual
        else
        {
            //Fun��o que seta o gerenciamento de estados, de acordo com o estado atual e o campo de
            //fun��o da instru��o em quest�o
            desc_controle();

            //Se o controle retornar 0, � dado prosseguimento sequencial ao estado
            if(ctrl == (char)0x00)
                s = s+1;

            //Caso o controle retorne 1, � necess�rio consultar a tabela de despacho 1 para, em fun��o
            //do campo de fun��o da instru��o, direcionar o programa para o pr�ximo estado adequado
            else if(ctrl == (char)0x01)
                desc_desp1(IR & 0xfc000000);

            //Caso o controle retorne 2, � necess�rio consultar a tabela de despacho 2 para, em fun��o
            //do campo de fun��o da instru��o, direcionar o programa para o pr�ximo estado adequado
            else if(ctrl == (char)0x02)
                desc_desp2(IR & 0xfc000000);

            //Caso o controle retorne 3, o pr�ximo estado ser�, obrigatoriamente, o de busca de nova instru��o
            else if(ctrl == (char)0x03)
                s = 0x00;
        }

        //Fun��o utilizada para que, em fun��o do pr�ximo estado escolhido acima, os 16 bits de controle sejam setados
        //de maneira a permitir o programa realizar t�o somente a(s) opera��o(�es) necess�ria(s) para tal estado.
        *sc = desc_sc();
    }
};

//Fun��o de busca de instru��o
void Busca_Instrucao(short int sc, int PC, int ALUOUT, int IR, int* PCnew, int* IRnew, int* MDRnew)
{
    //Caso o estado ativo seja o 0 (com os bits de controle setados na UC)
    if (sc == (short int)0x9410)
        {
            //IR recebe a fun��o apontada por PC.
            *IRnew = memoria[PC];

            //N�o h� bit de controle sobre essa estrutura.
            *MDRnew = memoria[PC];

            //ALUOUT = PC + unidade de mem�ria
            ula(PC, 1, 0x02, &resultado_ula, &zero, &overflow);
            ALUOUT = resultado_ula;

            //PC recebe ALUOUT
            *PCnew = ALUOUT;
        }
};

//Fun��o de decodifica��o de fun��o, realizada na UC, decodifica��o de registradores requisitados para as fun��es,
//atribui para vari�veis "tempor�rias" e c�lculo especulativo de endere�o, em fun��o dos �ltimos 16 bits da instru��o
void Decodifica_BuscaRegistrador(short int sc, int IR, int PC, int A, int B, int* Anew, int* Bnew, int* ALUOUTnew)
{
    //Caso o estado ativo seja o 1 (com os bits de controle setados na UC)
    if (sc == (short int)0x0018)
    {
        //Atribui��o dos valores para os operandos
        *Anew = reg[(IR & 0x03e00000) >> 21];
        *Bnew = reg[(IR & 0x001f0000) >> 16];

        //C�lculo especulativo de endere�o, caso a opera��o seja uma "Branch if Equal" (beq)
        ula(PC, (IR & 0x0000ffff), 0x02, &resultado_ula, &zero, &overflow);
        *ALUOUTnew = resultado_ula;
    }
};

//Execu��o da fun��o requisitada, em fun��o do campo de fun��o e do estado, setados na UC
void Execucao_CalcEnd_Desvio(short int sc, int A, int B, int IR, int PC, int ALUOUT, int* ALUOUTnew, int* PCnew)
{
    //Caso o estado ativo seja o 6 (Instru��o do tipo-R)
    if(sc == (short int)0x0024)
    {
        //Caso o campo de opera��o corresponda � opera��o de soma
        if((IR & 0x0000003f) == 0x00000020)
        {
            ula(A, B, 0x02, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }

        //Caso o campo de opera��o corresponda � opera��o de AND
        else if((IR & 0x0000003f) == 0x00000024)
        {
            ula(A, B, 0x00, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }

        //Caso o campo de opera��o corresponda � opera��o de OR
        else if((IR & 0x0000003f) == 0x00000025)
        {
            ula(A, B, 0x01, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }

        //Caso o campo de opera��o corresponda � opera��o de subtra��o
        else if((IR & 0x0000003f) == 0x00000022)
        {
            ula(A, B, 0x06, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }

        //Caso o campo de opera��o corresponda � opera��o de Set on Less Than (slt)
        else if((IR & 0x0000003f) == 0x0000002a)
        {
            ula(A, B, 0x07, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }
    }

    //Caso o estado ativo seja o 2 (Load Word [lw] ou Store Word [sw])
    else if (sc == (short int)0x000c)
    {
        //C�lculo do endere�o, em fun��o do registrador RS e do offset, ambos presentes em posi��es
        //espec�ficas da instru��o
        ula((IR & 0x03e00000) >> 21, IR & 0x0000ffff, 0x02, &resultado_ula, &zero, &overflow);
        *ALUOUTnew = resultado_ula;
    }

    //Caso o estado ativo seja o 8 (beq)
    else if (sc == (short int)0x0344)
    {
        //Opera��o de subtra��o, para verificar se o valor dos registradores s�o iguais
        ula(A, B, 0x06, &resultado_ula, &zero, &overflow);
        *ALUOUTnew = resultado_ula;

        //Caso forem, PC recebe o valor calculado especulativamente no estado 1
        if(zero == (char)0x01)
            *PCnew = ALUOUT;
    }

    //Caso o estado ativo seja o 9 (Jump[j])
    else if(sc == (short int)0x0480)
        //PC recebe a concatena��o dos �ltimos 4 bits de PC com o offset presente na instru��o, multiplicado por 4
        *PCnew = (PC & 0xf0000000)|((IR & 0x03ffffff) << 2);
};

//Escrita do resultado da opera��o l�gica requisitada ou acesso � mem�ria
void EscreveTipoR_AcessaMemoria(short int sc, int B, int IR, int ALUOUT, int PC, int* MDRnew, int* IRnew)
{
    //Caso o estado ativo seja o 7 (continua��o da fun��o do tipo-R)
    if(sc == (short int)0x0003)
        //Registrador identificado pelos bits 11~15 da instru��o, deslocados, recebem o resultado da opera��o realizada
        //anteriormente
        reg[(IR&0x0000f800) >> 11] = ALUOUT;

    //Caso o estado ativo seja o 3 (lw exclusivamente)
    else if(sc == (short int)0x1800)
        //Leitura da posi��o de mem�ria requisitada pelo c�lculo (reg+offset), realizado anteriormente
        //dividido por 4 (tamanho da palavra de bits)
        *MDRnew = memoria[reg[ALUOUT]>>2];

    //Caso o estado ativo seja o 5 (sw exclusivamente)
    else if(sc == (short int)0x2800)
        //Salva, na posi��o de mem�ria requisitada pelo c�lculo (reg+offset), realizado anteriormente
        //o conte�do do registrador RT
        memoria[reg[ALUOUT]>>2] = B;
};

//Escrita de dados na mem�ria, exclusivo de lw
void EscreveRefMem(short int sc, int IR, int MDR, int ALUOUT)
{
    //Caso o estado ativo seja o 4 (�ltimo passo de lw)
    if(sc == (short int)0x4002)
        //registrador na posi��o apontada por RT recebe o conte�do de mem�ria requisitado
        reg[(IR & 0x001f0000) >> 16] = MDR;
};

//Unidade L�gica e Aritm�tica
int ula(int A, int B, char OP, int *resultado_ula, char *zero, char *overflow)
{
    //Caso a opera��o requisitada seja uma soma
    if(OP == (char)0x02)
    {
        *resultado_ula = A+B;

        //Cuidado com o overflow
        if ((A>0 && B>0 && (A+B)<0)||(A<0 && B<0 && (A+B)>0))
            *overflow = 0x01;
    }

    //Caso a opera��o requisitada seja uma subtra��o
    else if(OP == (char)0x06)
    {
        *resultado_ula = A-B;

        //Cuidado com o overflow
        if((A<0 && B>0 && (A-B)>0)||(A>0 && B<0 && (A-B)<0))
            *overflow = 0x01;
    }

    //Caso a opera��o requisitada seja slt
    else if(OP == (char)0x07)
    {
        *resultado_ula = A-B;

        //Se A<B, retorna 1
        if(*resultado_ula < 0)
            *resultado_ula = 1;

        //Caso contr�rio, retorna 0
        else
            *resultado_ula = 0;
    }

    //Caso a opera��o requisitada seja o AND l�gico
    else if (OP == (char)0x00)
        *resultado_ula = (A&B);

    //Caso a opera��o requisitada seja o OR l�gico
    else if (OP == (char)0x01)
        *resultado_ula = (A|B);

    //Caso qualquer resultado seja 0, o bit zero � ativado
    if(*resultado_ula==0)
        *zero = 0x01;

    //Caso contr�rio, o bit zero � desativado
    else
        *zero = 0x00;

    //Exig�ncia da fun��o, por ser proposta com retorno de int
    return 0;
};

//Fun��o que, em fun��o do estado, seta bits que ir�o direcionar os pr�ximos estados na ULA
void desc_controle()
{
    //0x00 - Prosseguimento sequencial entre estados
    //0x01 - Necess�rio consultar "tabela de despacho 1"
    //0x02 - Necess�rio consultar "tabela de despacho 2"
    //0x03 - Volta para busca de nova instru��o

    //Se o estado for 0, � dado prosseguimento sequencial ao estado (busca -> decodifica��o/atribui��o de valores)
    if (s == 0x00)
        ctrl = 0x00;

    //Se o estado for 1, � necess�rio consultar tabela de despacho 1 (lw/sw, tipo-r, beq ou j)
    else if (s == 0x01)
        ctrl = 0x01;

    //Se o estado for 2, � necess�rio consultar tabela de despacho 2 (lw ou sw)
    else if (s == 0x02)
        ctrl = 0x02;

    //Se o estado for 3, � dado prosseguimento sequencial aos estados (4o ciclo da lw -> 5o ciclo da lw)
    else if (s == 0x03)
        ctrl = 0x00;

    //Se o estado for 4, � necess�rio buscar nova instru��o (lw finalizado)
    else if (s == 0x04)
        ctrl = 0x03;

    //Se o estado for 5, � necess�rio buscar nova instru��o (sw finalizado)
    else if (s == 0x05)
        ctrl = 0x03;

    //Se o estado for 6, � dado prosseguimento sequencial aos estados (execu�ao de tipo-r -> armazenamento do resultado)
    else if (s == 0x06)
        ctrl = 0x00;

    //Se o estado for 7, � necess�rio buscar nova instru��o (tipo-r finalizada)
    else if (s == 0x07)
        ctrl = 0x03;

    //Se o estado for 8, � necess�rio buscar nova instru��o (beq finalizado)
    else if (s == 0x08)
        ctrl = 0x03;

    //Se o estado for 9, � necess�rio buscar nova instru��o (j finalizado)
    else if (s == 0x09)
        ctrl = 0x03;
};

//"Tabela de despacho 1", que seta o estado de acordo com o campo de fun��o da instru��o atual
void desc_desp1(int IR)
{
    //Caso o campo de fun��o acuse uma instru��o do tipo-r
    if(IR == 0x00000000)
        s = 0x06;

    //Caso o campo de fun��o acuse uma instru��o beq
    else if(IR == 0x10000000)
        s = 0x08;

    //Caso o campo de fun��o acuse uma instru��o j
    else if (IR == 0x08000000)
        s = 0x09;

    //Caso o campo de fun��o acuse uma instru��o lw/sw
    else if(IR == 0x8c000000 || IR == 0xac000000)
        s = 0x02;
};

//"Tabela de despacho 2", que seta o estado de acordo com o campo de fun��o da instru��o atual
void desc_desp2(int IR)
{
    //Caso o campo de fun��o acuse uma instru��o lw
    if(IR == 0x8c000000)
        s = 0x03;

    //Caso o campo de fun��o acuse uma instru��o sw
    else if(IR == 0xac000000)
        s = 0x05;
};

//Fun��o que retorna os bits de controle necess�rios para realizar cada opera��o, de acordo com o estado
//Os bits "don't care" s�o setados para 0, para facilitar a l�gica
short int desc_sc()
{
    //Ativa MemRead, IRWrite, ALUSrcB1 e PCWrite
    //1001010000010000 == 0x9410
    if (s == (char)0x00)
        return 0x9410;

    //Ativa ALUSrcB0 e ALUSrcB1
    //0000000000011000 == 0x0018
    else if (s == (char)0x01)
        return 0x0018;

    //Ativa ALUSrcA e ALUSrcB0
    //0000000000001100 == 0x000c
    else if (s == (char)0x02)
        return 0x000c;

    //Ativa MemRead e IorD
    //0001100000000000 == 0x1800
    else if (s == (char)0x03)
        return 0x1800;

    //Ativa RegWrite e MemtoReg
    //0100000000000010 == 0x4002
    else if (s == (char)0x04)
        return 0x4002;

    //Ativa MemWrite e IorD
    //0010100000000000 == 0x2800
    else if (s == (char)0x05)
        return 0x2800;

    //Ativa ALUSrcA e ALUSrcOP0
    //0000000000100100 == 0x0024
    else if (s == (char)0x06)
        return 0x0024;

    //Ativa RegDst e RegWrite
    //0000000000000011 == 0x0003
    else if (s == (char)0x07)
        return 0x0003;

    //Ativa ALUSrcA, ALUOp1, PCWriteCond e PCSource1
    //0000001101000100 == 0x0344
    else if (s == (char)0x08)
        return 0x0344;

    //Ativa PCWrite e PCSource0
    //0000010010000000 == 0x0480
    else if (s == (char)0x09)
        return 0x0480;
};

/*
    Considera��o final:

    - Os castings dentro das estruturas de controle foram necess�rios para impedir a extens�o de sinal
        e a extens�o dos bits por parte do computador/compilador (n�o temos certeza de quem � a culpa)

*/
