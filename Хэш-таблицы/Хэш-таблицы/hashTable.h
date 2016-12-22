#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "list.h"

const int NULL_TABLE_POINTER = 1;
const int NULL_INSIDE_POINTER = 2;
const int NULL_FUNCTION_POINTER = 4;

struct hashTable {
	list** table;
	int size;
	unsigned int (*hashFunction) (const char*);
};

int assertHashTable(hashTable* hTable, const char* function);
int assertHashTableParameters(hashTable* hTable, const char* function, bool status, const char* error);
int errorHashTablePrint(FILE* errorFile, const int code);
int dumpHashTable(hashTable* hTable, FILE* dumpFile);
hashTable* initHashTable(const int size, unsigned int(*hashFunction)(const char*));
int deleteHashTable(hashTable* hTable);
int getHashValue(hashTable* hTable, char* key);
node* findHashTable(hashTable* hTable, char* key);
int insertHashTable(hashTable* hTable, char* key);
char* popHashTable(hashTable* hTable, char* key);