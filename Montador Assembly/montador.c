/*Montador  
Lucas Ricardo de Lima Figueiredo e
Luis Fellype Magalhães Cruz
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_LABELS 100
typedef struct {
    char name[50];
    int address;
} Label;

Label labels[MAX_LABELS];
int label_count = 0;

// Adiciona um novo label (nome e endereço) à tabela de labels.
void add_label(const char *name, int address) {
    if (label_count < MAX_LABELS) {
        strcpy(labels[label_count].name, name);
        labels[label_count].address = address;
        label_count++;
    } else {
        fprintf(stderr, "Erro: Limite de labels excedido. Aumente MAX_LABELS.\n");
    }
}

// Procura um label na tabela pelo nome e retorna seu endereço.
int find_label_address(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, name) == 0) {
            return labels[i].address;
        }
    }
    return -1;
}

// Converte o nome de um registrador (ex: "t0", "ra") para seu número correspondente.
int get_register_number(const char *reg_name) {
    if (reg_name == NULL) return -1;
    if (strcmp(reg_name, "zero") == 0) return 0;
    if (strcmp(reg_name, "ra") == 0) return 1;
    if (strcmp(reg_name, "sp") == 0) return 2;
    if (strcmp(reg_name, "gp") == 0) return 3;
    if (strcmp(reg_name, "tp") == 0) return 4;
    if (strcmp(reg_name, "t0") == 0) return 5;
    if (strcmp(reg_name, "t1") == 0) return 6;
    if (strcmp(reg_name, "t2") == 0) return 7;
    if (strcmp(reg_name, "s0") == 0 || strcmp(reg_name, "fp") == 0) return 8;
    if (strcmp(reg_name, "s1") == 0) return 9;
    if (strcmp(reg_name, "a0") == 0) return 10;
    if (strcmp(reg_name, "a1") == 0) return 11;
    if (strcmp(reg_name, "a2") == 0) return 12;
    if (strcmp(reg_name, "a3") == 0) return 13;
    if (strcmp(reg_name, "a4") == 0) return 14;
    if (strcmp(reg_name, "a5") == 0) return 15;
    if (strcmp(reg_name, "a6") == 0) return 16;
    if (strcmp(reg_name, "a7") == 0) return 17;
    if (strcmp(reg_name, "s2") == 0) return 18;
    if (strcmp(reg_name, "s3") == 0) return 19;
    if (strcmp(reg_name, "s4") == 0) return 20;
    if (strcmp(reg_name, "s5") == 0) return 21;
    if (strcmp(reg_name, "s6") == 0) return 22;
    if (strcmp(reg_name, "s7") == 0) return 23;
    if (strcmp(reg_name, "s8") == 0) return 24;
    if (strcmp(reg_name, "s9") == 0) return 25;
    if (strcmp(reg_name, "s10") == 0) return 26;
    if (strcmp(reg_name, "s11") == 0) return 27;
    if (strcmp(reg_name, "t3") == 0) return 28;
    if (strcmp(reg_name, "t4") == 0) return 29;
    if (strcmp(reg_name, "t5") == 0) return 30;
    if (strcmp(reg_name, "t6") == 0) return 31;
    
    if (reg_name[0] == 'x') {
        char *endptr;
        long val = strtol(&reg_name[1], &endptr, 10);
        if (*endptr == '\0' && val >= 0 && val <= 31) {
            return (int)val;
        }
    }
    return -1;
}

// Converte um número inteiro para sua representação em string binária com 'n' bits.
void dec_to_bin_n_bits(int n, int num, char *bin_str) {
    for (int i = n - 1; i >= 0; i--) {
        bin_str[n - 1 - i] = ((num >> i) & 1) ? '1' : '0';
    }
    bin_str[n] = '\0';
}

// Limpa uma linha de entrada: remove espaços/tabs iniciais/finais, comentários e newlines.
// Retorna 1 se a linha contém conteúdo útil após a limpeza, 0 caso contrário.
int clean_input_line(const char* original_line, char* cleaned_line_out, size_t out_size) {
    const char* p_in = original_line;
    char* p_out = cleaned_line_out;
    char* out_end = cleaned_line_out + out_size - 1; 

    while (*p_in != '\0' && isspace((unsigned char)*p_in)) {
        p_in++;
    }

    int content_found = 0;
    while (*p_in != '\0' && *p_in != '\n' && *p_in != '\r' && *p_in != '#' && *p_in != ';') {
        if (p_out < out_end) {
            *p_out++ = *p_in;
            if (!isspace((unsigned char)*p_in)) { 
                content_found = 1;
            }
        } else {
            *p_out = '\0';
            return 0; 
        }
        p_in++;
    }
    *p_out = '\0';

    p_out--;
    while (p_out >= cleaned_line_out && isspace((unsigned char)*p_out)) {
        *p_out-- = '\0';
    }
    
    if (cleaned_line_out[0] == '\0') { 
        content_found = 0;
    }
    return content_found;
}

// Realiza a primeira passagem do montador para identificar e armazenar todos os labels e seus endereços.
void first_pass(const char *input_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Erro ao abrir o arquivo de entrada na primeira passagem");
        exit(1);
    }

    char line_buffer[256];
    char cleaned_line[256];
    int current_address = 0;
    label_count = 0; 

    while (fgets(line_buffer, sizeof(line_buffer), input_file) != NULL) {
        if (!clean_input_line(line_buffer, cleaned_line, sizeof(cleaned_line))) {
            continue; 
        }
        char temp_line_for_token[256];
        strcpy(temp_line_for_token, cleaned_line); 
        char *token = strtok(temp_line_for_token, " ,\t");
        if (token == NULL) continue; 

        char *colon_ptr = strchr(token, ':');
        if (colon_ptr != NULL) {
            *colon_ptr = '\0'; 
            add_label(token, current_address);
            token = strtok(NULL, " ,\t"); 
            if (token == NULL) { 
                continue; 
            }
        }
        current_address += 4; 
    }
    fclose(input_file);
}

// Realiza a segunda passagem: traduz as instruções para código de máquina binário.
// Escreve a saída binária no arquivo especificado, controlando o newline final.
void second_pass(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Erro ao abrir o arquivo de entrada na segunda passagem");
        return;
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(input_file);
        return;
    }

    char line_buffer[256];
    char cleaned_line[256];
    char original_line_for_error[256];
    int current_address = 0;

    char previous_instruction_bytes[4][10]; 
    int has_previous_instruction = 0;

    while (fgets(line_buffer, sizeof(line_buffer), input_file) != NULL) {
        strcpy(original_line_for_error, line_buffer);
        original_line_for_error[strcspn(original_line_for_error, "\n\r")] = 0;

        if (!clean_input_line(line_buffer, cleaned_line, sizeof(cleaned_line))) {
            continue;
        }
        
        char temp_line_for_token[256];
        strcpy(temp_line_for_token, cleaned_line);
        char *token = strtok(temp_line_for_token, " ,\t");
        if (token == NULL) continue;

        char *instr = token;
        if (strchr(instr, ':') != NULL) { 
            instr = strtok(NULL, " ,\t"); 
            if (instr == NULL) { 
                continue;
            }
        }
        
        uint32_t instruction_binary = 0;
        int instruction_valid = 1;

        if (strcmp(instr, "add") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || rs1_str == NULL || rs2_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'add'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
                if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'add'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;
            }
        } else if (strcmp(instr, "sub") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || rs1_str == NULL || rs2_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'sub'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
                if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'sub'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else instruction_binary = (0b0100000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;
            }
        } else if (strcmp(instr, "xor") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || rs1_str == NULL || rs2_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'xor'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
                if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'xor'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b100 << 12) | (rd << 7) | 0b0110011;
            }
        } else if (strcmp(instr, "or") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || rs1_str == NULL || rs2_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'or'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
                if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'or'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b110 << 12) | (rd << 7) | 0b0110011;
            }
        } else if (strcmp(instr, "and") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || rs1_str == NULL || rs2_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'and'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
                if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'and'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b111 << 12) | (rd << 7) | 0b0110011;
            }
        } else if (strcmp(instr, "slli") == 0) { 
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *shamt_str = strtok(NULL, " ,\t");
             if (rd_str == NULL || rs1_str == NULL || shamt_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'slli'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int shamt = atoi(shamt_str);
                if (rd == -1 || rs1 == -1 || shamt < 0 || shamt > 31) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'slli'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else instruction_binary = (shamt << 20) | (rs1 << 15) | (0b001 << 12) | (rd << 7) | 0b0010011;
            }
        } else if (strcmp(instr, "srli") == 0) { 
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *shamt_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || rs1_str == NULL || shamt_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'srli'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int shamt = atoi(shamt_str);
                if (rd == -1 || rs1 == -1 || shamt < 0 || shamt > 31) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'srli'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else instruction_binary = (shamt << 20) | (rs1 << 15) | (0b101 << 12) | (rd << 7) | 0b0010011;
            }
        } else if (strcmp(instr, "addi") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *imm_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || rs1_str == NULL || imm_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'addi'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int imm = (int)strtol(imm_str, NULL, 0);
                if (rd == -1 || rs1 == -1 || imm < -2048 || imm > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'addi'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else instruction_binary = ((imm & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0010011;
            }
        } else if (strcmp(instr, "lw") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *offset_rs1_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || offset_rs1_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'lw'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                char *temp_offset_rs1 = strdup(offset_rs1_str); 
                char *offset_str = strtok(temp_offset_rs1, "("); 
                char *rs1_name_token = strtok(NULL, ")");
                if (offset_str == NULL || rs1_name_token == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inválido para 'lw' (esperado offset(rs1)). Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else {
                    int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_name_token); int offset = (int)strtol(offset_str, NULL, 0);
                    if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'lw'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                    else instruction_binary = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b010 << 12) | (rd << 7) | 0b0000011;
                }
                free(temp_offset_rs1);
            }
        } else if (strcmp(instr, "sw") == 0) {
            char *rs2_str = strtok(NULL, " ,\t"); char *offset_rs1_str = strtok(NULL, " ,\t");
             if (rs2_str == NULL || offset_rs1_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'sw'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                char *temp_offset_rs1 = strdup(offset_rs1_str);
                char *offset_str = strtok(temp_offset_rs1, "("); 
                char *rs1_name_token = strtok(NULL, ")");
                if (offset_str == NULL || rs1_name_token == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inválido para 'sw' (esperado offset(rs1)). Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else {
                    int rs2 = get_register_number(rs2_str); int rs1 = get_register_number(rs1_name_token); int offset = (int)strtol(offset_str, NULL, 0);
                    if (rs2 == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'sw'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                    else {
                        uint32_t imm_11_5 = (offset >> 5) & 0x7F; uint32_t imm_4_0 = offset & 0x1F;
                        instruction_binary = (imm_11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b010 << 12) | (imm_4_0 << 7) | 0b0100011;
                    }
                }
                free(temp_offset_rs1);
            }
        } else if (strcmp(instr, "beq") == 0 || strcmp(instr, "bne") == 0) {
            char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t"); char *target_label = strtok(NULL, " ,\t");
            if (rs1_str == NULL || rs2_str == NULL || target_label == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para '%s'. Linha: %s\n", current_address, instr, original_line_for_error); instruction_valid = 0; }
            else {
                int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
                int target_address = find_label_address(target_label);
                if (rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para '%s'. Linha: %s\n", current_address, instr, original_line_for_error); instruction_valid = 0; }
                else if (target_address == -1) { fprintf(stderr, "Erro em 0x%04X: Label '%s' não encontrado. Linha: %s\n", current_address, target_label, original_line_for_error); instruction_valid = 0; }
                else {
                    int offset = target_address - current_address; 
                    if (offset % 2 != 0) { fprintf(stderr, "Erro em 0x%04X: Offset do branch '%s' não é múltiplo de 2 (offset=%d). Linha: %s\n", current_address, target_label, offset, original_line_for_error); instruction_valid = 0; }
                    else {
                        int imm = offset / 2; 
                        if (imm < -2048 || imm > 2047) { fprintf(stderr, "Erro em 0x%04X: Offset do branch '%s' fora do alcance (imm=%d). Linha: %s\n", current_address, target_label, imm, original_line_for_error); instruction_valid = 0;}
                        else {
                            uint32_t imm12   = (imm >> 11) & 0x1; uint32_t imm10_5 = (imm >> 4) & 0x3F; 
                            uint32_t imm4_1  = (imm >> 0) & 0xF;  uint32_t imm11 = (imm >> 10) & 0x1;  
                            if (strcmp(instr, "beq") == 0) instruction_binary = (imm12 << 31) | (imm10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (imm4_1 << 8) | (imm11 << 7) | 0b1100011;
                            else instruction_binary = (imm12 << 31) | (imm10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b001 << 12) | (imm4_1 << 8) | (imm11 << 7) | 0b1100011;
                        }
                    }
                }
            }
        } else if (strcmp(instr, "lui") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *imm_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || imm_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'lui'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); 
                long imm_val = strtol(imm_str, NULL, 0); 
                if (rd == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'lui'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else if (imm_val < 0 || imm_val > 0xFFFFF) { fprintf(stderr, "Erro em 0x%04X: Imediato para 'lui' fora do alcance (0 a 0xFFFFF), valor: %ld. Linha: %s\n", current_address, imm_val, original_line_for_error); instruction_valid = 0;}
                else instruction_binary = ((uint32_t)imm_val << 12) | (rd << 7) | 0b0110111;
            }
        } else if (strcmp(instr, "jal") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *target_label = strtok(NULL, " ,\t");
             if (rd_str == NULL || target_label == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'jal'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str);
                int target_address = find_label_address(target_label);
                if (rd == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'jal'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                else if (target_address == -1) { fprintf(stderr, "Erro em 0x%04X: Label '%s' não encontrado. Linha: %s\n", current_address, target_label, original_line_for_error); instruction_valid = 0; }
                else {
                    int offset = target_address - current_address;
                    if (offset % 2 != 0) { fprintf(stderr, "Erro em 0x%04X: Offset do JAL '%s' não é múltiplo de 2 (offset=%d). Linha: %s\n", current_address, target_label, offset, original_line_for_error); instruction_valid = 0; }
                    else {
                        int imm = offset / 2; 
                        if (imm < -(1 << 19) || imm >= (1 << 19) ) { fprintf(stderr, "Erro em 0x%04X: Offset JAL '%s' fora do alcance (imm=%d). Linha: %s\n", current_address, target_label, imm, original_line_for_error); instruction_valid = 0; }
                        else {
                            uint32_t imm20 = (imm >> 19) & 0x1; uint32_t imm10_1 = (imm >> 0) & 0x3FF; 
                            uint32_t imm11 = (imm >> 10) & 0x1; uint32_t imm19_12 = (imm >> 11) & 0xFF;  
                            instruction_binary = (imm20 << 31) | (imm10_1 << 21) | (imm11 << 20) | (imm19_12 << 12) | (rd << 7) | 0b1101111;
                        }
                    }
                }
            }
        } else if (strcmp(instr, "jalr") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *arg2 = strtok(NULL, " ,\t"); 
            if (rd_str == NULL || arg2 == NULL) { fprintf(stderr, "Erro em 0x%04X: Faltam operandos para 'jalr'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = -1; int offset = 0;
                char *temp_arg2 = strdup(arg2); 
                char *rs1_str_in_offset = strchr(temp_arg2, '(');
                if (rs1_str_in_offset != NULL) { 
                    *rs1_str_in_offset = '\0'; 
                    offset = (int)strtol(temp_arg2, NULL, 0);
                    char *rs1_name_token = rs1_str_in_offset + 1;
                    char *closing_paren = strchr(rs1_name_token, ')');
                    if (closing_paren != NULL) *closing_paren = '\0'; else {fprintf(stderr, "Erro em 0x%04X: Falta ')' em 'jalr'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0;}
                    if (instruction_valid) rs1 = get_register_number(rs1_name_token);
                } else { 
                    rs1 = get_register_number(temp_arg2); offset = 0;
                }
                free(temp_arg2);
                if (instruction_valid) { 
                    if (rd == -1 || rs1 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'jalr'. Linha: %s\n", current_address, original_line_for_error); instruction_valid = 0; }
                    else if (offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Offset para 'jalr' fora do alcance (%d). Linha: %s\n", current_address, offset, original_line_for_error); instruction_valid = 0;}
                    else instruction_binary = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b1100111;
                }
            }
        } else {
            fprintf(stderr, "Instrução desconhecida em 0x%04X: '%s'. Linha: %s\n", current_address, instr, original_line_for_error);
            instruction_valid = 0;
        }

        if (instruction_valid) {
            if (has_previous_instruction) {
                for (int i = 0; i < 4; ++i) {
                    fprintf(output_file, "%s\n", previous_instruction_bytes[i]);
                }
            }

            uint8_t byte0 = (instruction_binary >> 0) & 0xFF;
            uint8_t byte1 = (instruction_binary >> 8) & 0xFF;
            uint8_t byte2 = (instruction_binary >> 16) & 0xFF;
            uint8_t byte3 = (instruction_binary >> 24) & 0xFF;

            dec_to_bin_n_bits(8, byte0, previous_instruction_bytes[0]);
            dec_to_bin_n_bits(8, byte1, previous_instruction_bytes[1]);
            dec_to_bin_n_bits(8, byte2, previous_instruction_bytes[2]);
            dec_to_bin_n_bits(8, byte3, previous_instruction_bytes[3]);
            has_previous_instruction = 1;
            
            current_address += 4;
        }
    }

    if (has_previous_instruction) {
        for (int i = 0; i < 4; ++i) {
            fprintf(output_file, "%s", previous_instruction_bytes[i]); 
            if (i < 3) { 
                fprintf(output_file, "\n");
            }
        }
    }

    fclose(input_file);
    fclose(output_file);
    printf("Montagem concluída! Arquivo '%s' gerado.\n", output_filename);
}

// Função principal: processa argumentos de linha de comando, chama as passagens e imprime resultados.
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada.asm> <arquivo_saida.mif>\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    printf("%s:\n", input_filename);
    FILE *asm_file_for_print = fopen(input_filename, "r");
    if (asm_file_for_print == NULL) {
        perror("Erro ao abrir arquivo de entrada para leitura inicial");
        return 1; 
    }
    
    char asm_line_buffer[256];
    int asm_file_had_content = 0;
    char last_char_of_asm = '\0'; 

    while (fgets(asm_line_buffer, sizeof(asm_line_buffer), asm_file_for_print)) {
        fputs(asm_line_buffer, stdout);
        asm_file_had_content = 1;
        if (strlen(asm_line_buffer) > 0) {
            last_char_of_asm = asm_line_buffer[strlen(asm_line_buffer) - 1];
        }
    }
    fclose(asm_file_for_print);

    if (asm_file_had_content && last_char_of_asm != '\n' && last_char_of_asm != '\r') {
        printf("\n"); 
    } else if (!asm_file_had_content && input_filename != NULL) {
    }

    first_pass(input_filename);
    second_pass(input_filename, output_filename);

    printf("%s:\n", output_filename); 
    FILE *mif_file_for_print = fopen(output_filename, "r");
    if (mif_file_for_print == NULL) {
        fprintf(stderr, "Aviso: Não foi possível abrir '%s' para ler seu conteúdo.\n", output_filename);
    } else {
        char current_mif_line[32];
        char next_mif_line[32]; 
        
        if (fgets(current_mif_line, sizeof(current_mif_line), mif_file_for_print)) {
            while (1) {
                if (fgets(next_mif_line, sizeof(next_mif_line), mif_file_for_print)) {
                    fputs(current_mif_line, stdout); 
                    strcpy(current_mif_line, next_mif_line);
                } else { 
                    fputs(current_mif_line, stdout); 
                    break; 
                }
            }
        }
        fclose(mif_file_for_print);
    }
    return 0;
}