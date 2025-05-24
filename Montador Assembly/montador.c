#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// --- Estrutura para armazenar Labels ---
#define MAX_LABELS 100 // Número máximo de labels que o montador pode lidar
typedef struct {
    char name[50]; // Nome do label
    int address;   // Endereço (em bytes) do label
} Label;

Label labels[MAX_LABELS];
int label_count = 0;

// Função para adicionar um label à tabela
void add_label(const char *name, int address) {
    if (label_count < MAX_LABELS) {
        strcpy(labels[label_count].name, name);
        labels[label_count].address = address;
        label_count++;
    } else {
        fprintf(stderr, "Erro: Limite de labels excedido. Aumente MAX_LABELS.\n");
    }
}

// Função para buscar o endereço de um label
int find_label_address(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, name) == 0) {
            return labels[i].address;
        }
    }
    return -1; // Label não encontrado
}

// --- Funções Auxiliares ---
int get_register_number(const char *reg_name) {
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
        return atoi(&reg_name[1]);
    }
    return -1; // Erro
}

void dec_to_bin_n_bits(int n, int num, char *bin_str) {
    for (int i = n - 1; i >= 0; i--) {
        bin_str[n - 1 - i] = ((num >> i) & 1) ? '1' : '0';
    }
    bin_str[n] = '\0';
}

// --- Função da Primeira Passagem ---
void first_pass(const char *input_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Erro ao abrir o arquivo de entrada na primeira passagem");
        exit(1);
    }

    char line[256];
    int current_address = 0;

    while (fgets(line, sizeof(line), input_file) != NULL) {
        if (strlen(line) <= 1 || line[0] == '#' || line[0] == ';') {
            continue;
        }
        line[strcspn(line, "\n")] = 0;
        char temp_line[256];
        strcpy(temp_line, line);
        char *token = strtok(temp_line, " ,\t");
        if (token == NULL) continue;

        if (strchr(token, ':') != NULL) {
            token[strlen(token) - 1] = '\0';
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

// --- Função da Segunda Passagem ---
void second_pass(const char *input_filename, const char *output_filename) {
    FILE *input_file = NULL;
    FILE *output_file = NULL;
    char line[256];
    // char opcode_str[33]; // No longer needed for full 32-bit string at once
    int current_address = 0;

    input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Erro ao abrir o arquivo de entrada na segunda passagem");
        return;
    }

    output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(input_file);
        return;
    }

    while (fgets(line, sizeof(line), input_file) != NULL) {
        if (strlen(line) <= 1 || line[0] == '#' || line[0] == ';') {
            continue;
        }
        line[strcspn(line, "\n")] = 0;
        char temp_line[256];
        strcpy(temp_line, line);
        char *token = strtok(temp_line, " ,\t");
        if (token == NULL) continue;

        char *instr = token;
        if (strchr(instr, ':') != NULL) {
            instr = strtok(NULL, " ,\t");
            if (instr == NULL) {
                continue;
            }
        }
        
        uint32_t instruction_binary = 0; // Initialize for safety, though it should be set by a valid instruction
        int instruction_valid = 1; // Flag to check if instruction was processed correctly

        // --- Instruções tipo R ---
        if (strcmp(instr, "add") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'add'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "sub") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'sub'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else instruction_binary = (0b0100000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "xor") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'xor'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b100 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "or") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'or'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b110 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "and") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'and'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b111 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "slli") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *shamt_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int shamt = atoi(shamt_str);
            if (rd == -1 || rs1 == -1 || shamt < 0 || shamt >= 32) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'slli'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else instruction_binary = (0b0000000 << 25) | (shamt << 20) | (rs1 << 15) | (0b001 << 12) | (rd << 7) | 0b0010011;
        } else if (strcmp(instr, "srli") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *shamt_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int shamt = atoi(shamt_str);
            if (rd == -1 || rs1 == -1 || shamt < 0 || shamt >= 32) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'srli'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else instruction_binary = (0b0000000 << 25) | (shamt << 20) | (rs1 << 15) | (0b101 << 12) | (rd << 7) | 0b0010011;

        // --- Instruções tipo I ---
        } else if (strcmp(instr, "addi") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *imm_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int imm = atoi(imm_str);
            if (rd == -1 || rs1 == -1 || imm < -2048 || imm > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'addi'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else instruction_binary = ((imm & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0010011;
        } else if (strcmp(instr, "lw") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *offset_rs1_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(offset_rs1_str, "("); char *rs1_str = strtok(NULL, ")");
            if (rd_str == NULL || offset_str == NULL || rs1_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inválido para 'lw'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int offset = atoi(offset_str);
                if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'lw'. Linha: %s\n", current_address, line); instruction_valid = 0; }
                else instruction_binary = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b010 << 12) | (rd << 7) | 0b0000011;
            }
        
        // --- Instruções tipo S ---
        } else if (strcmp(instr, "sw") == 0) {
            char *rs2_str = strtok(NULL, " ,\t"); char *offset_rs1_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(offset_rs1_str, "("); char *rs1_str = strtok(NULL, ")");
            if (rs2_str == NULL || offset_str == NULL || rs1_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inválido para 'sw'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else {
                int rs2 = get_register_number(rs2_str); int rs1 = get_register_number(rs1_str); int offset = atoi(offset_str);
                if (rs2 == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'sw'. Linha: %s\n", current_address, line); instruction_valid = 0; }
                else {
                    uint32_t imm_11_5 = (offset >> 5) & 0x7F; uint32_t imm_4_0 = offset & 0x1F;
                    instruction_binary = (imm_11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b010 << 12) | (imm_4_0 << 7) | 0b0100011;
                }
            }

        // --- Instruções tipo B ---
        } else if (strcmp(instr, "beq") == 0 || strcmp(instr, "bne") == 0) {
            char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t"); char *target_label = strtok(NULL, " ,\t");
            if (rs1_str == NULL || rs2_str == NULL || target_label == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inválido para '%s'. Linha: %s\n", current_address, instr, line); instruction_valid = 0; }
            else {
                int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
                int target_address = find_label_address(target_label);
                if (rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para '%s'. Linha: %s\n", current_address, instr, line); instruction_valid = 0; }
                else if (target_address == -1) { fprintf(stderr, "Erro em 0x%04X: Label '%s' não encontrado. Linha: %s\n", current_address, target_label, line); instruction_valid = 0; }
                else {
                    int offset = target_address - current_address; 
                    if (offset % 2 != 0) { fprintf(stderr, "Erro em 0x%04X: Offset do branch '%s' não é múltiplo de 2. Linha: %s\n", current_address, target_label, line); instruction_valid = 0; }
                    else {
                        offset = offset / 2; // Immediate is in terms of half-words
                        // Check B-imm range (-4096 to +4094 in steps of 2 -> offset/2 is -2048 to +2047, fits in 12 bits signed + 1 implicit 0 bit)
                        if (offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Offset do branch '%s' fora do alcance. Linha: %s\n", current_address, target_label, line); instruction_valid = 0;}
                        else {
                            uint32_t imm_12 = (offset >> 11) & 0x1;  // imm[12]
                            uint32_t imm_10_5 = (offset >> 4) & 0x3F; // imm[10:5]
                            uint32_t imm_4_1 = offset & 0xF;          // imm[4:1]
                            uint32_t imm_11 = (offset >> 10) & 0x1;   // imm[11]
                            if (strcmp(instr, "beq") == 0) {
                                instruction_binary = (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (imm_4_1 << 8) | (imm_11 << 7) | 0b1100011;
                            } else { // bne
                                instruction_binary = (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b001 << 12) | (imm_4_1 << 8) | (imm_11 << 7) | 0b1100011;
                            }
                        }
                    }
                }
            }
        
        // --- Instruções tipo U ---
        } else if (strcmp(instr, "lui") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *imm_str = strtok(NULL, " ,\t");
            if (rd_str == NULL || imm_str == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inválido para 'lui'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str); int imm = atoi(imm_str); // Or strtol for hex/octal
                if (rd == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'lui'. Linha: %s\n", current_address, line); instruction_valid = 0; }
                // LUI immediate is 20 bits. imm operand usually represents the upper 20 bits.
                // Example: lui rd, 0xABCDE -> loads 0xABCDE000 into rd.
                // The value provided to lui is typically the value for bits 31:12.
                // The assembler takes this value and places it directly into the U-immediate field.
                else instruction_binary = ((imm & 0xFFFFF) << 12) | (rd << 7) | 0b0110111;
            }

        // --- Instruções tipo J (jal) ---
        } else if (strcmp(instr, "jal") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *target_label = strtok(NULL, " ,\t");
            if (rd_str == NULL || target_label == NULL) { fprintf(stderr, "Erro em 0x%04X: Formato inválido para 'jal'. Linha: %s\n", current_address, line); instruction_valid = 0; }
            else {
                int rd = get_register_number(rd_str);
                int target_address = find_label_address(target_label);
                if (rd == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'jal'. Linha: %s\n", current_address, line); instruction_valid = 0; }
                else if (target_address == -1) { fprintf(stderr, "Erro em 0x%04X: Label '%s' não encontrado. Linha: %s\n", current_address, target_label, line); instruction_valid = 0; }
                else {
                    int offset = target_address - current_address;
                     if (offset % 2 != 0) { fprintf(stderr, "Erro em 0x%04X: Offset do JAL '%s' não é múltiplo de 2. Linha: %s\n", current_address, target_label, line); instruction_valid = 0; }
                    else {
                        offset = offset / 2; // Immediate is in terms of half-words
                        // J-imm range: +/- 1MB (20 bits for offset/2, so 2^20 half-words = 2^21 bytes)
                        if (offset < -(1 << 19) || offset >= (1 << 19) ) { fprintf(stderr, "Erro em 0x%04X: Offset JAL '%s' fora do alcance. Linha: %s\n", current_address, target_label, line); instruction_valid = 0; }
                        else {
                            // Correct JAL immediate bit extraction (offset is already offset/2)
                            // imm[20]    (offset bit 19)
                            // imm[10:1]  (offset bits 9:0)
                            // imm[11]    (offset bit 10)
                            // imm[19:12] (offset bits 18:11)
                            uint32_t imm20    = (offset >> 19) & 0x1;    // imm[20]
                            uint32_t imm10_1  = (offset >> 0)  & 0x3FF;  // imm[10:1]
                            uint32_t imm11    = (offset >> 10) & 0x1;    // imm[11]
                            uint32_t imm19_12 = (offset >> 11) & 0xFF;   // imm[19:12]

                            // Correct JAL instruction assembly
                            // Inst: imm[20] | imm[10:1] | imm[11] | imm[19:12] | rd | opcode
                            // Bits: 31      | 30-21     | 20      | 19-12        | 11-7 | 6-0
                            instruction_binary = (imm20 << 31) | (imm10_1 << 21) | (imm11 << 20) | (imm19_12 << 12) | (rd << 7) | 0b1101111;
                        }
                    }
                }
            }

        // --- Instruções tipo I (jalr) ---
        } else if (strcmp(instr, "jalr") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *offset_rs1_str = strtok(NULL, " ,\t"); // e.g. "0(ra)"
            // Some assemblers allow "jalr rd, rs1" which implies offset 0, or "jalr rd, offset(rs1)"
            // Current parsing expects offset(rs1)
            char *rs1_str_full = offset_rs1_str; // Keep a pointer for error message if parsing fails
            char *offset_str = strtok(offset_rs1_str, "("); 
            char *rs1_str = strtok(NULL, ")");

            if (rd_str == NULL || offset_str == NULL || rs1_str == NULL ) { 
                // Attempt to parse "jalr rd, rs1" (offset is 0)
                rs1_str = strtok(rs1_str_full, " ,\t"); // rs1_str_full should be just rs1 now
                offset_str = "0"; // Assume offset 0
                 if (rd_str == NULL || rs1_str == NULL) {
                    fprintf(stderr, "Erro em 0x%04X: Formato inválido para 'jalr'. Use 'rd, offset(rs1)' or 'rd, rs1'. Linha: %s\n", current_address, line); instruction_valid = 0;
                 }
            }
            if (instruction_valid) { // Proceed if format parsing was okay or adjusted
                int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int offset = atoi(offset_str);
                if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'jalr'. Linha: %s\n", current_address, line); instruction_valid = 0; }
                else instruction_binary = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b1100111;
            }


        } else {
            fprintf(stderr, "Instrução desconhecida em 0x%04X: '%s'. Linha: %s\n", current_address, instr, line);
            instruction_valid = 0;
        }

        if (instruction_valid) {
            char byte_binary_str[9]; // 8 bits + null terminator

            // Little-endian byte order
            uint8_t byte0 = (instruction_binary >> 0) & 0xFF;
            uint8_t byte1 = (instruction_binary >> 8) & 0xFF;
            uint8_t byte2 = (instruction_binary >> 16) & 0xFF;
            uint8_t byte3 = (instruction_binary >> 24) & 0xFF;

            dec_to_bin_n_bits(8, byte0, byte_binary_str);
            fprintf(output_file, "%s\n", byte_binary_str);

            dec_to_bin_n_bits(8, byte1, byte_binary_str);
            fprintf(output_file, "%s\n", byte_binary_str);

            dec_to_bin_n_bits(8, byte2, byte_binary_str);
            fprintf(output_file, "%s\n", byte_binary_str);

            dec_to_bin_n_bits(8, byte3, byte_binary_str);
            fprintf(output_file, "%s\n", byte_binary_str);
            
            current_address += 4;
        }
    }

    fclose(input_file);
    fclose(output_file);
    printf("Montagem concluída! Arquivo '%s' gerado.\n", output_filename);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada.asm> <arquivo_saida.mif>\n", argv[0]);
        return 1;
    }

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    // Print input file name and content
    printf("%s:\n", input_filename);
    FILE *asm_file_for_print = fopen(input_filename, "r");
    if (asm_file_for_print == NULL) {
        perror("Erro ao abrir arquivo de entrada para leitura inicial");
        return 1; 
    }
    char line_buffer[256];
    while (fgets(line_buffer, sizeof(line_buffer), asm_file_for_print)) {
        fputs(line_buffer, stdout); // Prints the line including its newline
    }
    fclose(asm_file_for_print);
    // Ensure a newline after asm content if the file doesn't end with one,
    // or if you want consistent spacing. The example output suggests this.
    if (line_buffer[strlen(line_buffer)-1] != '\n') {
        printf("\n");
    }


    // First Pass: Coleta labels
    first_pass(input_filename);

    // Second Pass: Monta o código e resolve labels
    second_pass(input_filename, output_filename);

    // Print output file name and content
    printf("\n%s:\n", output_filename); 
    FILE *mif_file_for_print = fopen(output_filename, "r");
    if (mif_file_for_print == NULL) {
        fprintf(stderr, "Aviso: Não foi possível abrir '%s' para ler seu conteúdo. Pode não ter sido gerado ou estar vazio devido a erros na montagem.\n", output_filename);
    } else {
        char mif_line_buffer[32]; // MIF lines are short (8 chars + newline + null)
        while (fgets(mif_line_buffer, sizeof(mif_line_buffer), mif_file_for_print)) {
            fputs(mif_line_buffer, stdout);
        }
        fclose(mif_file_for_print);
    }

    return 0;
}
