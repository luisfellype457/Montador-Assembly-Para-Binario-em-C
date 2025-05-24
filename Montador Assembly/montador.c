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

// --- Funções Auxiliares (mantidas do seu código) ---
int get_register_number(const char *reg_name) {
    // Mapeamento simbólico para registradores RISC-V padrão
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
    
    if (reg_name[0] == 'x') { // Permite x0, x1, etc. também
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
        exit(1); // Sai se não conseguir abrir
    }

    char line[256];
    int current_address = 0; // Endereço em bytes

    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Ignorar linhas vazias ou de comentário
        if (strlen(line) <= 1 || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Remover quebra de linha
        line[strcspn(line, "\n")] = 0;

        char temp_line[256]; // Usar uma cópia para strtok, pois ele modifica a string
        strcpy(temp_line, line);

        char *token = strtok(temp_line, " ,\t");
        if (token == NULL) continue;

        // Verificar se é um label (termina com ':')
        if (strchr(token, ':') != NULL) {
            token[strlen(token) - 1] = '\0'; // Remove o ':'
            add_label(token, current_address);
            // Agora, tente pegar a próxima parte da linha, se houver uma instrução após o label
            token = strtok(NULL, " ,\t");
            if (token == NULL) { // Era apenas um label na linha
                continue; // Não incrementa o endereço se não houver instrução
            }
            // Se houver instrução, cairemos no else if abaixo para incrementar o endereço
        }

        // Se é uma instrução, incrementa o endereço para a próxima instrução
        // Todas as instruções RISC-V são de 4 bytes
        current_address += 4;
    }
    fclose(input_file);
}

// --- Função da Segunda Passagem (Seu 'assemble' modificado) ---
void second_pass(const char *input_filename, const char *output_filename) {
    FILE *input_file = NULL;
    FILE *output_file = NULL;
    char line[256];
    char opcode_str[33]; // Para armazenar o binário de 32 bits + '\0'
    int current_address = 0; // Endereço de instrução atual

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

    // Escreve o cabeçalho do arquivo .mif
    fprintf(output_file, "DEPTH = 256;\n");
    fprintf(output_file, "WIDTH = 32;\n");
    fprintf(output_file, "ADDRESS_RADIX = HEX;\n");
    fprintf(output_file, "DATA_RADIX = BIN;\n");
    fprintf(output_file, "CONTENT BEGIN\n");

    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Ignorar linhas vazias ou de comentário
        if (strlen(line) <= 1 || line[0] == '#' || line[0] == ';') {
            continue;
        }

        line[strcspn(line, "\n")] = 0; // Remover quebra de linha

        char temp_line[256];
        strcpy(temp_line, line); // Cópia para strtok

        char *token = strtok(temp_line, " ,\t");
        if (token == NULL) continue;

        char *instr = token;

        // Se a linha começa com um label, pule-o e pegue a instrução real
        if (strchr(instr, ':') != NULL) {
            instr = strtok(NULL, " ,\t");
            if (instr == NULL) { // A linha continha apenas um label
                continue;
            }
        }
        
        uint32_t instruction_binary = 0;

        // --- Instruções tipo R (add, sub, xor, or, and, slli, srli) ---
        if (strcmp(instr, "add") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'add'.\n", current_address); continue; }
            instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "sub") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'sub'.\n", current_address); continue; }
            instruction_binary = (0b0100000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "xor") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'xor'.\n", current_address); continue; }
            instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b100 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "or") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'or'.\n", current_address); continue; }
            instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b110 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "and") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *rs2_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int rs2 = get_register_number(rs2_str);
            if (rd == -1 || rs1 == -1 || rs2 == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'and'.\n", current_address); continue; }
            instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b111 << 12) | (rd << 7) | 0b0110011;
        } else if (strcmp(instr, "slli") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *shamt_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int shamt = atoi(shamt_str);
            if (rd == -1 || rs1 == -1 || shamt < 0 || shamt >= 32) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'slli'.\n", current_address); continue; }
            instruction_binary = (0b0000000 << 25) | (shamt << 20) | (rs1 << 15) | (0b001 << 12) | (rd << 7) | 0b0010011;
        } else if (strcmp(instr, "srli") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *shamt_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int shamt = atoi(shamt_str);
            if (rd == -1 || rs1 == -1 || shamt < 0 || shamt >= 32) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'srli'.\n", current_address); continue; }
            instruction_binary = (0b0000000 << 25) | (shamt << 20) | (rs1 << 15) | (0b101 << 12) | (rd << 7) | 0b0010011;

        // --- Instruções tipo I (addi, lw, jalr) ---
        } else if (strcmp(instr, "addi") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *rs1_str = strtok(NULL, " ,\t"); char *imm_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int imm = atoi(imm_str);
            if (rd == -1 || rs1 == -1 || imm < -2048 || imm > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'addi'.\n", current_address); continue; }
            instruction_binary = ((imm & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0010011;
        } else if (strcmp(instr, "lw") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *offset_rs1_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(offset_rs1_str, "("); char *rs1_str = strtok(NULL, ")");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int offset = atoi(offset_str);
            if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'lw'.\n", current_address); continue; }
            instruction_binary = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b010 << 12) | (rd << 7) | 0b0000011;
        
        // --- Instruções tipo S (sw) ---
        } else if (strcmp(instr, "sw") == 0) {
            char *rs2_str = strtok(NULL, " ,\t"); char *offset_rs1_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(offset_rs1_str, "("); char *rs1_str = strtok(NULL, ")");
            int rs2 = get_register_number(rs2_str); int rs1 = get_register_number(rs1_str); int offset = atoi(offset_str);
            if (rs2 == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'sw'.\n", current_address); continue; }
            uint32_t imm_11_5 = (offset >> 5) & 0x7F; uint32_t imm_4_0 = offset & 0x1F;
            instruction_binary = (imm_11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b010 << 12) | (imm_4_0 << 7) | 0b0100011;

        // --- Instruções tipo B (beq, bne) - AGORA COM RESOLUÇÃO DE LABEL ---
        } else if (strcmp(instr, "beq") == 0 || strcmp(instr, "bne") == 0) {
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");
            char *target_label = strtok(NULL, " ,\t"); // Agora é o nome do label

            int rs1 = get_register_number(rs1_str);
            int rs2 = get_register_number(rs2_str);
            int target_address = find_label_address(target_label);

            if (rs1 == -1 || rs2 == -1) {
                fprintf(stderr, "Erro em 0x%04X: Registrador inválido para '%s'.\n", current_address, instr);
                continue;
            }
            if (target_address == -1) {
                fprintf(stderr, "Erro em 0x%04X: Label '%s' não encontrado.\n", current_address, target_label);
                continue;
            }

            // O offset é relativo ao PC, e o PC já avançou 4 bytes para a próxima instrução
            int offset = target_address - current_address; 
            
            // Verifica se o offset é múltiplo de 2
            if (offset % 2 != 0) {
                 fprintf(stderr, "Erro em 0x%04X: Offset do branch '%s' não é múltiplo de 2.\n", current_address, target_label);
                 continue;
            }

            // O imediato do branch é 13 bits, mas o bit 0 é sempre 0 (salta endereços pares)
            // Divide por 2 para obter o valor que será codificado nos bits do imediato
            offset = offset / 2;

            // Montar o imediato do tipo B (imm[12|10:5], rs2, rs1, funct3, imm[4:1|11], opcode)
            uint32_t imm_12 = (offset >> 11) & 0x1; // Bit 12 (sign bit) do offset
            uint32_t imm_10_5 = (offset >> 4) & 0x3F; // Bits 10 a 5 do offset
            uint32_t imm_4_1 = offset & 0xF;         // Bits 4 a 1 do offset
            uint32_t imm_11 = (offset >> 10) & 0x1;  // Bit 11 do offset

            if (strcmp(instr, "beq") == 0) {
                instruction_binary = (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (imm_4_1 << 8) | (imm_11 << 7) | 0b1100011;
            } else { // bne
                instruction_binary = (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b001 << 12) | (imm_4_1 << 8) | (imm_11 << 7) | 0b1100011;
            }
        
        // --- Instruções tipo U (lui) ---
        } else if (strcmp(instr, "lui") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *imm_str = strtok(NULL, " ,\t");
            int rd = get_register_number(rd_str); int imm = atoi(imm_str);
            if (rd == -1) { fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'lui'.\n", current_address); continue; }
            instruction_binary = ((imm & 0xFFFFF) << 12) | (rd << 7) | 0b0110111;

        // --- Instruções tipo J (jal) - AGORA COM RESOLUÇÃO DE LABEL ---
        } else if (strcmp(instr, "jal") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *target_label = strtok(NULL, " ,\t"); // Agora é o nome do label

            int rd = get_register_number(rd_str);
            int target_address = find_label_address(target_label);

            if (rd == -1) {
                fprintf(stderr, "Erro em 0x%04X: Registrador inválido para 'jal'.\n", current_address);
                continue;
            }
            if (target_address == -1) {
                fprintf(stderr, "Erro em 0x%04X: Label '%s' não encontrado.\n", current_address, target_label);
                continue;
            }

            // O offset é relativo ao PC, e o PC já avançou 4 bytes para a próxima instrução
            int offset = target_address - current_address;
            
            // O imediato do jal é 21 bits, mas o bit 0 é sempre 0 (salta endereços pares)
            // Divide por 2 para obter o valor que será codificado nos bits do imediato
            offset = offset / 2;

            // Montar o imediato do tipo J (imm[20|10:1|11|19:12], rd, opcode=1101111)
            uint32_t imm_20 = (offset >> 19) & 0x1;   // Bit 20 (sign bit) do offset
            uint32_t imm_10_1 = (offset >> 0) & 0x3FF; // Bits 10 a 1 do offset
            uint32_t imm_11 = (offset >> 10) & 0x1;   // Bit 11 do offset
            uint32_t imm_19_12 = (offset >> 11) & 0xFF; // Bits 19 a 12 do offset

            instruction_binary = (imm_20 << 31) | (imm_19_12 << 21) | (imm_11 << 20) | (imm_10_1 << 12) | (rd << 7) | 0b1101111;

        // --- Instruções tipo I (jalr) ---
        } else if (strcmp(instr, "jalr") == 0) {
            char *rd_str = strtok(NULL, " ,\t"); char *offset_rs1_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(offset_rs1_str, "("); char *rs1_str = strtok(NULL, ")");
            int rd = get_register_number(rd_str); int rs1 = get_register_number(rs1_str); int offset = atoi(offset_str);
            if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) { fprintf(stderr, "Erro em 0x%04X: Parâmetro inválido para 'jalr'.\n", current_address); continue; }
            instruction_binary = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b1100111;

        } else {
            fprintf(stderr, "Instrução desconhecida em 0x%04X: %s\n", current_address, instr);
            continue;
        }

        // Converte o binário de 32 bits para string e escreve no arquivo .mif
        for (int i = 31; i >= 0; i--) {
            opcode_str[31 - i] = ((instruction_binary >> i) & 1) ? '1' : '0';
        }
        opcode_str[32] = '\0';

        fprintf(output_file, "  [%04X] : %s;\n", current_address, opcode_str);
        current_address += 4; // Incrementa o endereço para a próxima instrução
    }

    fprintf(output_file, "END;\n");

    fclose(input_file);
    fclose(output_file);
    printf("Montagem concluída! Arquivo '%s' gerado.\n", output_filename);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada.asm> <arquivo_saida.mif>\n", argv[0]);
        return 1;
    }

    // Primeira Passagem: Coleta labels
    first_pass(argv[1]);

    // Segunda Passagem: Monta o código e resolve labels
    second_pass(argv[1], argv[2]);

    return 0;
}