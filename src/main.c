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

int main(int argc, char *argv[]) {
    CPU cpu;
    init_cpu(&cpu);
    printf("CPU initialized\n");

    Bus bus;
    init_bus(&bus);
    printf("Bus initialized\n");

    return EXIT_SUCCESS;
}
