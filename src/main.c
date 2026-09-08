#include "cpu.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void init_cpu(CPU *cpu) {
    for (int i = 0; i < 32; i++) {
        cpu->x[i] = 0;
    }
    cpu->pc = 0;
}

void init_bus(Bus *bus) {
    bus->ram = (uint8_t *)malloc(sizeof(uint8_t) * RAM_SIZE);
    bus->ram_size = RAM_SIZE;
}

void print_regs(CPU *cpu) {
    for (int i = 0; i < 32; i++) {
        printf("r%d = 0x%X\n", i, cpu->x[i]);
    }
}

int main(int argc, char *argv[]) {
    CPU cpu;
    init_cpu(&cpu);
    printf("CPU initialized\n");

    Bus bus;
    init_bus(&bus);
    printf("Bus initialized\n");

    if (load_program(&bus, "data/loadstore.bin")) {
        printf("erreur de chargement du programme\n");
    } else {
        printf("programme chargé avec succès\n");
    }

    do {
        step(&bus, &cpu);
    } while (cpu.pc != HALT_ADDR);

    print_regs(&cpu);

    return EXIT_SUCCESS;
}
