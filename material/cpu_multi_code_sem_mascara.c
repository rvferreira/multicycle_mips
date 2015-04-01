/*
    Trabalho desenvolvido para a disciplina SSC0610 - Organização de Computadores Digitais I,
    ministrada pelo professor Paulo Sérgio Lopes de Souza, onde implementamos um simulador de
    processador mips de 32 bits.

    Membros participantes, em ordem de contribuição para o código:
    - Vitor Pinto Ribeiro (7961005)
    - Victor Palombo Silvano (7961012)
    - Arthur Evanil Zanelatto e Silva (7232756)
    .
    .
    - Wesley Vieira (7593968)
*/

//Funções utilizadas pela Unidade de Controle (UC), para setar bits de controle, em função do estado atual.
void desc_controle();
void desc_desp1(int IR);
void desc_desp2(int IR);
short int desc_sc();

//Variáveis utilizadas pela Unidade Lógica e Aritmética (ULA) em suas funções internas.
int resultado_ula;
char zero;
char overflow;

//Variáveis utilizadas pela UC, para que, em função destas, sejam setados os bits de controle
//necessários para ativar a função desejada a cada ciclo.
char s;
char ctrl;

//Função que representa a UC do processador, que, em função do estado atual, seta o próximo estado
//e os bits de controle respectivos
void UnidadeControle(int IR, short int* sc)
{
    //Condição de término do programa (memoria[PC]==0)
    if(IR==(int)0x00000000)
        loop = 0x00;

    else
    {
        //Condição de início de programa
        if(IR == -1)
            s = 0x00;

        //Caso o programa já esteja em execução, são utilizadas funções para atribuir um próximo estado,
        //em função do atual
        else
        {
            //Função que seta o gerenciamento de estados, de acordo com o estado atual e o campo de
            //função da instrução em questão
            desc_controle();

            //Se o controle retornar 0, é dado prosseguimento sequencial ao estado
            if(ctrl == (char)0x00)
                s = s+1;

            //Caso o controle retorne 1, é necessário consultar a tabela de despacho 1 para, em função
            //do campo de função da instrução, direcionar o programa para o próximo estado adequado
            else if(ctrl == (char)0x01)
                desc_desp1(IR & 0xfc000000);

            //Caso o controle retorne 2, é necessário consultar a tabela de despacho 2 para, em função
            //do campo de função da instrução, direcionar o programa para o próximo estado adequado
            else if(ctrl == (char)0x02)
                desc_desp2(IR & 0xfc000000);

            //Caso o controle retorne 3, o próximo estado será, obrigatoriamente, o de busca de nova instrução
            else if(ctrl == (char)0x03)
                s = 0x00;
        }

        //Função utilizada para que, em função do próximo estado escolhido acima, os 16 bits de controle sejam setados
        //de maneira a permitir o programa realizar tão somente a(s) operação(ões) necessária(s) para tal estado.
        *sc = desc_sc();
    }
};

//Função de busca de instrução
void Busca_Instrucao(short int sc, int PC, int ALUOUT, int IR, int* PCnew, int* IRnew, int* MDRnew)
{
    //Caso o estado ativo seja o 0 (com os bits de controle setados na UC)
    if (sc == (short int)0x9410)
        {
            //IR recebe a função apontada por PC.
            *IRnew = memoria[PC];

            //Não há bit de controle sobre essa estrutura.
            *MDRnew = memoria[PC];

            //ALUOUT = PC + unidade de memória
            ula(PC, 1, 0x02, &resultado_ula, &zero, &overflow);
            ALUOUT = resultado_ula;

            //PC recebe ALUOUT
            *PCnew = ALUOUT;
        }
};

//Função de decodificação de função, realizada na UC, decodificação de registradores requisitados para as funções,
//atribui para variáveis "temporárias" e cálculo especulativo de endereço, em função dos últimos 16 bits da instrução
void Decodifica_BuscaRegistrador(short int sc, int IR, int PC, int A, int B, int* Anew, int* Bnew, int* ALUOUTnew)
{
    //Caso o estado ativo seja o 1 (com os bits de controle setados na UC)
    if (sc == (short int)0x0018)
    {
        //Atribuição dos valores para os operandos
        *Anew = reg[(IR & 0x03e00000) >> 21];
        *Bnew = reg[(IR & 0x001f0000) >> 16];

        //Cálculo especulativo de endereço, caso a operação seja uma "Branch if Equal" (beq)
        ula(PC, (IR & 0x0000ffff), 0x02, &resultado_ula, &zero, &overflow);
        *ALUOUTnew = resultado_ula;
    }
};

//Execução da função requisitada, em função do campo de função e do estado, setados na UC
void Execucao_CalcEnd_Desvio(short int sc, int A, int B, int IR, int PC, int ALUOUT, int* ALUOUTnew, int* PCnew)
{
    //Caso o estado ativo seja o 6 (Instrução do tipo-R)
    if(sc == (short int)0x0024)
    {
        //Caso o campo de operação corresponda à operação de soma
        if((IR & 0x0000003f) == 0x00000020)
        {
            ula(A, B, 0x02, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }

        //Caso o campo de operação corresponda à operação de AND
        else if((IR & 0x0000003f) == 0x00000024)
        {
            ula(A, B, 0x00, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }

        //Caso o campo de operação corresponda à operação de OR
        else if((IR & 0x0000003f) == 0x00000025)
        {
            ula(A, B, 0x01, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }

        //Caso o campo de operação corresponda à operação de subtração
        else if((IR & 0x0000003f) == 0x00000022)
        {
            ula(A, B, 0x06, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }

        //Caso o campo de operação corresponda à operação de Set on Less Than (slt)
        else if((IR & 0x0000003f) == 0x0000002a)
        {
            ula(A, B, 0x07, &resultado_ula, &zero, &overflow);
            *ALUOUTnew = resultado_ula;
        }
    }

    //Caso o estado ativo seja o 2 (Load Word [lw] ou Store Word [sw])
    else if (sc == (short int)0x000c)
    {
        //Cálculo do endereço, em função do registrador RS e do offset, ambos presentes em posições
        //específicas da instrução
        ula((IR & 0x03e00000) >> 21, IR & 0x0000ffff, 0x02, &resultado_ula, &zero, &overflow);
        *ALUOUTnew = resultado_ula;
    }

    //Caso o estado ativo seja o 8 (beq)
    else if (sc == (short int)0x0344)
    {
        //Operação de subtração, para verificar se o valor dos registradores são iguais
        ula(A, B, 0x06, &resultado_ula, &zero, &overflow);
        *ALUOUTnew = resultado_ula;

        //Caso forem, PC recebe o valor calculado especulativamente no estado 1
        if(zero == (char)0x01)
            *PCnew = ALUOUT;
    }

    //Caso o estado ativo seja o 9 (Jump[j])
    else if(sc == (short int)0x0480)
        //PC recebe a concatenação dos últimos 4 bits de PC com o offset presente na instrução, multiplicado por 4
        *PCnew = (PC & 0xf0000000)|((IR & 0x03ffffff) << 2);
};

//Escrita do resultado da operação lógica requisitada ou acesso à memória
void EscreveTipoR_AcessaMemoria(short int sc, int B, int IR, int ALUOUT, int PC, int* MDRnew, int* IRnew)
{
    //Caso o estado ativo seja o 7 (continuação da função do tipo-R)
    if(sc == (short int)0x0003)
        //Registrador identificado pelos bits 11~15 da instrução, deslocados, recebem o resultado da operação realizada
        //anteriormente
        reg[(IR&0x0000f800) >> 11] = ALUOUT;

    //Caso o estado ativo seja o 3 (lw exclusivamente)
    else if(sc == (short int)0x1800)
        //Leitura da posição de memória requisitada pelo cálculo (reg+offset), realizado anteriormente
        //dividido por 4 (tamanho da palavra de bits)
        *MDRnew = memoria[reg[ALUOUT]>>2];

    //Caso o estado ativo seja o 5 (sw exclusivamente)
    else if(sc == (short int)0x2800)
        //Salva, na posição de memória requisitada pelo cálculo (reg+offset), realizado anteriormente
        //o conteúdo do registrador RT
        memoria[reg[ALUOUT]>>2] = B;
};

//Escrita de dados na memória, exclusivo de lw
void EscreveRefMem(short int sc, int IR, int MDR, int ALUOUT)
{
    //Caso o estado ativo seja o 4 (último passo de lw)
    if(sc == (short int)0x4002)
        //registrador na posição apontada por RT recebe o conteúdo de memória requisitado
        reg[(IR & 0x001f0000) >> 16] = MDR;
};

//Unidade Lógica e Aritmética
int ula(int A, int B, char OP, int *resultado_ula, char *zero, char *overflow)
{
    //Caso a operação requisitada seja uma soma
    if(OP == (char)0x02)
    {
        *resultado_ula = A+B;

        //Cuidado com o overflow
        if ((A>0 && B>0 && (A+B)<0)||(A<0 && B<0 && (A+B)>0))
            *overflow = 0x01;
    }

    //Caso a operação requisitada seja uma subtração
    else if(OP == (char)0x06)
    {
        *resultado_ula = A-B;

        //Cuidado com o overflow
        if((A<0 && B>0 && (A-B)>0)||(A>0 && B<0 && (A-B)<0))
            *overflow = 0x01;
    }

    //Caso a operação requisitada seja slt
    else if(OP == (char)0x07)
    {
        *resultado_ula = A-B;

        //Se A<B, retorna 1
        if(*resultado_ula < 0)
            *resultado_ula = 1;

        //Caso contrário, retorna 0
        else
            *resultado_ula = 0;
    }

    //Caso a operação requisitada seja o AND lógico
    else if (OP == (char)0x00)
        *resultado_ula = (A&B);

    //Caso a operação requisitada seja o OR lógico
    else if (OP == (char)0x01)
        *resultado_ula = (A|B);

    //Caso qualquer resultado seja 0, o bit zero é ativado
    if(*resultado_ula==0)
        *zero = 0x01;

    //Caso contrário, o bit zero é desativado
    else
        *zero = 0x00;

    //Exigência da função, por ser proposta com retorno de int
    return 0;
};

//Função que, em função do estado, seta bits que irão direcionar os próximos estados na ULA
void desc_controle()
{
    //0x00 - Prosseguimento sequencial entre estados
    //0x01 - Necessário consultar "tabela de despacho 1"
    //0x02 - Necessário consultar "tabela de despacho 2"
    //0x03 - Volta para busca de nova instrução

    //Se o estado for 0, é dado prosseguimento sequencial ao estado (busca -> decodificação/atribuição de valores)
    if (s == 0x00)
        ctrl = 0x00;

    //Se o estado for 1, é necessário consultar tabela de despacho 1 (lw/sw, tipo-r, beq ou j)
    else if (s == 0x01)
        ctrl = 0x01;

    //Se o estado for 2, é necessário consultar tabela de despacho 2 (lw ou sw)
    else if (s == 0x02)
        ctrl = 0x02;

    //Se o estado for 3, é dado prosseguimento sequencial aos estados (4o ciclo da lw -> 5o ciclo da lw)
    else if (s == 0x03)
        ctrl = 0x00;

    //Se o estado for 4, é necessário buscar nova instrução (lw finalizado)
    else if (s == 0x04)
        ctrl = 0x03;

    //Se o estado for 5, é necessário buscar nova instrução (sw finalizado)
    else if (s == 0x05)
        ctrl = 0x03;

    //Se o estado for 6, é dado prosseguimento sequencial aos estados (execuçao de tipo-r -> armazenamento do resultado)
    else if (s == 0x06)
        ctrl = 0x00;

    //Se o estado for 7, é necessário buscar nova instrução (tipo-r finalizada)
    else if (s == 0x07)
        ctrl = 0x03;

    //Se o estado for 8, é necessário buscar nova instrução (beq finalizado)
    else if (s == 0x08)
        ctrl = 0x03;

    //Se o estado for 9, é necessário buscar nova instrução (j finalizado)
    else if (s == 0x09)
        ctrl = 0x03;
};

//"Tabela de despacho 1", que seta o estado de acordo com o campo de função da instrução atual
void desc_desp1(int IR)
{
    //Caso o campo de função acuse uma instrução do tipo-r
    if(IR == 0x00000000)
        s = 0x06;

    //Caso o campo de função acuse uma instrução beq
    else if(IR == 0x10000000)
        s = 0x08;

    //Caso o campo de função acuse uma instrução j
    else if (IR == 0x08000000)
        s = 0x09;

    //Caso o campo de função acuse uma instrução lw/sw
    else if(IR == 0x8c000000 || IR == 0xac000000)
        s = 0x02;
};

//"Tabela de despacho 2", que seta o estado de acordo com o campo de função da instrução atual
void desc_desp2(int IR)
{
    //Caso o campo de função acuse uma instrução lw
    if(IR == 0x8c000000)
        s = 0x03;

    //Caso o campo de função acuse uma instrução sw
    else if(IR == 0xac000000)
        s = 0x05;
};

//Função que retorna os bits de controle necessários para realizar cada operação, de acordo com o estado
//Os bits "don't care" são setados para 0, para facilitar a lógica
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
    Consideração final:

    - Os castings dentro das estruturas de controle foram necessários para impedir a extensão de sinal
        e a extensão dos bits por parte do computador/compilador (não temos certeza de quem é a culpa)

*/
