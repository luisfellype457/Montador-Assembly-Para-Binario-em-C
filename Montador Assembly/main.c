#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

//Struct que guarda índice dos registradores
typedef struct regs{
    int indice;
    char *nome;
}REGS;

typedef struct opcode{
    char *op;
    char *nome;
}OPCODE;

OPCODE lista_opcodes[] = {
    {"0110111","lui"},
    {"1101111","jal"},
    {"1100111","jalr"},

};

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

//Checar lista de registradores:(Remover quando finalizar)
void print_regs(){
    for(int i=0;banco_registradores[i].indice>=0;i++){
        printf("Indice:%2d, registrador %s, \n",banco_registradores[i].indice,banco_registradores[i].nome);
    }
}

//Ler Arquivo com os comandos|Luis Fellype
void ler_arquivo(){
    FILE *p;
    p = fopen("Assembly.txt", "r");
    if (p == NULL){
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

}

//Retorna o tipo de instrução a partir do nome
int optype_from_opcode(char *opname){
    if(strcmp(opname,"lui")) {return TYPE_U;}
    if(strcmp(opname,"addi")||strcmp(opname,"jalr")||strcmp(opname,"lw")||strcmp(opname,"slli")||strcmp(opname,"srli")) {return TYPE_I;}
    if(strcmp(opname,"beq")||strcmp(opname,"bne")) {return TYPE_B;}
    if(strcmp(opname,"jal")) {return TYPE_J;}
    if(strcmp(opname,"sw")){return TYPE_S;}
    if(strcmp(opname,"add")||strcmp(opname,"sub")||strcmp(opname,"xor")||strcmp(opname,"or")||strcmp(opname,"and")){return TYPE_R;}
}

int binary_from_instruction(char *opname,char *rd,char *rs1,char *rs2imm,char *extra){
    int op_type = optype_from_opcode(opname);
    
}

void traduzir(char* ins){
    char opname[20],rd[20], rs1[20], rs2imm[20],extra[30];
    
    //Guarda dados da instrução
    *opname  =   strtok(ins ," ,\t\n");
    *rd      =   strtok(NULL," ,\t\n");//O NULL aqui serve pra string continuar de onde parou 
    *rs1     =   strtok(NULL," ,\t\n");
    *rs2imm  =   strtok(NULL," ,\t\n");
    *extra   =   strtok(NULL," ,\t\n");

    char *binary_ins = binary_from_instruction(opname,rd,rs1,rs2imm,extra);
}

int main(){
    print_regs();
    
    /*Ler texto e transformar em binário linha por linha*/
    char *instruction;//= *Recebe somente 1 instrução*;
    traduzir(instruction);    
}