#pragma once

const double SIZE_MULTIPLIER = 1.5;
const double SIZE_BAD_MULTIPLIER = 2;
const int SIZE_NORMAL_DIFFERENCE = 20;
const int STACK_INIT_SIZE = 10;
const int BAD_COUNT_VALUE = -1;
const double DEFAULT_HASH_SUM = 0xBADCAFE;

enum STACK_ERRORS {
	ZERO_SIZE = -2,
	NULL_STACK_POINTER,
	NULL_ARRAY_POINTER = 1,
	INCORRECT_SIZE,
	INCORRECT_TOP
};

const int OK = 0;
const int VALIDATION_FAILS = -1;

struct stack {
	double protection1;
	double* arr;
	double ctr_sum;
	int size;
	int top;
	double protection2;
};

int checkStack(stack* st);
int assertStack(stack* st, const char* function);
int assertParameters(stack* st, const char* function, bool status, const char* error);
int errorStackPrint(FILE* errorFile, const int code);
int dumpStack(stack* st, FILE* dumpFile);
int pushStack(stack* st, const double value);
double popStack(stack* st);
double peekStack(stack* st);
stack* initStack(const int size);
int resizeStack(stack* st, const int newSize);
int deleteStack(stack** st);
double createSum(stack *st);
int rehashSum(stack *st);
int checkSum(stack *st);