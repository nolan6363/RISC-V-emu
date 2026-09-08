#ifndef CPU_H
#define CPU_H

#include <stddef.h>
#include <stdint.h>

#define RAM_BASE 0x80000000u
#define RAM_SIZE 0x08000000u // 128Mo

#define HALT_ADDR 0xFFFFFFFE

typedef struct {
    uint32_t x[32];
    uint32_t pc;
} CPU;

typedef struct {
    uint8_t *ram;
    size_t ram_size;
} Bus;

void step(Bus *bus, CPU *cpu);
int load_program(Bus *bus, const char *path);

#endif // !CPU_H
