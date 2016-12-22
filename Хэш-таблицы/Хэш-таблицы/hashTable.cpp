#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashTable.h"

#define ASSERT_OK(struct, wat, function) {		  \
    if (assert##struct(wat, function) != OK) {    \
        assert(false);							  \
	}											  \
}

#define ASSERT_PARAMETERS(lst, function, status, error) {						\
    if (assertHashTableParameters(lst, function, status, error) != OK) {		\
        assert(false);															\
	}																			\
}

int assertHashTable(hashTable* hTable, const char* function) {
	bool NULLInsidePointer = false;

	for (int i = 0; i < hTable->size; i++) {
		if (!hTable->table[i]) {
			NULLInsidePointer = true;
			break;
		}
	}

	if (hTable && hTable->table && !NULLInsidePointer && hTable->size > 0 && hTable->hashFunction) {
		return OK;
	}

	FILE* assertFile = fopen("assert.txt", "a");

	fprintf(assertFile, "=== Error in function: %s ===\n", function);
	if (!hTable) {
		errorListPrint(assertFile, NULL_POINTER);
	}
	if (!hTable->table) {
		errorListPrint(assertFile, NULL_TABLE_POINTER);
	}
	if (NULLInsidePointer) {
		errorListPrint(assertFile, NULL_INSIDE_POINTER);
	}
	if (hTable->size <= 0) {
		errorListPrint(assertFile, INCORRECT_SIZE);
	}
	if (!hTable->hashFunction) {
		errorListPrint(assertFile, NULL_FUNCTION_POINTER);
	}
	fprintf(assertFile, "\n");

	fclose(assertFile);

	FILE* dumpFile = fopen("dump.txt", "a");
	dumpHashTable(hTable, dumpFile);
	fclose(dumpFile);

	deleteHashTable(hTable);

	return VALIDATION_FAILS;
}

int assertHashTableParameters(hashTable* hTable, const char* function, bool status, const char* error) {
	if (status) {
		return OK;
	}

	FILE* assertFile = fopen("assert.txt", "a");

	fprintf(assertFile, "=== Error in function: %s ===\n", function);
	fprintf(assertFile, "%s\n\n", error);

	fclose(assertFile);

	FILE* dumpFile = fopen("dump.txt", "a");
	dumpHashTable(hTable, dumpFile);
	fclose(dumpFile);

	deleteHashTable(hTable);

	return VALIDATION_FAILS;
}

int errorHashTablePrint(FILE* errorFile, const int code) {
	switch (code) {
	case NULL_POINTER:
		fprintf(errorFile, "hashTable = NULL\n");
		break;
	case NULL_TABLE_POINTER:
		fprintf(errorFile, "hashTable->table = NULL\n");
		break;
	case NULL_INSIDE_POINTER:
		fprintf(errorFile, "hashTable->table[i] = NULL\n");
		break;
	case INCORRECT_SIZE:
		fprintf(errorFile, "hashTable->size <= 0\n");
		break;
	case NULL_FUNCTION_POINTER:
		fprintf(errorFile, "hashTable->hashFunction = NULL\n");
		break;
	default:
		break;
	}

	return code;
}

int dumpHashTable(hashTable* hTable, FILE* dumpFile) {

	fprintf(dumpFile, "\n========== DUMP_HASH_TABLE ==========\n");
	fprintf(dumpFile, "hashTable = %p\n", hTable);
	fprintf(dumpFile, "hashTable->table = %p\n", hTable->table);
	fprintf(dumpFile, "hashTable->size = %d\n", hTable->size);
	fprintf(dumpFile, "hashTable->hashFunction = %p\n", hTable->hashFunction);

	for (int i = 0; i < hTable->size; i++) {
		fprintf(dumpFile, "\n----------------- %d -----------------", i);
		dumpList(hTable->table[i], dumpFile);
		fprintf(dumpFile, "-------------------------------------\n");
	}
	fprintf(dumpFile, "\n=====================================\n");

	return OK;
}

hashTable* initHashTable(const int size, unsigned int (*hashFunction) (const char*)) {
	hashTable* hTable = (hashTable*)calloc(1, sizeof(hashTable));
	ASSERT_PARAMETERS(hTable, __func__, size > 0, "size <= 0");
	ASSERT_PARAMETERS(hTable, __func__, hashFunction != NULL, "hashFunction = NULL");

	list** table = (list**)calloc(size, sizeof(list*));
	for (int i = 0; i < size; i++) {
		table[i] = initList();
	}

	hTable->table = table;
	hTable->size = size;
	hTable->hashFunction = hashFunction;

	ASSERT_OK(HashTable, hTable, __func__);
	return hTable;
}

int deleteHashTable(hashTable* hTable) {
	for (int i = 0; i < hTable->size; i++) {
		deleteList(hTable->table[i]);
	}

	delete hTable->table;
	hTable->table = NULL;
	hTable->size = 0;
	hTable->hashFunction = NULL;

	delete hTable;
	hTable = NULL;

	return OK;
}

int getHashValue(hashTable* hTable, char* key) {
	ASSERT_OK(HashTable, hTable, __func__);
	ASSERT_PARAMETERS(hTable, __func__, key != NULL, "key = NULL");

	return hTable->hashFunction(key) % hTable->size;
}

node* findHashTable(hashTable* hTable, char* key) {
	ASSERT_OK(HashTable, hTable, __func__);
	ASSERT_PARAMETERS(hTable, __func__, key != NULL, "key = NULL");

	int hashValue = getHashValue(hTable, key);
	node* nodeFound = findList(hTable->table[hashValue], key);

	ASSERT_OK(HashTable, hTable, __func__);
	return nodeFound;
}

int insertHashTable(hashTable* hTable, char* key) {
	ASSERT_OK(HashTable, hTable, __func__);
	ASSERT_PARAMETERS(hTable, __func__, key != NULL, "key = NULL");

	node* nodeFound = findHashTable(hTable, key);
	if (nodeFound != NULL) {
		return VALIDATION_FAILS;
	}

	int hashValue = getHashValue(hTable, key);
	pushBack(hTable->table[hashValue], key);

	ASSERT_OK(HashTable, hTable, __func__);
	return OK;
}

char* popHashTable(hashTable* hTable, char* key) {
	ASSERT_OK(HashTable, hTable, __func__);
	ASSERT_PARAMETERS(hTable, __func__, key != NULL, "key = NULL");

	if (!findHashTable(hTable, key)) {
		return "";
	}

	int hashValue = getHashValue(hTable, key);
	char* str = deleteNode(hTable->table[hashValue], findHashTable(hTable, key));

	ASSERT_OK(HashTable, hTable, __func__);
	return str;
}
