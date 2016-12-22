#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "CPU.h"
#include "ALU.h"

int pushChar(char** ptr, const char value) {
	assert(ptr != NULL);
	assert(*ptr != NULL);

	**ptr = (char)value;
	*ptr += sizeof(char);

	return OK;
}

int pushInt(char** ptr, const int value) {
	assert(ptr != NULL);
	assert(*ptr != NULL);

	*(int*)*ptr = (int)value;
	*ptr += sizeof(int);

	return OK;
}

int pushDouble(char** ptr, const double value) {
	assert(ptr != NULL);
	assert(*ptr != NULL);

	*(double*)*ptr = (double)value;
	*ptr += sizeof(double);

	return OK;
}

bool isCorrectLabel(char* str) {
	assert(str != NULL);

	int size = strlen(str);
	if (size == 0) {
		return false;
	}
	int i = size - 1;
	while (isspace(str[i]) != 0 || str[i] == '\0') {
		i--;
	}
	if (i < 0 || str[i] != ':') {
		return false;
	}
	/*if (!('a' <= str[0] && str[0] <= 'z' || 'A' <= str[0] && str[0] <= 'Z')) {
		return false;
	}*/
	for (int i = 0; i < size - 1; i++) {
		if (!('0' <= str[i] && str[i] <= '9' || 'a' <= str[i] && str[i] <= 'z' || 'A' <= str[i] && str[i] <= 'Z' || str[i] == '_')) {
			return false;
		}
	}
	return true;
}

label* initLabel(char* name, int pos) {
	assert(name != NULL);
	assert(pos >= 0);

	label* newLabel = (label*)calloc(1, sizeof(label));

	strcpy(newLabel->name, name);
	newLabel->pos = pos;

	return newLabel;
}

int assembler(const char* inputFileName, char* commands, const char* codeFileName) {
	assert(commands != NULL);

	char* ptr = commands;

	FILE* inputFile = fopen(inputFileName, "r");
	FILE* codeFile = fopen(codeFileName, "w");
	
	label* callLabels[LABELS_NUMBER];
	int callLabelsSize = 0;
	label* labels[LABELS_NUMBER];
	int labelsSize = 0;

	while (true) {
		char buf[COMMAND_SIZE];
		int count = fscanf(inputFile, "%s", buf);
		if (count <= 0) {
			break;
		}
		label1:
		if (!_stricmp(buf, "push")) {
			fprintf(codeFile, "%d ", PUSH);
			pushChar(&ptr, PUSH);

			char param[COMMAND_SIZE];
			fscanf(inputFile, "%s", param);

			if (!_stricmp(param, "ax")) {
				fprintf(codeFile, "%d\n", 1);
				pushChar(&ptr, 1);
			}
			else if (!_stricmp(param, "bx")) {
				fprintf(codeFile, "%d\n", 2);
				pushChar(&ptr, 2);
			}
			else if (!_stricmp(param, "cx")) {
				fprintf(codeFile, "%d\n", 3);
				pushChar(&ptr, 3);
			}
			else if (!_stricmp(param, "dx")) {
				fprintf(codeFile, "%d\n", 4);
				pushChar(&ptr, 4);
			}
			else if (!_stricmp(param, "i")) {
				fprintf(codeFile, "%d\n", 5);
				pushChar(&ptr, 5);
			}
			else if (!_stricmp(param, "cache")) {
				fprintf(codeFile, "%d\n", 6);
				pushChar(&ptr, 6);
			}
			else {
				fprintf(codeFile, "%d %s\n", 0, param);
				pushChar(&ptr, 0);
				pushDouble(&ptr, atof(param));
			}
		}
		else if (!_stricmp(buf, "pop")) {
			fprintf(codeFile, "%d ", POP);
			pushChar(&ptr, POP);

			char param[COMMAND_SIZE];
			fscanf(inputFile, "%s", param);

			if (!_stricmp(param, "ax")) {
				fprintf(codeFile, "%d\n", 1);
				pushChar(&ptr, 1);
			}
			else if (!_stricmp(param, "bx")) {
				fprintf(codeFile, "%d\n", 2);
				pushChar(&ptr, 2);
			}
			else if (!_stricmp(param, "cx")) {
				fprintf(codeFile, "%d\n", 3);
				pushChar(&ptr, 3);
			}
			else if (!_stricmp(param, "dx")) {
				fprintf(codeFile, "%d\n", 4);
				pushChar(&ptr, 4);
			}
			else if (!_stricmp(param, "i")) {
				fprintf(codeFile, "%d\n", 5);
				pushChar(&ptr, 5);
			}
			else if (!_stricmp(param, "cache")) {
				fprintf(codeFile, "%d\n", 6);
				pushChar(&ptr, 6);
			}
			else {
				fprintf(codeFile, "%d\n", 0);
				pushChar(&ptr, 0);
				strcpy(buf, param);
				goto label1;
			}
		}
		else if (!_stricmp(buf, "in")) {
			fprintf(codeFile, "%d\n", IN);
			pushChar(&ptr, IN);
		}
		else if (!_stricmp(buf, "out")) {
			fprintf(codeFile, "%d\n", OUT);
			pushChar(&ptr, OUT);
		}
		else if (!_stricmp(buf, "end")) {
			fprintf(codeFile, "%d\n", END);
			pushChar(&ptr, END);
		}
		else if (!_stricmp(buf, "add")) {
			fprintf(codeFile, "%d\n", ADD);
			pushChar(&ptr, ADD);
		}
		else if (!_stricmp(buf, "div")) {
			fprintf(codeFile, "%d\n", DIV);
			pushChar(&ptr, DIV);
		}
		else if (!_stricmp(buf, "mul")) {
			fprintf(codeFile, "%d\n", MUL);
			pushChar(&ptr, MUL);
		}
		else if (!_stricmp(buf, "sub")) {
			fprintf(codeFile, "%d\n", SUB);
			pushChar(&ptr, SUB);
		}
		else if (!_stricmp(buf, "call")) {
			char labelName[LABEL_NAME_SIZE];
			fscanf(inputFile, "%s", labelName);

			fprintf(codeFile, "%d %s\n", CALL, labelName);
			pushChar(&ptr, CALL);
			callLabels[callLabelsSize] = initLabel(labelName, ptr - commands);
			callLabelsSize++;
			pushInt(&ptr, 0);
		}
		else if (!_stricmp(buf, "ret")) {
			fprintf(codeFile, "%d\n", RET);
			pushChar(&ptr, RET);
		}
		else if (!_stricmp(buf, "je")) {
			char labelName[LABEL_NAME_SIZE];
			fscanf(inputFile, "%s", labelName);

			fprintf(codeFile, "%d %s\n", JE, labelName);
			pushChar(&ptr, JE);
			callLabels[callLabelsSize] = initLabel(labelName, ptr - commands);
			callLabelsSize++;
			pushInt(&ptr, 0);
		}
		else if (!_stricmp(buf, "jne")) {
			char labelName[LABEL_NAME_SIZE];
			fscanf(inputFile, "%s", labelName);

			fprintf(codeFile, "%d %s\n", JNE, labelName);
			pushChar(&ptr, JNE);
			callLabels[callLabelsSize] = initLabel(labelName, ptr - commands);
			callLabelsSize++;
			pushInt(&ptr, 0);
		}
		else if (!_stricmp(buf, "ja")) {
			char labelName[LABEL_NAME_SIZE];
			fscanf(inputFile, "%s", labelName);

			fprintf(codeFile, "%d %s\n", JA, labelName);
			pushChar(&ptr, JA);
			callLabels[callLabelsSize] = initLabel(labelName, ptr - commands);
			callLabelsSize++;
			pushInt(&ptr, 0);
		}
		else if (!_stricmp(buf, "jae")) {
			char labelName[LABEL_NAME_SIZE];
			fscanf(inputFile, "%s", labelName);

			fprintf(codeFile, "%d %s\n", JAE, labelName);
			pushChar(&ptr, JAE);
			callLabels[callLabelsSize] = initLabel(labelName, ptr - commands);
			callLabelsSize++;
			pushInt(&ptr, 0);
		}
		else if (!_stricmp(buf, "jb")) {
			char labelName[LABEL_NAME_SIZE];
			fscanf(inputFile, "%s", labelName);

			fprintf(codeFile, "%d %s\n", JB, labelName);
			pushChar(&ptr, JB);
			callLabels[callLabelsSize] = initLabel(labelName, ptr - commands);
			callLabelsSize++;
			pushInt(&ptr, 0);
		}
		else if (!_stricmp(buf, "jbe")) {
			char labelName[LABEL_NAME_SIZE];
			fscanf(inputFile, "%s", labelName);

			fprintf(codeFile, "%d %s\n", JBE, labelName);
			pushChar(&ptr, JBE);
			callLabels[callLabelsSize] = initLabel(labelName, ptr - commands);
			callLabelsSize++;
			pushInt(&ptr, 0);
		}
		else if (!_stricmp(buf, "jmp")) {
			char labelName[LABEL_NAME_SIZE];
			fscanf(inputFile, "%s", labelName);

			fprintf(codeFile, "%d %s\n", JMP, labelName);
			pushChar(&ptr, JMP);
			callLabels[callLabelsSize] = initLabel(labelName, ptr - commands);
			callLabelsSize++;
			pushInt(&ptr, 0);
		}
		else if (!_stricmp(buf, "sin")) {
			fprintf(codeFile, "%d\n", SIN);
			pushChar(&ptr, SIN);
		}
		else if (!_stricmp(buf, "cos")) {
			fprintf(codeFile, "%d\n", COS);
			pushChar(&ptr, COS);
		}
		else if (!_stricmp(buf, "sqrt")) {
			fprintf(codeFile, "%d\n", SQRT);
			pushChar(&ptr, SQRT);
		}
		else if (!_stricmp(buf, "pow")) {
			fprintf(codeFile, "%d\n", POW);
			pushChar(&ptr, POW);
		}
		else {
			if (isCorrectLabel(buf)) {
				fprintf(codeFile, "%d %s\n", MARK, buf);
				pushChar(&ptr, MARK);
				buf[strlen(buf) - 1] = '\0';
				labels[labelsSize] = initLabel(buf, ptr - commands);
				labelsSize++;
			}
		}
	}
	fclose(inputFile);
	fclose(codeFile);

	int ramSize = ptr - commands;
	for (int i = 0; i < callLabelsSize; i++) {
		ptr = commands + callLabels[i]->pos;
		
		for (int j = 0; j < labelsSize; j++) {
			if (!_stricmp(callLabels[i]->name, labels[j]->name)) {
				*(int*)ptr = labels[j]->pos;
				break;
			}
		}
	}
	
	return ramSize;
}

int disassembler(const char* codeFileName, const char* outputFileName) {
	FILE* codeFile = fopen(codeFileName, "r");
	FILE* outputFile = fopen(outputFileName, "w");

	while (true) {
		int buf = 0;
		int count = fscanf(codeFile, "%d", &buf);
		if (count <= 0) {
			break;
		}
		switch (buf) {
			case PUSH: {
				int param = 0;
				double value = 0;
				fscanf(codeFile, "%d", &param);
				switch (param) {
					case 0: {
						fscanf(codeFile, "%lg", &value);

						fprintf(outputFile, "push %lf\n", value);
					}
					break;
					case 1:
						fprintf(outputFile, "push ax\n");
						break;
					case 2:
						fprintf(outputFile, "push bx\n");
						break;
					case 3:
						fprintf(outputFile, "push cx\n");
						break;
					case 4:
						fprintf(outputFile, "push dx\n");
						break;
					case 5:
						fprintf(outputFile, "push i\n");
						break;
					case 6:
						fprintf(outputFile, "push cache\n");
						break;
				}	
			}
			break;
			case POP: {
				int param = 0;
				fscanf(codeFile, "%d", &param);
				switch (param) {
					case 0:
						fprintf(outputFile, "pop\n");
						break;
					case 1:
						fprintf(outputFile, "pop ax\n");
						break;
					case 2:
						fprintf(outputFile, "pop bx\n");
						break;
					case 3:
						fprintf(outputFile, "pop cx\n");
						break;
					case 4:
						fprintf(outputFile, "pop dx\n");
						break;
					case 5:
						fprintf(outputFile, "pop i\n");
						break;
					case 6:
						fprintf(outputFile, "pop cache\n");
						break;
				}
			}
			break;
			case IN:
				fprintf(outputFile, "in\n");
				break;
			case OUT:
				fprintf(outputFile, "out\n");
				break;
			case END:
				fprintf(outputFile, "end\n");
				break;
			case ADD:
				fprintf(outputFile, "add\n");
				break;
			case DIV:
				fprintf(outputFile, "div\n");
				break;
			case MUL:
				fprintf(outputFile, "mul\n");
				break;
			case SUB:
				fprintf(outputFile, "sub\n");
				break;
			case SIN:
				fprintf(outputFile, "sin\n");
				break;
			case COS:
				fprintf(outputFile, "cos\n");
				break;
			case SQRT:
				fprintf(outputFile, "sqrt\n");
				break;
			case POW:
				fprintf(outputFile, "pow\n");
				break;
			case CALL: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "call %s\n", labelName);
			}
			break;
			case RET:
				fprintf(outputFile, "ret\n");
				break;
			case JMP: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "jmp %s\n", labelName);
			}
			break;
			case JE: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "je %s\n", labelName);
			}
			break;
			case JNE: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "jne %s\n", labelName);
			}
			break;
			case JA: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "ja %s\n", labelName);
			}
			break;
			case JAE: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "jae %s\n", labelName);
			}
			break;
			case JB: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "jb %s\n", labelName);
			}
			break;
			case JBE: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "jbe %s\n", labelName);
			}
			break;
			case MARK: {
				char labelName[LABEL_NAME_SIZE];
				fscanf(codeFile, "%s", labelName);

				fprintf(outputFile, "%s\n", labelName);
			}
			break;
			default:
				break;
		}
	}

	fclose(codeFile);
	fclose(outputFile);

	return OK;
}