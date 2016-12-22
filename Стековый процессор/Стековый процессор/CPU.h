#pragma once

#include "stack.h" 
#include "ALU.h"

enum CPU_ERRORS {
	NULL_CPU_POINTER = -1,
	AX_IS_NOT_FINITE = 1,
	BX_IS_NOT_FINITE,
	CX_IS_NOT_FINITE,
	DX_IS_NOT_FINITE,
	NULL_RAM_POINTER,
	NULL_CACHE_POINTER
};

struct CPU {
	stack* st;
	double ax;
	double bx;
	double cx;
	double dx;
	int i;
	double* cache;
	char* ram;
	unsigned int ram_size;
	stack* ret_add;
};

CPU* initCPU(unsigned int cacheSize, unsigned int ramSize);
int deleteCPU(CPU** cpu);
int dumpCPU(CPU* cpu, FILE* dumpFile);
int assertCPU(CPU* cpu, const char* function);
int errorCPUprint(FILE* errorFile, const int code);
int startProgram(CPU* cpu);

int push(CPU* cpu, int type, double value);
int pop(CPU* cpu, int type);
double in(CPU* cpu);
double out(CPU* cpu);
double add(CPU* cpu);
double mul(CPU* cpu);
double sub(CPU* cpu);
double div(CPU* cpu);
double cpu_sin(CPU* cpu);
double cpu_cos(CPU* cpu);
double cpu_sqrt(CPU* cpu);
double cpu_pow(CPU* cpu);
int call(CPU* cpu, char** ptr, int pos);
int ret(CPU* cpu, char** ptr);
int jmp(CPU* cpu, char** ptr, int pos);
int je(CPU* cpu, char** ptr, int pos);
int jne(CPU* cpu, char** ptr, int pos);
int ja(CPU* cpu, char** ptr, int pos);
int jae(CPU* cpu, char** ptr, int pos);
int jb(CPU* cpu, char** ptr, int pos);
int jbe(CPU* cpu, char** ptr, int pos);

char readChar(char** ptr);
double readDouble(char** ptr);
int readInt(char** ptr);