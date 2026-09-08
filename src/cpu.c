#include "cpu.h"
#include <iso646.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int load_program(Bus *bus, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < 0 || (size_t)size > bus->ram_size) {
        fclose(f);
        return -1;
    }

    size_t n = fread(bus->ram, 1, (size_t)size, f);
    fclose(f);

    return (n == (size_t)size) ? 0 : -1;
}

static inline int32_t sext(uint32_t x,
                           int bits) { // étend le bit de signe lors d'un cast
    return (int32_t)(x << (32 - bits)) >> (32 - bits);
}

void step(Bus *bus, CPU *cpu) {
    uint32_t inst;
    memcpy(&inst, &(bus->ram[cpu->pc]), 4);

    uint32_t opcode = (inst >> 0) & 0x7F;
    uint32_t rd = (inst >> 7) & 0x1F;
    uint32_t funct3 = (inst >> 12) & 0x07;
    uint32_t rs1 = (inst >> 15) & 0x1F;
    uint32_t rs2 = (inst >> 20) & 0x1F;
    uint32_t funct7 = (inst >> 25) & 0x7F;

    int32_t imm_i = sext((inst >> 20) & 0xFFF, 12);
    int32_t imm_s = sext(((inst >> 25) & 0x7F) << 5 | (inst >> 7) & 0x1F, 12);
    int32_t imm_b =
        sext(((inst >> 31) & 0x01) << 12 | ((inst >> 7) & 0x01) << 11 |
                 ((inst >> 25) & 0x3F) << 5 | ((inst >> 8) & 0x0F) << 1,
             13);
    int32_t imm_u = (int32_t)(inst & 0xFFFFF000);
    int32_t imm_j =
        sext(((inst >> 31) & 0x01) << 20 | ((inst >> 12) & 0xFF) << 12 |
                 ((inst >> 20) & 0x01) << 11 | ((inst >> 21) & 0x3FF) << 1,
             21);

    int32_t pc_next = cpu->pc + 4;

    switch (opcode) {
    case 0x33: // OP
        switch (funct3) {
        case 0x00: // ADD/SUB
            cpu->x[rd] = (funct7 & 0x20) ? cpu->x[rs1] - cpu->x[rs2]
                                         : cpu->x[rs1] + cpu->x[rs2];
            break;
        case 0x01: // SLL
            cpu->x[rd] = cpu->x[rs1] << (cpu->x[rs2] & 0x1F);
            break;
        case 0x02: // SLT
            cpu->x[rd] = ((int32_t)cpu->x[rs1] < (int32_t)cpu->x[rs2]) ? 1 : 0;
            break;
        case 0x03: // SLTU
            cpu->x[rd] = (cpu->x[rs1] < cpu->x[rs2]) ? 1 : 0;
            break;
        case 0x04: // XOR
            cpu->x[rd] = cpu->x[rs1] ^ cpu->x[rs2];
            break;
        case 0x05: // SRL/SRA
            cpu->x[rd] = (funct7 & 0x20)
                             ? (int32_t)cpu->x[rs1] >> (cpu->x[rs2] & 0x1F)
                             : cpu->x[rs1] >> (cpu->x[rs2] & 0x1F);
            break;
        case 0x06: // OR
            cpu->x[rd] = cpu->x[rs1] | cpu->x[rs2];
            break;
        case 0x07: // AND
            cpu->x[rd] = cpu->x[rs1] & cpu->x[rs2];
            break;
        }
        break;
    case 0x13: // OP-IMM
        switch (funct3) {
        case 0x00: // ADDI
            cpu->x[rd] = cpu->x[rs1] + imm_i;
            break;
        case 0x01: // SLLI
            cpu->x[rd] = cpu->x[rs1] << (imm_i & 0x1F);
            break;
        case 0x02: // SLTI
            cpu->x[rd] = ((int32_t)cpu->x[rs1] < imm_i) ? 1 : 0;
            break;
        case 0x03: // SLTIU
            cpu->x[rd] = (cpu->x[rs1] < (uint32_t)imm_i) ? 1 : 0;
            break;
        case 0x04: // XORI
            cpu->x[rd] = cpu->x[rs1] ^ imm_i;
            break;
        case 0x05: // SRLI/SRAI
            cpu->x[rd] = (funct7 & 0x20)
                             ? (int32_t)cpu->x[rs1] >> (imm_i & 0x1F)
                             : cpu->x[rs1] >> (imm_i & 0x1F);
            break;
        case 0x06: // ORI
            cpu->x[rd] = cpu->x[rs1] | imm_i;
            break;
        case 0x07: // ANDI
            cpu->x[rd] = cpu->x[rs1] & imm_i;
            break;
        }
        break;
    case 0x6F: // JAL
        pc_next = cpu->pc + imm_j;
        cpu->x[rd] = cpu->pc + 4;
        break;
    case 0x67: // JALR
        pc_next = (cpu->x[rs1] + imm_i) & 0xFFFFFFFE;
        cpu->x[rd] = cpu->pc + 4;
        break;
    case 0x63: // BRANCH
        switch (funct3) {
        case 0x00: // BEQ
            if (cpu->x[rs1] == cpu->x[rs2])
                pc_next = cpu->pc + imm_b;
            break;
        case 0x01: // BNE
            if (cpu->x[rs1] != cpu->x[rs2])
                pc_next = cpu->pc + imm_b;
            break;
        case 0x04: // BLT
            if ((int32_t)cpu->x[rs1] < (int32_t)cpu->x[rs2])
                pc_next = cpu->pc + imm_b;
            break;
        case 0x05: // BGE
            if ((int32_t)cpu->x[rs1] >= (int32_t)cpu->x[rs2])
                pc_next = cpu->pc + imm_b;
            break;
        case 0x06: // BLTU
            if (cpu->x[rs1] < cpu->x[rs2])
                pc_next = cpu->pc + imm_b;
            break;
        case 0x07: // BGEU
            if (cpu->x[rs1] >= cpu->x[rs2])
                pc_next = cpu->pc + imm_b;
            break;
        }
        break;
    case 0x03: // LOAD
        switch (funct3) {
        case 0x00: // LB
            cpu->x[rd] = sext((bus->ram[cpu->x[rs1] + imm_i]) & 0xFF, 8);
            break;
        case 0x01: { // LH
            uint16_t w;
            memcpy(&w, &bus->ram[cpu->x[rs1] + imm_i], sizeof(w));
            cpu->x[rd] = sext(w, sizeof(w) * 8);
            break;
        }
        case 0x02: { // LW
            uint32_t w;
            memcpy(&w, &bus->ram[cpu->x[rs1] + imm_i], sizeof(w));
            cpu->x[rd] = sext(w, sizeof(w) * 8);
            break;
        }
        case 0x04: // LBU
            cpu->x[rd] = (bus->ram[cpu->x[rs1] + imm_i]) & 0xFF;
            break;
        case 0x05: { // LHU
            uint16_t w;
            memcpy(&w, &bus->ram[cpu->x[rs1] + imm_i], sizeof(w));
            cpu->x[rd] = w;
            break;
        }
        }
        break;
    case 0x23: // STORE
        switch (funct3) {
        case 0x00: // SB
            bus->ram[cpu->x[rs1] + imm_s] = cpu->x[rs2] & 0xFF;
            break;
        case 0x01: { // SH
            uint16_t w = cpu->x[rs2] & 0xFFFF;
            memcpy(&bus->ram[cpu->x[rs1] + imm_s], &w, sizeof(w));
            break;
        }
        case 0x02: { // SW
            uint32_t w = cpu->x[rs2];
            memcpy(&bus->ram[cpu->x[rs1] + imm_s], &w, sizeof(w));
            break;
        } break;
        }
        break;
    }
    // le r0 doit rester à 0
    cpu->x[0] = 0;

    cpu->pc = pc_next;
}
