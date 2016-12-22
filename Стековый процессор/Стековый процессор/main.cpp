#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ALU.h"
#include "CPU.h"

int main() {

	CPU* cpu = initCPU(CACHE_SIZE, MAX_RAM_SIZE);
	
	cpu->ram_size = assembler("input_se.txt", cpu->ram, "code.txt");
	disassembler("code.txt", "output.txt");

	startProgram(cpu);

	deleteCPU(&cpu);
	
	return 0;
}