#pragma once

const int COMMAND_SIZE = 1000 + 1;
const int LABELS_NUMBER = 100 + 1;
const int LABEL_NAME_SIZE = 100 + 1;
const int MAX_RAM_SIZE = 1000;
const int CACHE_SIZE = 1000;

enum COMMANDS {
	PUSH = 1,
	POP,
	IN,
	OUT,
	END,
	ADD,
	DIV,
	MUL,
	SUB,
	SIN,
	COS,
	SQRT,
	POW,
	CALL,
	RET,
	JE,
	JNE,
	JA,
	JAE,
	JB,
	JBE,
	JMP,
	MARK
};

int assembler(const char* inputFileName, char* commands, const char* outputFileName);
int disassembler(const char* codeFileName, const char* outputFileName);

struct label {
	char name[LABEL_NAME_SIZE];
	int pos;
};

int pushChar(char** ptr, const char value);
int pushInt(char** ptr, const int value);
int pushDouble(char** ptr, const double value);
bool isCorrectLabel(char* str);
label* initLabel(char* name, int pos);
