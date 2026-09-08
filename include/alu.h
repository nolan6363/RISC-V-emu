#ifndef ALU_H
#define ALU_H

#include "cpu.h"

// Integer Register-Register Instructions
void add(CPU *cpu, size_t rs1, size_t rs2,
         size_t rd); // addition of rs1 and rs2
void sub();          // substraction of rs2 from rs1
void slt();  // signed compare, writing 1 to rd if rs1 < rs2, 0 otherwise
void sltu(); // unsigned compare, writing 1 to rd if rs1 < rs2, 0 otherwise
void and();  // bitwise logical operation
void or ();  // bitwise logical operation
void xor (); // bitwise logical operation
void sll();  // shift logical left on the value in rs1 by lower 5 bits of rs2
void slr();  // shift logical rigth on the value in rs1 by lower 5 bits of rs2
void sra(); // shift arithmetic right on the value in rs1 by lower 5 bits of rs2

#endif // !ALU_H
