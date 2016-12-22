#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashTable.h"

#define HashFunction HashLy
#define HashTableSize 100

unsigned int H1(const char* str);
unsigned int H2(const char* str);
unsigned int H3(const char* str);
unsigned int H4(const char* str);
unsigned int H5(const char* str);
unsigned int H6(const char* str);
unsigned int HashLy(const char * str);

char* textReadFromFile(const char* fileName, int* textSize);
bool isCorrectChar(char c);

int main() {
	/*FILE* dumpFile = fopen("dump.txt", "a");

	list* lst = initList();
	pushBack(lst, "12");
	pushBack(lst, "13");
	pushFront(lst, "11");
	popBack(lst);
	popFront(lst);
	node* a = nodeByNumber(lst, 1);
	insertBefore(lst, a, "1");
	insertAfter(lst, a, "100");
	popBack(lst);
	popBack(lst);
	cleanList(lst);
	dumpList(lst, dumpFile); 
	pushBack(lst, "-1");
	printf("%p", findList(lst, "-1"));
	popBack(lst);
	dumpList(lst, dumpFile);
	deleteList(lst);

	fclose(dumpFile);
	*/
	
	hashTable* HashTable = initHashTable(HashTableSize, HashFunction);
	
	int textSize = 0;
	char* text = textReadFromFile("input_rus.txt", &textSize);

	for (int i = 0; i < textSize; i++) {
		if (!isCorrectChar(text[i])) {
			text[i] = '\0';
		}
	}
	if (isCorrectChar(text[0])) {
		insertHashTable(HashTable, &text[0]);
	}
	for (int i = 2; i < textSize; i++) {
		if (isCorrectChar(text[i]) && !isCorrectChar(text[i - 1])) {
			insertHashTable(HashTable, &text[i]);
		}
	}

	FILE* fWrite = fopen("output.txt", "w");
	for (int i = 0; i < HashTable->size; i++) {
		fprintf(fWrite, "%d %d\n", i, HashTable->table[i]->size);
	}
	fclose(fWrite);
	
	return 0;
}

bool isCorrectChar(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('à' <= c && c <= 'ÿ') || ('À' <= c && c <= 'ß') || ('0' <= c && c <= '9');
}

char* textReadFromFile(const char* fileName, int* textSize) {
	assert(fileName);
	*textSize = 0;

	FILE* fRead = fopen(fileName, "r");
	assert(fRead && "Open file ERROR!");

	fseek(fRead, 0, SEEK_END);
	*textSize = ftell(fRead) + 1;
	fseek(fRead, 0, SEEK_SET);

	char* text = (char*)calloc(*textSize, sizeof(char));

	int result = fread(text, sizeof(char), *textSize, fRead);
	
	assert(result && "Read file ERROR!");

	fclose(fRead);

	return text;
}

unsigned int H1(const char* str) {

	return 1;
}

unsigned int H2(const char* str) {

	return *str;
}

unsigned int H3(const char* str) {
	int count = 0;
	for (; *str; str++) {
		count++;
	}

	return count;
}

unsigned int H4(const char* str) {
	int sum = 0;
	for (; *str; str++) {
		sum += *str;
	}

	return sum;
}

unsigned int H5(const char* str) {
	int sum = 0;
	int count = 0;
	for (; *str; str++) {
		sum += *str;
		count++;
	}

	return sum / count;
}

unsigned int H6(const char* str) {
	unsigned int hash = 0;

	for (; *str; str++) {
		hash = _rotl(hash, 1) ^ *str;
	}

	return hash;
}

unsigned int HashLy(const char * str) {
	unsigned int hash = 0;

	for (; *str; str++) {
		hash = (hash * 1664525) + (unsigned char)(*str) + 1013904223;
	}

	return hash;
}