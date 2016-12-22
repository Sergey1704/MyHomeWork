#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "stack.h"

#define ASSERT_OK(struct, wat, function) {		  \
    if (assert##struct(wat, function) != OK) {    \
        assert(false);							  \
	}											  \
}

#define ASSERT_PARAMETERS(st, function, status, error) {		  \
    if (assertParameters(st, function, status, error) != OK) {    \
        assert(false);											  \
	}															  \
}

#define STACK_OK(st) {								\
	if (checkStack(st) != OK) {						\
		FILE* dumpFile = fopen("dump.txt", "a");	\
		dumpStack(st, dumpFile);					\
		fclose(dumpFile);							\
		abort();									\
	}												\
}                   

int checkStack(stack* st) {
	
	if ((st->protection1 != DEFAULT_HASH_SUM) || (st->protection2 != DEFAULT_HASH_SUM) ||
		(*st->arr != DEFAULT_HASH_SUM) || (*(st->arr + st->size - 1) != DEFAULT_HASH_SUM) ||
		(checkSum(st) != OK)) {
		
		return VALIDATION_FAILS;
	}

	return OK;
}

int assertStack(stack* st, const char* function) {
	STACK_OK(st);

	if (st && st->arr && (st->size >= 1 + 0 + 1) && (1 + 0 <= st->top && st->top <= st->size - 1)) {
		return OK;
	}

	FILE* assertFile = fopen("assert.txt", "a");

	fprintf(assertFile, "=== Error in function: %s ===\n", function);

	if (!st) {
		errorStackPrint(assertFile, NULL_STACK_POINTER);
	}
	if (!st->arr) {
		errorStackPrint(assertFile, NULL_ARRAY_POINTER);
	}
	if (!(st->size >= 1 + 0 + 1)) {
		errorStackPrint(assertFile, INCORRECT_SIZE);
	}
	if (!(1 + 0 <= st->top && st->top <= st->size - 1)) {
		errorStackPrint(assertFile, INCORRECT_TOP);
	}

	fclose(assertFile);

	FILE* dumpFile = fopen("dump.txt", "a");
	dumpStack(st, dumpFile);
	fclose(dumpFile);

	deleteStack(&st);
	
	return VALIDATION_FAILS;
}

int assertParameters(stack* st, const char* function, bool status, const char* error) {
	if (status) {
		return OK;
	}

	FILE* assertFile = fopen("assert.txt", "a");

	fprintf(assertFile, "=== Error in function: %s ===\n", function);
	fprintf(assertFile, "%s\n", error);

	fclose(assertFile);

	FILE* dumpFile = fopen("dump.txt", "a");
	dumpStack(st, dumpFile);
	fclose(dumpFile);

	deleteStack(&st);

	return VALIDATION_FAILS;
}

int errorStackPrint(FILE* errorFile, const int code) {
	switch (code) {
		case NULL_STACK_POINTER:
			fprintf(errorFile, "st = NULL\n");
			break;
		case NULL_ARRAY_POINTER:
			fprintf(errorFile, "st->arr = NULL\n");
			break;
		case INCORRECT_SIZE:
			fprintf(errorFile, "st->size < 0\n");
			break;
		case INCORRECT_TOP:
			fprintf(errorFile, "st->top < 0\n");
			break;
		default:
			break;
	}

	return code;
}

int dumpStack(stack* st, FILE* dumpFile) {

	fprintf(dumpFile, "========== DUMP_STACK ==========\n");
	fprintf(dumpFile, "st = %p\n", st);
	fprintf(dumpFile, "st->arr = %p\n", st->arr);
	fprintf(dumpFile, "st->size = %d\n", st->size);
	fprintf(dumpFile, "st->top = %d\n", st->top);
	fprintf(dumpFile, "st->protection1 = %d\n", st->protection1);
	fprintf(dumpFile, "st->protection2 = %d\n", st->protection2);
	fprintf(dumpFile, "st->ctr_sum = %lf\n", st->ctr_sum);

	for (int i = 0; i < st->size; i++) {
		fprintf(dumpFile, "\nst->arr[%d] = %lf", i, st->arr[i]);
		if (i == 0 || i == st->size - 1) {
			fprintf(dumpFile, " (protection)");
		}
	}

	fprintf(dumpFile, "\n================================\n");

	return OK;
}

int pushStack(stack* st, const double value) {
	ASSERT_OK(Stack, st, __func__);
	
	if (st->top >= st->size - 1) {
		resizeStack(st, int((st->size - 2)*SIZE_MULTIPLIER) + 1);
	}

	st->arr[st->top] = value;
	st->top++;

	rehashSum(st);
	ASSERT_OK(Stack, st, __func__);
	return st->top;
}

double popStack(stack* st) {
	ASSERT_OK(Stack, st, __func__);
	ASSERT_PARAMETERS(st, __func__, st->top > 1, "st->top = 1");

	double value = st->arr[st->top - 1];
	st->arr[st->top - 1] = 0;
	st->top--;

	rehashSum(st);
	ASSERT_OK(Stack, st, __func__);

	if (st->size > int(st->top*SIZE_BAD_MULTIPLIER) + 1 && (st->size - 1) - st->top > SIZE_NORMAL_DIFFERENCE) {
		resizeStack(st, int((st->top - 1) * SIZE_MULTIPLIER) + 1);
	}

	return value;
}

double peekStack(stack* st) {
	ASSERT_OK(Stack, st, __func__);
	ASSERT_PARAMETERS(st, __func__, st->top > 1, "st->top = 1");

	return st->arr[st->top - 1];
}

stack* initStack(const int size) {
	stack* st = (stack*)calloc(1, sizeof(stack));

	st->protection1 = DEFAULT_HASH_SUM;
	st->protection2 = DEFAULT_HASH_SUM;

	st->size = 1 + size + 1;

	double* pointer = (double*)calloc(st->size, sizeof(double));
	ASSERT_PARAMETERS(st, __func__, pointer != NULL, "Not enough memory for st->arr");
	st->arr = pointer;

	*st->arr = DEFAULT_HASH_SUM;
	*(st->arr + st->size - 1) = DEFAULT_HASH_SUM;
	
	st->top = 1;

	rehashSum(st);
	ASSERT_OK(Stack, st, __func__);

	return st;
}

int resizeStack(stack* st, const int newSize) {
	ASSERT_OK(Stack, st, __func__);
	ASSERT_PARAMETERS(st, __func__, 1 + newSize + 1 >= st->top + 1, "1 + newSize + 1 < st->top + 1");

	st->size = 1 + newSize + 1;
	double* pointer = (double*)realloc(st->arr, st->size * sizeof(double));
	ASSERT_PARAMETERS(st, __func__, pointer != NULL, "Not enough memory for st->arr");
	st->arr = pointer;
	
	st->arr[st->size - 1] = DEFAULT_HASH_SUM;

	rehashSum(st);
	ASSERT_OK(Stack, st, __func__);

	return st->size;
}

int deleteStack(stack** st) {

	free((*st)->arr);
	(*st)->arr = NULL;
	(*st)->size = BAD_COUNT_VALUE;
	(*st)->top = BAD_COUNT_VALUE;
	(*st)->protection1 = BAD_COUNT_VALUE;
	(*st)->protection2 = BAD_COUNT_VALUE;
	(*st)->ctr_sum = 0;
	free(*st);
	*st = NULL;

	return OK;
}

double createSum(stack *st) {
	double sum = 0;

	for (int i = 1; i < st->size - 1; i++) {
		sum += st->arr[i];
	}

	sum += st->size;
	sum += st->top;

	return sum;
}

int rehashSum(stack *st) {
	st->ctr_sum = createSum(st);

	return OK;
}

int checkSum(stack *st) {
	if (createSum(st) == st->ctr_sum) {
		return OK;
	}
	else {
		return VALIDATION_FAILS;
	}
}