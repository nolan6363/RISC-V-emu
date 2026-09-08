#include "cpu.h"
#include <stdint.h>
#include <string.h>

void decode(Bus *bus, CPU *cpu) {
    uint32_t inst;
    memcpy(&inst, &(bus->ram[cpu->pc]), 4);

    uint32_t opcode = (inst >> 0) & 0x7F;
    uint32_t rd = (inst >> 7) & 0x1F;
    uint32_t funct3 = (inst >> 12) & 0x07;
    uint32_t rs1 = (inst >> 15) & 0x1F;
    uint32_t rs2 = (inst >> 20) & 0x1F;
    uint32_t funct7 = (inst >> 25) & 0x7F;

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
    }
}
