#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "translate.h"

//Ler Arquivo com os comandos|Luis Fellype
void ler_arquivo(){
    FILE *p;
    p = fopen("Assembly.txt", "r");
    if (p == NULL){
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

}

//Struct que guarda índice dos registradores
typedef struct regs{
    int indice;
    char *nome;
}REGS;

typedef struct opcode{
    char *op;
    char *nome;
}OPCODE;

//Definir nome da instrução com seu opcode
OPCODE lista_opcodes[] = {
    {"0110111", "lui"},
    {"1101111", "jal"},
    {"1100111", "jalr"},
    {"1100011", "beq"},  
    {"1100011", "bne"},
    {"0000011", "lw"},    
    {"0100011", "sw"},    
    {"0010011", "addi"},  
    {"0010011", "slli"},  
    {"0010011", "srli"},
    {"0110011", "add"},   
    {"0110011", "sub"},
    {"0110011", "xor"},
    {"0110011", "or"},
    {"0110011", "and"}
};

//enumerar os tipos no montador
enum types {TYPE_I,TYPE_U,TYPE_J,TYPE_B,TYPE_S,TYPE_R};

//vincular indice ao nome dos registradores
REGS banco_registradores[] = {
    {0,  "x0"},   {0,  "zero"},
    {1,  "x1"},   {1,  "ra"},
    {2,  "x2"},   {2,  "sp"},
    {3,  "x3"},   {3,  "gp"},
    {4,  "x4"},   {4,  "tp"},
    {5,  "x5"},   {5,  "t0"},
    {6,  "x6"},   {6,  "t1"},
    {7,  "x7"},   {7,  "t2"},
    {8,  "x8"},   {8,  "s0"},   {8,  "fp"}, //mesmo registrador que s0
    {9,  "x9"},   {9,  "s1"},
    {10, "x10"},  {10, "a0"},
    {11, "x11"},  {11, "a1"},
    {12, "x12"},  {12, "a2"},
    {13, "x13"},  {13, "a3"},
    {14, "x14"},  {14, "a4"},
    {15, "x15"},  {15, "a5"},
    {16, "x16"},  {16, "a6"},
    {17, "x17"},  {17, "a7"},
    {18, "x18"},  {18, "s2"},
    {19, "x19"},  {19, "s3"},
    {20, "x20"},  {20, "s4"},
    {21, "x21"},  {21, "s5"},
    {22, "x22"},  {22, "s6"},
    {23, "x23"},  {23, "s7"},
    {24, "x24"},  {24, "s8"},
    {25, "x25"},  {25, "s9"},
    {26, "x26"},  {26, "s10"},
    {27, "x27"},  {27, "s11"},
    {28, "x28"},  {28, "t3"},
    {29, "x29"},  {29, "t4"},
    {30, "x30"},  {30, "t5"},
    {31, "x31"},  {31, "t6"},
    {-1," "}//Referência de parada
};


//Transforma um numero em uma string em binário de 5bits
int int_para_5bits(int num, char* bin_str) {
    if (num < 0 || num > 31) {
        return 0;  // erro
    }

    for (int i = 4; i >= 0; i--) {
        bin_str[4 - i] = ((num >> i) & 1) + '0';
    }
    bin_str[5] = '\0';
    return 1;  // sucesso
}

int get_rdnum(char* rd){
    for(int i=0;banco_registradores[i].indice!=-1;i++){
        if(!strcmp(rd,banco_registradores[i].nome)){
            return banco_registradores[i].indice;
        }
    }
    return -1;
}

//Retorna o rd em binário
char *get_rd(char* reg_d){
    char rd_bit[5];
    int rd_num = get_rdnum(reg_d);
    printf("rd_num:  %d\n", rd_num);//(para debug, retirar dps)
    int_para_5bits(rd_num,rd_bit);
    printf("rd_bit:  %s\n", rd_bit);//(para debug, retirar dps)
    return rd_bit;
}

//Checar lista de registradores:(Remover quando finalizar)
void print_regs(){
    for(int i=0;banco_registradores[i].indice>=0;i++){
        printf("Indice:%2d, registrador %s, \n",banco_registradores[i].indice,banco_registradores[i].nome);
    }
}



//Retorna o tipo de instrução a partir do nome
int optype_from_opname(char *opname){
    if(strcmp(opname,"addi")||strcmp(opname,"jalr")||strcmp(opname,"lw")||strcmp(opname,"slli")||strcmp(opname,"srli")) {return TYPE_I;}
    if(strcmp(opname,"lui")) {return TYPE_U;}
    if(strcmp(opname,"jal")) {return TYPE_J;}
    if(strcmp(opname,"beq")||strcmp(opname,"bne")) {return TYPE_B;}
    if(strcmp(opname,"sw")){return TYPE_S;}
    if(strcmp(opname,"add")||strcmp(opname,"sub")||strcmp(opname,"xor")||strcmp(opname,"or")||strcmp(opname,"and")){return TYPE_R;}
    return (-1);//Caso não seja nenhum dos casos acima
}

//Retorna opcode
char * get_opcode(char* opname){
    for(int i=0;i!=-1;i++){
        if(strcmp(opname,lista_opcodes[i].nome)){
            return lista_opcodes[i].op;
        }
    }
    return NULL;
}


//Separa cada parte da instrução e usa uma função para retornar seu valor em binário
void traduzir(char* ins){
    char opname[20] , rd[20] , rs1[20] , rs2imm[20] , extra[30] ;
    char *token;

    //Transferir cada campo da instrução pra uma variável
    token = strtok(ins, " ,\t\n");
    if (token) strcpy(opname, token);
    token = strtok(NULL, " ,\t\n");//O NULL faz continuar de onde parou
    if (token) strcpy(rd, token);
    token = strtok(NULL, " ,\t\n");
    if (token) strcpy(rs1, token);
    token = strtok(NULL, " ,\t\n");
    if (token) strcpy(rs2imm, token);
    token = strtok(NULL, " ,\t\n");
    if (token) strcpy(extra, token);

    // instrução separada(Para debug, retirar dps)
    printf("opname:  %s\n", opname);
    printf("rd:      %s\n", rd);
    printf("rs1:     %s\n", rs1);
    printf("rs2imm:  %s\n", rs2imm);
    printf("extra:   %s\n", extra);
    
    //Qual o tipo da função?
    int op_type = optype_from_opname(opname);
    printf("optype: %d\n",op_type);//(Para debug, retirar dps)
    
    //Transformar em binário de acordo com o tipo
    //Independente do tipo, todos tem opcode então:
    char opcode_bit[7];
    strcpy(opcode_bit,get_opcode(opname));
    printf("opcode:  %s\n", opcode_bit);//(para debug, retirar dps)
    
    
    if(op_type==TYPE_I){
        char rd_bit[5];
        strcpy(rd_bit,get_rd(rd));

    }

}

int main(){
    //print_regs();
    /*Ler texto e transformar em binário linha por linha*/
    char instruction[30] = "addi x10 s2 8";//= *Recebe somente 1 instrução*;
    traduzir(instruction);    
}