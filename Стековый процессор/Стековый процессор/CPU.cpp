#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "CPU.h"
#include "ALU.h"

#define ASSERT_OK(struct, wat, function) {		  \
    if (assert##struct(wat, function) != OK) {    \
        assert(false);							  \
	}											  \
}

#define ASSERT_PARAMETERS(struct, function, status, error) {			\
    if (assertParameters(struct, function, status, error) != OK) {		\
        assert(false);													\
	}																	\
}

CPU* initCPU(unsigned int cacheSize, unsigned int ramSize) {
	assert(cacheSize > 0 && "cachSize < 0");
	assert(ramSize > 0 && "ramSize < 0");

	CPU* cpu = (CPU*)calloc(1, sizeof(CPU));

	cpu->st = initStack(0);
	cpu->ax = 0;
	cpu->bx = 0;
	cpu->cx = 0;
	cpu->dx = 0;
	cpu->i = 0;
	cpu->cache = (double*)calloc(cacheSize, sizeof(double));
	cpu->ram = (char*)calloc(ramSize, sizeof(char));
	cpu->ram_size = 0;
	cpu->ret_add = initStack(0);

	ASSERT_OK(CPU, cpu, __func__);
	return cpu;
}

int deleteCPU(CPU** cpu) {
	deleteStack(&((*cpu)->st));
	
	(*cpu)->ax = 0;
	(*cpu)->bx = 0;
	(*cpu)->cx = 0;
	(*cpu)->dx = 0;
	(*cpu)->i = 0;
	(*cpu)->cache = NULL;
	(*cpu)->ram = NULL;
	(*cpu)->ram_size = 0;

	deleteStack(&((*cpu)->ret_add));

	free(*cpu);
	*cpu = NULL;

	return OK;
}

int dumpCPU(CPU* cpu, FILE* dumpFile) {

	fprintf(dumpFile, "=========== DUMP_CPU ===========\n");
	fprintf(dumpFile, "CPU = %p\n", cpu);
	fprintf(dumpFile, "CPU->st:\n");
	dumpStack(cpu->st, dumpFile);
	fprintf(dumpFile, "CPU->ax = %lf\n", cpu->ax);
	fprintf(dumpFile, "CPU->bx = %lf\n", cpu->bx);
	fprintf(dumpFile, "CPU->cx = %lf\n", cpu->cx);
	fprintf(dumpFile, "CPU->dx = %lf\n", cpu->dx);
	fprintf(dumpFile, "CPU->i = %d\n", cpu->i);
	fprintf(dumpFile, "CPU->cache = %p\n", cpu->cache);
	fprintf(dumpFile, "CPU->ram = %p\n", cpu->ram);
	fprintf(dumpFile, "CPU->ram_size = %d\n", cpu->ram_size);
	fprintf(dumpFile, "CPU->ret_add:\n");
	dumpStack(cpu->ret_add, dumpFile);

	fprintf(dumpFile, "\n================================\n");

	return OK;
}

int assertCPU(CPU* cpu, const char* function) {

	if (cpu && assertStack(cpu->st, __func__) == OK &&
		isfinite(cpu->ax) && isfinite(cpu->bx) && isfinite(cpu->cx) && isfinite(cpu->dx) &&
		cpu->ram && cpu->cache && assertStack(cpu->ret_add, __func__) == OK) {

		return OK;
	}

	FILE* assertFile = fopen("assert.txt", "a");  
	fprintf(assertFile, "=== Error in function: %s ===\n", function);

	if (!cpu) {
		errorCPUprint(assertFile, NULL_CPU_POINTER);
	}
	if (!isfinite(cpu->ax)) {
		errorCPUprint(assertFile, AX_IS_NOT_FINITE);
	}
	if (!isfinite(cpu->bx)) {
		errorCPUprint(assertFile, BX_IS_NOT_FINITE);
	}
	if (!isfinite(cpu->cx)) {
		errorCPUprint(assertFile, CX_IS_NOT_FINITE);
	}
	if (!isfinite(cpu->dx)) {
		errorCPUprint(assertFile, DX_IS_NOT_FINITE);
	}
	if (!cpu->ram) {
		errorCPUprint(assertFile, NULL_RAM_POINTER);
	}
	if (!cpu->cache) {
		errorCPUprint(assertFile, NULL_CACHE_POINTER);
	}

	fclose(assertFile);

	FILE* dumpFile = fopen("dump.txt", "a");
	dumpCPU(cpu, dumpFile);
	fclose(dumpFile);

	deleteCPU(&cpu);

	return VALIDATION_FAILS;
}

int assertParameters(CPU* cpu, const char* function, bool status, const char* error) {
	if (status) {
		return OK;
	}

	FILE* assertFile = fopen("assert.txt", "a");

	fprintf(assertFile, "=== Error in function: %s ===\n", function);
	fprintf(assertFile, "%s\n", error);

	fclose(assertFile);

	FILE* dumpFile = fopen("dump.txt", "a");
	dumpCPU(cpu, dumpFile);
	fclose(dumpFile);

	deleteCPU(&cpu);

	return VALIDATION_FAILS;
}

int errorCPUprint(FILE* errorFile, const int code) {
	switch (code) {
		case NULL_CPU_POINTER:
			fprintf(errorFile, "CPU = NULL\n");
			break;
		case AX_IS_NOT_FINITE:
			fprintf(errorFile, "CPU->ax is not finite\n");
			break;
		case BX_IS_NOT_FINITE:
			fprintf(errorFile, "CPU->bx is not finite");
			break;
		case CX_IS_NOT_FINITE:
			fprintf(errorFile, "CPU->cx is not finite");
			break;
		case DX_IS_NOT_FINITE:
			fprintf(errorFile, "CPU->dx is not finite");
			break;
		case NULL_RAM_POINTER:
			fprintf(errorFile, "CPU->ram = NULL\n");
			break;
		case NULL_CACHE_POINTER:
			fprintf(errorFile, "CPU->cache = NULL\n");
			break;
		default:
			break;
	}

	return code;
}

int push(CPU* cpu, int type, double value) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, 0 <= type && type <= 6, "Incorrect command push type!");
	ASSERT_PARAMETERS(cpu, __func__, isfinite(cpu->ax), "Value is not finite!");

	switch (type) {
		case 0:
			pushStack(cpu->st, value);
			break;
		case 1:
			pushStack(cpu->st, cpu->ax);
			break;
		case 2:
			pushStack(cpu->st, cpu->bx);
			break;
		case 3:
			pushStack(cpu->st, cpu->cx);
			break;
		case 4:
			pushStack(cpu->st, cpu->dx);
			break;
		case 5:
			pushStack(cpu->st, (double)cpu->i);
			break;
		case 6:
			pushStack(cpu->st, cpu->cache[cpu->i]);
			break;
		default:
			break;
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int pop(CPU* cpu, int type) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, 0 <= type && type <= 6, "Incorrect command pop type!");

	switch (type) {
		case 0:
			popStack(cpu->st);
			break;
		case 1:
			cpu->ax = popStack(cpu->st);
				break;
		case 2:
			cpu->bx = popStack(cpu->st);
			break;
		case 3:
			cpu->cx = popStack(cpu->st);
			break;
		case 4:
			cpu->dx = popStack(cpu->st);
			break;
		case 5:
			cpu->i = (int)popStack(cpu->st);
			break;
		case 6:
			cpu->cache[cpu->i] = popStack(cpu->st);
			break;
		default:
			break;
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

double in(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double value = 0;
	scanf("%lg", &value);

	pushStack(cpu->st, value);

	ASSERT_OK(CPU, cpu, __func__);
	return value;
}

double out(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double value = popStack(cpu->st);
	printf("%lf\n", value);

	ASSERT_OK(CPU, cpu, __func__);
	return value;
}

double add(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double first = popStack(cpu->st);
	double second = popStack(cpu->st);

	pushStack(cpu->st, first + second);

	ASSERT_OK(CPU, cpu, __func__);
	return first + second;
}

double mul(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double first = popStack(cpu->st);
	double second = popStack(cpu->st);

	pushStack(cpu->st, first * second);

	ASSERT_OK(CPU, cpu, __func__);
	return first * second;
}

double sub(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double first = popStack(cpu->st);
	double second = popStack(cpu->st);

	pushStack(cpu->st, second - first);

	ASSERT_OK(CPU, cpu, __func__);
	return second - first;
}

double div(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double first = popStack(cpu->st);
	double second = popStack(cpu->st);

	pushStack(cpu->st, second / first);

	ASSERT_OK(CPU, cpu, __func__);
	return second / first;
}

double cpu_sin(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double value = popStack(cpu->st);

	pushStack(cpu->st, sin(value));

	ASSERT_OK(CPU, cpu, __func__);
	return sin(value);
}

double cpu_cos(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double value = popStack(cpu->st);

	pushStack(cpu->st, cos(value));

	ASSERT_OK(CPU, cpu, __func__);
	return cos(value);
}

double cpu_sqrt(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double value = popStack(cpu->st);

	pushStack(cpu->st, sqrt(value));

	ASSERT_OK(CPU, cpu, __func__);
	return sqrt(value);
}

double cpu_pow(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	double first = popStack(cpu->st);
	double second = popStack(cpu->st);

	pushStack(cpu->st, pow(second, first));

	ASSERT_OK(CPU, cpu, __func__);
	return pow(second, first);
}

int call(CPU* cpu, char** ptr, int pos) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");
	ASSERT_PARAMETERS(cpu, __func__, pos >= 0, "pos < 0");

	pushStack(cpu->ret_add, *ptr - cpu->ram);
	*ptr = cpu->ram + pos;

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int ret(CPU* cpu, char** ptr) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");

	*ptr = cpu->ram + (int)popStack(cpu->ret_add);

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int jmp(CPU* cpu, char** ptr, int pos) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");
	ASSERT_PARAMETERS(cpu, __func__, pos >= 0, "pos < 0");

	*ptr = cpu->ram + pos;

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int je(CPU* cpu, char** ptr, int pos) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");
	ASSERT_PARAMETERS(cpu, __func__, pos >= 0, "pos < 0");

	double first = popStack(cpu->st);
	double second = peekStack(cpu->st);
	pushStack(cpu->st, first);

	if (first == second) {
		*ptr = cpu->ram + pos;
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int jne(CPU* cpu, char** ptr, int pos) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");
	ASSERT_PARAMETERS(cpu, __func__, pos >= 0, "pos < 0");

	double first = popStack(cpu->st);
	double second = peekStack(cpu->st);
	pushStack(cpu->st, first);

	if (first != second) {
		*ptr = cpu->ram + pos;
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int ja(CPU* cpu, char** ptr, int pos) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");
	ASSERT_PARAMETERS(cpu, __func__, pos >= 0, "pos < 0");

	double second = popStack(cpu->st);
	double first = peekStack(cpu->st);
	pushStack(cpu->st, second);

	if (first > second) {
		*ptr = cpu->ram + pos;
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int jae(CPU* cpu, char** ptr, int pos) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");
	ASSERT_PARAMETERS(cpu, __func__, pos >= 0, "pos < 0");

	double second = popStack(cpu->st);
	double first = peekStack(cpu->st);
	pushStack(cpu->st, second);

	if (first >= second) {
		*ptr = cpu->ram + pos;
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int jb(CPU* cpu, char** ptr, int pos) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");
	ASSERT_PARAMETERS(cpu, __func__, pos >= 0, "pos < 0");

	double second = popStack(cpu->st);
	double first = peekStack(cpu->st);
	pushStack(cpu->st, second);

	if (first < second) {
		*ptr = cpu->ram + pos;
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

int jbe(CPU* cpu, char** ptr, int pos) {
	ASSERT_OK(CPU, cpu, __func__);
	ASSERT_PARAMETERS(cpu, __func__, ptr != NULL, "ptr = NULL");
	ASSERT_PARAMETERS(cpu, __func__, pos >= 0, "pos < 0");

	double second = popStack(cpu->st);
	double first = peekStack(cpu->st);
	pushStack(cpu->st, second);

	if (first <= second) {
		*ptr = cpu->ram + pos;
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}

char readChar(char** ptr) {
	assert(ptr != NULL);
	assert(*ptr != NULL);

	char value = **ptr;
	*ptr += sizeof(char);

	return value;
}

int readInt(char** ptr) {
	assert(ptr != NULL);
	assert(*ptr != NULL);

	int value = *(int*)*ptr;
	*ptr += sizeof(int);

	return value;
}

double readDouble(char** ptr) {
	assert(ptr != NULL);
	assert(*ptr != NULL);

	double value = *(double*)*ptr;
	*ptr += sizeof(double);

	return value;
}

int startProgram(CPU* cpu) {
	ASSERT_OK(CPU, cpu, __func__);

	char* ptr = cpu->ram;
	char temp = ' ';
	while (temp != END && ptr < cpu->ram + cpu->ram_size) {
		temp = readChar(&ptr);

		switch (temp) {
			case PUSH: {
				char type = readChar(&ptr);
				switch (type) {
					case 0: {
						double value = readDouble(&ptr);
						push(cpu, 0, value);
						}
						break;
					case 1:
						push(cpu, 1, 0);
						break;
					case 2:
						push(cpu, 2, 0);
						break;
					case 3:
						push(cpu, 3, 0);
						break;
					case 4:
						push(cpu, 4, 0);
						break;
					case 5:
						push(cpu, 5, 0);
						break;
					case 6:
						push(cpu, 6, 0);
						break;
				}
			}
			break;
			case POP: {
				char type = readChar(&ptr);
				switch (type) {
					case 0:
						pop(cpu, 0);
						break;
					case 1:
						pop(cpu, 1);
						break;
					case 2:
						pop(cpu, 2);
						break;
					case 3:
						pop(cpu, 3);
						break;
					case 4:
						pop(cpu, 4);
						break;
					case 5:
						pop(cpu, 5);
						break;
					case 6:
						pop(cpu, 6);
						break;
				}
			}
			break;
			case IN:
				in(cpu);
				break;
			case OUT:
				out(cpu);
				break;
			case END:
				break;
				break;
			case ADD:
				add(cpu);
				break;
			case DIV:
				div(cpu);
				break;
			case MUL:
				mul(cpu);
				break;
			case SUB:
				sub(cpu);
				break;
			case SIN:
				cpu_sin(cpu);
				break;
			case COS:
				cpu_cos(cpu);
				break;
			case SQRT:
				cpu_sqrt(cpu);
				break;
			case POW:
				cpu_pow(cpu);
				break;
			case CALL: 
				call(cpu, &ptr, readInt(&ptr));
				break;
			case RET:
				ret(cpu, &ptr);
				break;
			case JMP:
				jmp(cpu, &ptr, readInt(&ptr));
				break;
			case JE:
				je(cpu, &ptr, readInt(&ptr));
				break;
			case JNE:
				jne(cpu, &ptr, readInt(&ptr));
				break;
			case JA:
				ja(cpu, &ptr, readInt(&ptr));
				break;
			case JAE:
				jae(cpu, &ptr, readInt(&ptr));
				break;
			case JB:
				jb(cpu, &ptr, readInt(&ptr));
				break;
			case JBE:
				jbe(cpu, &ptr, readInt(&ptr));
				break;
			case MARK:
				break;
			default:
				break;
		}
	}

	ASSERT_OK(CPU, cpu, __func__);
	return OK;
}