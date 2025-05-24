#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // Para uint32_t

// Mapeamento simples de registradores para seus números
// Você pode expandir isso para suportar os nomes simbólicos (a0, t0, etc.)
int get_register_number(const char *reg_name) {
    if (reg_name[0] == 'x') {
        return atoi(&reg_name[1]);
    }
    // Adicione mais mapeamentos aqui se quiser suportar nomes simbólicos
    // Ex: if (strcmp(reg_name, "zero") == 0) return 0;
    // Ex: if (strcmp(reg_name, "a0") == 0) return 10;
    // Por simplicidade, assumimos x0, x1, etc.
    return -1; // Erro
}

// Converte um número decimal para uma string binária de n bits
void dec_to_bin_n_bits(int n, int num, char *bin_str) {
    for (int i = n - 1; i >= 0; i--) {
        bin_str[n - 1 - i] = ((num >> i) & 1) ? '1' : '0';
    }
    bin_str[n] = '\0';
}

// Função principal para montar o código
void assemble(const char *input_filename, const char *output_filename) {
    FILE *input_file = NULL;
    FILE *output_file = NULL;
    char line[256];
    char opcode_str[33]; // Para armazenar o binário de 32 bits + '\0'
    int line_number = 0;

    input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return;
    }

    output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(input_file);
        return;
    }

    // Escreve o cabeçalho do arquivo .mif (opcional, mas comum para simuladores)
    fprintf(output_file, "DEPTH = 256;\n"); // Exemplo: 256 palavras de profundidade
    fprintf(output_file, "WIDTH = 32;\n"); // Largura de 32 bits
    fprintf(output_file, "ADDRESS_RADIX = HEX;\n");
    fprintf(output_file, "DATA_RADIX = BIN;\n");
    fprintf(output_file, "CONTENT BEGIN\n");

    while (fgets(line, sizeof(line), input_file) != NULL) {
        line_number++;
        // Ignorar linhas vazias ou de comentário
        if (strlen(line) <= 1 || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Remover quebra de linha
        line[strcspn(line, "\n")] = 0;

        // Tokenização da linha
        char *token = strtok(line, " ,\t");
        if (token == NULL) continue;

        char *instr = token;
        uint32_t instruction_binary = 0;

        // --- Instruções tipo R (add, sub, xor, or, and, slli, srli) ---
        // add rd, rs1, rs2
        if (strcmp(instr, "add") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int rs2 = get_register_number(rs2_str);

            if (rd == -1 || rs1 == -1 || rs2 == -1) {
                fprintf(stderr, "Erro na linha %d: Registrador inválido para 'add'.\n", line_number);
                continue;
            }
            // add: funct7=0000000, rs2, rs1, funct3=000, rd, opcode=0110011
            instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;

        } else if (strcmp(instr, "sub") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int rs2 = get_register_number(rs2_str);

            if (rd == -1 || rs1 == -1 || rs2 == -1) {
                fprintf(stderr, "Erro na linha %d: Registrador inválido para 'sub'.\n", line_number);
                continue;
            }
            // sub: funct7=0100000, rs2, rs1, funct3=000, rd, opcode=0110011
            instruction_binary = (0b0100000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0110011;

        } else if (strcmp(instr, "xor") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int rs2 = get_register_number(rs2_str);

            if (rd == -1 || rs1 == -1 || rs2 == -1) {
                fprintf(stderr, "Erro na linha %d: Registrador inválido para 'xor'.\n", line_number);
                continue;
            }
            // xor: funct7=0000000, rs2, rs1, funct3=100, rd, opcode=0110011
            instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b100 << 12) | (rd << 7) | 0b0110011;

        } else if (strcmp(instr, "or") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int rs2 = get_register_number(rs2_str);

            if (rd == -1 || rs1 == -1 || rs2 == -1) {
                fprintf(stderr, "Erro na linha %d: Registrador inválido para 'or'.\n", line_number);
                continue;
            }
            // or: funct7=0000000, rs2, rs1, funct3=110, rd, opcode=0110011
            instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b110 << 12) | (rd << 7) | 0b0110011;

        } else if (strcmp(instr, "and") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int rs2 = get_register_number(rs2_str);

            if (rd == -1 || rs1 == -1 || rs2 == -1) {
                fprintf(stderr, "Erro na linha %d: Registrador inválido para 'and'.\n", line_number);
                continue;
            }
            // and: funct7=0000000, rs2, rs1, funct3=111, rd, opcode=0110011
            instruction_binary = (0b0000000 << 25) | (rs2 << 20) | (rs1 << 15) | (0b111 << 12) | (rd << 7) | 0b0110011;

        } else if (strcmp(instr, "slli") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *shamt_str = strtok(NULL, " ,\t"); // Shift amount (imediato)

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int shamt = atoi(shamt_str); // Converte para inteiro

            if (rd == -1 || rs1 == -1 || shamt < 0 || shamt >= 32) {
                fprintf(stderr, "Erro na linha %d: Parâmetro inválido para 'slli'.\n", line_number);
                continue;
            }
            // slli: funct7=0000000, shamt, rs1, funct3=001, rd, opcode=0010011
            instruction_binary = (0b0000000 << 25) | (shamt << 20) | (rs1 << 15) | (0b001 << 12) | (rd << 7) | 0b0010011;

        } else if (strcmp(instr, "srli") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *shamt_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int shamt = atoi(shamt_str);

            if (rd == -1 || rs1 == -1 || shamt < 0 || shamt >= 32) {
                fprintf(stderr, "Erro na linha %d: Parâmetro inválido para 'srli'.\n", line_number);
                continue;
            }
            // srli: funct7=0000000, shamt, rs1, funct3=101, rd, opcode=0010011
            instruction_binary = (0b0000000 << 25) | (shamt << 20) | (rs1 << 15) | (0b101 << 12) | (rd << 7) | 0b0010011;


        // --- Instruções tipo I (addi, lw) ---
        // addi rd, rs1, imm
        } else if (strcmp(instr, "addi") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *rs1_str = strtok(NULL, " ,\t");
            char *imm_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int imm = atoi(imm_str); // Imediato

            if (rd == -1 || rs1 == -1 || imm < -2048 || imm > 2047) { // 12 bits com sinal
                fprintf(stderr, "Erro na linha %d: Parâmetro inválido para 'addi'.\n", line_number);
                continue;
            }
            // addi: imm[11:0], rs1, funct3=000, rd, opcode=0010011
            instruction_binary = ((imm & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b0010011;

        // lw rd, offset(rs1)
        } else if (strcmp(instr, "lw") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *offset_rs1_str = strtok(NULL, " ,\t"); // Ex: "0(x2)"

            char *offset_str = strtok(offset_rs1_str, "(");
            char *rs1_str = strtok(NULL, ")");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int offset = atoi(offset_str);

            if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) {
                fprintf(stderr, "Erro na linha %d: Parâmetro inválido para 'lw'.\n", line_number);
                continue;
            }
            // lw: imm[11:0], rs1, funct3=010, rd, opcode=0000011
            instruction_binary = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b010 << 12) | (rd << 7) | 0b0000011;

        // --- Instruções tipo S (sw) ---
        // sw rs2, offset(rs1)
        } else if (strcmp(instr, "sw") == 0) {
            char *rs2_str = strtok(NULL, " ,\t");
            char *offset_rs1_str = strtok(NULL, " ,\t"); // Ex: "0(x2)"

            char *offset_str = strtok(offset_rs1_str, "(");
            char *rs1_str = strtok(NULL, ")");

            int rs2 = get_register_number(rs2_str);
            int rs1 = get_register_number(rs1_str);
            int offset = atoi(offset_str);

            if (rs2 == -1 || rs1 == -1 || offset < -2048 || offset > 2047) {
                fprintf(stderr, "Erro na linha %d: Parâmetro inválido para 'sw'.\n", line_number);
                continue;
            }
            // sw: imm[11:5], rs2, rs1, funct3=010, imm[4:0], opcode=0100011
            uint32_t imm_11_5 = (offset >> 5) & 0x7F; // Bits 11 a 5 do imediato
            uint32_t imm_4_0 = offset & 0x1F;       // Bits 4 a 0 do imediato
            instruction_binary = (imm_11_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b010 << 12) | (imm_4_0 << 7) | 0b0100011;

        // --- Instruções tipo B (beq, bne) ---
        // beq rs1, rs2, offset (sem labels, offset direto)
        } else if (strcmp(instr, "beq") == 0) {
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(NULL, " ,\t");

            int rs1 = get_register_number(rs1_str);
            int rs2 = get_register_number(rs2_str);
            int offset = atoi(offset_str); // Offset em bytes

            // A instrução de branch precisa de um offset de 13 bits (múltiplo de 2, então o bit 0 é sempre 0)
            // imm[12|10:5], rs2, rs1, funct3, imm[4:1|11], opcode
            // Beq: funct3=000, opcode=1100011

            // Montar o imediato do tipo B
            uint32_t imm_12 = (offset >> 12) & 0x1;
            uint32_t imm_10_5 = (offset >> 5) & 0x3F;
            uint32_t imm_4_1 = (offset >> 1) & 0xF;
            uint32_t imm_11 = (offset >> 11) & 0x1;

            instruction_binary = (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b000 << 12) | (imm_4_1 << 8) | (imm_11 << 7) | 0b1100011;

        } else if (strcmp(instr, "bne") == 0) {
            char *rs1_str = strtok(NULL, " ,\t");
            char *rs2_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(NULL, " ,\t");

            int rs1 = get_register_number(rs1_str);
            int rs2 = get_register_number(rs2_str);
            int offset = atoi(offset_str);

            // Bne: funct3=001, opcode=1100011
            uint32_t imm_12 = (offset >> 12) & 0x1;
            uint32_t imm_10_5 = (offset >> 5) & 0x3F;
            uint32_t imm_4_1 = (offset >> 1) & 0xF;
            uint32_t imm_11 = (offset >> 11) & 0x1;

            instruction_binary = (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) | (rs1 << 15) | (0b001 << 12) | (imm_4_1 << 8) | (imm_11 << 7) | 0b1100011;


        // --- Instruções tipo U (lui) ---
        // lui rd, imm
        } else if (strcmp(instr, "lui") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *imm_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int imm = atoi(imm_str); // Imediato (20 bits)

            if (rd == -1) {
                fprintf(stderr, "Erro na linha %d: Registrador inválido para 'lui'.\n", line_number);
                continue;
            }
            // lui: imm[31:12], rd, opcode=0110111
            instruction_binary = ((imm & 0xFFFFF) << 12) | (rd << 7) | 0b0110111;

        // --- Instruções tipo J (jal) ---
        // jal rd, offset (sem labels, offset direto)
        } else if (strcmp(instr, "jal") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *offset_str = strtok(NULL, " ,\t");

            int rd = get_register_number(rd_str);
            int offset = atoi(offset_str); // Offset em bytes

            if (rd == -1) {
                fprintf(stderr, "Erro na linha %d: Registrador inválido para 'jal'.\n", line_number);
                continue;
            }
            // jal: imm[20|10:1|11|19:12], rd, opcode=1101111
            // O imediato é complicado para o tipo J
            uint32_t imm_20 = (offset >> 20) & 0x1;
            uint32_t imm_10_1 = (offset >> 1) & 0x3FF;
            uint32_t imm_11 = (offset >> 11) & 0x1;
            uint32_t imm_19_12 = (offset >> 12) & 0xFF;

            instruction_binary = (imm_20 << 31) | (imm_19_12 << 21) | (imm_11 << 20) | (imm_10_1 << 12) | (rd << 7) | 0b1101111;

        // --- Instruções tipo I (jalr) ---
        // jalr rd, offset(rs1)
        } else if (strcmp(instr, "jalr") == 0) {
            char *rd_str = strtok(NULL, " ,\t");
            char *offset_rs1_str = strtok(NULL, " ,\t");

            char *offset_str = strtok(offset_rs1_str, "(");
            char *rs1_str = strtok(NULL, ")");

            int rd = get_register_number(rd_str);
            int rs1 = get_register_number(rs1_str);
            int offset = atoi(offset_str);

            if (rd == -1 || rs1 == -1 || offset < -2048 || offset > 2047) {
                fprintf(stderr, "Erro na linha %d: Parâmetro inválido para 'jalr'.\n", line_number);
                continue;
            }
            // jalr: imm[11:0], rs1, funct3=000, rd, opcode=1100111
            instruction_binary = ((offset & 0xFFF) << 20) | (rs1 << 15) | (0b000 << 12) | (rd << 7) | 0b1100111;


        } else {
            fprintf(stderr, "Instrução desconhecida na linha %d: %s\n", line_number, instr);
            continue;
        }

        // Converte o binário de 32 bits para string e escreve no arquivo .mif
        // Formatamos para 32 bits, preenchendo com zeros à esquerda
        for (int i = 31; i >= 0; i--) {
            opcode_str[31 - i] = ((instruction_binary >> i) & 1) ? '1' : '0';
        }
        opcode_str[32] = '\0';

        fprintf(output_file, "  [%04X] : %s;\n", (line_number -1 ) * 4, opcode_str); // Endereço em HEX, instrução em BIN
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

    assemble(argv[1], argv[2]);

    return 0;
}