#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

#define ASSERT_OK(struct, wat, function) {		  \
    if (assert##struct(wat, function) != OK) {    \
        assert(false);							  \
	}											  \
}

#define ASSERT_PARAMETERS(lst, function, status, error) {				\
    if (assertListParameters(lst, function, status, error) != OK) {		\
        assert(false);													\
	}																	\
}


int assertList(list* lst, const char* function) {
	bool strNULLpointer = false;
	bool incorrectInsidePointer = false;
	
	node* tempNode = lst->head;
	int size = 0;
	while (size <= lst->size + 2 && tempNode != NULL) {
		if ((tempNode->prev != NULL && tempNode == lst->head) || (tempNode->next != NULL && tempNode == lst->tail) ||
			(tempNode->prev == NULL && tempNode != lst->head) || (tempNode->next == NULL && tempNode != lst->tail)) {

			incorrectInsidePointer = true;
		}
		if (tempNode->str == NULL) {
			strNULLpointer = true;
		}
		
		tempNode = tempNode->next;
		size++;
	}

	if (lst && size == lst->size && !strNULLpointer && !incorrectInsidePointer) {
		return OK;
	}

	FILE* assertFile = fopen("assert.txt", "a");

	fprintf(assertFile, "=== Error in function: %s ===\n", function);
	if (!lst) {
		errorListPrint(assertFile, NULL_POINTER);
	}
	if (size != lst->size) {
		errorListPrint(assertFile, INCORRECT_SIZE);
	}
	if (strNULLpointer) {
		errorListPrint(assertFile, NULL_VALUE_POINTER);
	}
	if (incorrectInsidePointer) {
		errorListPrint(assertFile, INCORRECT_INSIDE_POINTER);
	}
	fprintf(assertFile, "\n");

	fclose(assertFile);

	FILE* dumpFile = fopen("dump.txt", "a");
	dumpList(lst, dumpFile);
	fclose(dumpFile);
	deleteList(lst);

	return VALIDATION_FAILS;
}

int assertListParameters(list* lst, const char* function, bool status, const char* error) {
	if (status) {
		return OK;
	}

	FILE* assertFile = fopen("assert.txt", "a");

	fprintf(assertFile, "=== Error in function: %s ===\n", function);
	fprintf(assertFile, "%s\n\n", error);

	fclose(assertFile);

	FILE* dumpFile = fopen("dump.txt", "a");
	dumpList(lst, dumpFile);
	fclose(dumpFile);

	deleteList(lst);

	return VALIDATION_FAILS;
}

int errorListPrint(FILE* errorFile, const int code) {
	switch (code) {
		case NULL_POINTER:
			fprintf(errorFile, "list = NULL\n");
			break;
		case INCORRECT_SIZE:
			fprintf(errorFile, "list->size is incorrect\n");
			break;
		case NULL_VALUE_POINTER:
			fprintf(errorFile, "list->str = NULL\n");
			break;
		case INCORRECT_INSIDE_POINTER:
			fprintf(errorFile, "node->prev / node->next is incorrect\n");
			break;
		default:
			break;
	}

	return code;
}

int dumpList(list* lst, FILE* dumpFile) {
	
	fprintf(dumpFile, "\n========== DUMP_LIST ==========\n");
	fprintf(dumpFile, "list = %p\n", lst);
	fprintf(dumpFile, "list->head = %p\n", lst->head);
	fprintf(dumpFile, "list->tail = %p\n", lst->tail);
	fprintf(dumpFile, "list->size = %d\n", lst->size);

	node* tempNode = lst->head;
	for (int i = 1; tempNode != NULL; i++) {
		fprintf(dumpFile, "\nnode[%d]->str = %s (pointer = %p, prev = %p, next = %p)", i, tempNode->str, tempNode, tempNode->prev, tempNode->next);

		tempNode = tempNode->next;
	}
	
	fprintf(dumpFile, "\n===============================\n");

	return OK;
}

node* initNode() {
	node* newNode = (node*)calloc(1, sizeof(node));

	newNode->str = "";
	newNode->next = NULL;
	newNode->prev = NULL;

	return newNode;
}

node* initNode(char* str, node* prev, node* next) {
	node* newNode = (node*)calloc(1, sizeof(node));

	newNode->str = str;
	newNode->next = prev;
	newNode->prev = next;

	return newNode;
}

list* initList() {
	list* lst = (list*)calloc(1, sizeof(list));

	lst->head = NULL;
	lst->tail = NULL;
	lst->size = 0;

	ASSERT_OK(List, lst, __func__);
	return lst;
}

int deleteList(list* lst) {
	cleanList(lst);

	delete lst;
	lst = NULL;

	return OK;
}

char* deleteNode(list* lst, node* nd) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, nd != NULL, "node = NULL");

	char* str = nd->str;
	nd->str = "";

	if (nd->prev != NULL && nd->next != NULL) {
		nd->next->prev = nd->prev;
		nd->prev->next = nd->next;
	}
	if (nd->prev == NULL && nd->next != NULL) {
		lst->head = nd->next;
		lst->head->prev = NULL;
	}
	if (nd->prev != NULL && nd->next == NULL) {
		lst->tail = nd->prev;
		lst->tail->next = NULL;
	}
	if (nd->prev == NULL && nd->next == NULL) {
		lst->head = NULL;
		lst->tail = NULL;
	}

	delete nd;
	nd = NULL;

	lst->size--;

	ASSERT_OK(List, lst, __func__);
	return str;
}

int cleanList(list* lst) {
	ASSERT_OK(List, lst, __func__);

	if (lst->size == 0) {
		return OK;
	}
	if (lst->size == 1) {
		deleteNode(lst, lst->head);
		return OK;
	}

	node* tempNode = lst->head->next;
	do {
		deleteNode(lst, tempNode->prev);
		tempNode = tempNode->next;

	} while (tempNode != lst->tail);
	deleteNode(lst, tempNode);

	lst->head = NULL;
	lst->tail = NULL;
	lst->size = 0;

	ASSERT_OK(List, lst, __func__);
	return OK;
}

char* insertBefore(list* lst, node* nd, char* str) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, nd != NULL, "node = NULL");
	ASSERT_PARAMETERS(lst, __func__, str != NULL, "str = NULL");

	node* newNode = initNode(str, NULL, NULL);

	if (nd == lst->head) {
		newNode->next = nd;
		nd->prev = newNode;
		lst->head = newNode;
	}
	else {
		newNode->next = nd;
		newNode->prev = nd->prev;
		nd->prev->next = newNode;
		nd->prev = newNode;
	}

	lst->size++;

	ASSERT_OK(List, lst, __func__);
	return str;
}

char* insertAfter(list* lst, node* nd, char* str) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, nd != NULL, "node = NULL");
	ASSERT_PARAMETERS(lst, __func__, str != NULL, "str = NULL");

	node* newNode = initNode(str, NULL, NULL);

	if (nd == lst->tail) {
		newNode->prev = lst->tail;
		lst->tail->next = newNode;
		lst->tail = newNode;
	}
	else {
		newNode->prev = nd;
		newNode->next = nd->next;
		nd->next->prev = newNode;
		nd->next = newNode;
	}

	lst->size++;

	ASSERT_OK(List, lst, __func__);
	return str;
}

char* insertInEmpty(list* lst, char* str) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, lst->size == 0, "list->size != 0");
	ASSERT_PARAMETERS(lst, __func__, str != NULL, "str = NULL");

	node* newNode = initNode(str, NULL, NULL);

	lst->head = newNode;
	lst->tail = newNode;
	lst->size++;

	ASSERT_OK(List, lst, __func__);
	return str;
}

char* pushFront(list* lst, char* str) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, str != NULL, "str = NULL");

	if (lst->size == 0) {
		insertInEmpty(lst, str);
	}
	else {
		insertBefore(lst, lst->head, str);
	}
	
	ASSERT_OK(List, lst, __func__);
	return str;
}

char* pushBack(list* lst, char* str) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, str != NULL, "str = NULL");

	if (lst->size == 0) {
		insertInEmpty(lst, str);
	}
	else {
		insertAfter(lst, lst->tail, str);
	}
	
	ASSERT_OK(List, lst, __func__);
	return str;
}

char* popFront(list* lst) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, lst->size != 0, "list->size = 0");

	char* str = lst->head->str;

	deleteNode(lst, lst->head);

	ASSERT_OK(List, lst, __func__);
	return str;
}

char* popBack(list* lst) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, lst->size != 0, "list->size = 0");

	char* str = lst->tail->str;

	deleteNode(lst, lst->tail);

	ASSERT_OK(List, lst, __func__);
	return str;
}

node* nodeByNumber(list* lst, int pos) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, 0 < pos && pos <= lst->size, "pos <= 0 || list->size < pos");

	printf("\n!!! Using of function \"%s\" is irrational !!!\n", __func__);

	node* tempNode = lst->head;
	for (int i = 1; i < pos; i++) {
		tempNode = tempNode->next;
	}

	ASSERT_OK(List, lst, __func__);
	return tempNode;
}

node* findList(list* lst, char* str) {
	ASSERT_OK(List, lst, __func__);
	ASSERT_PARAMETERS(lst, __func__, str != NULL, "str = NULL");

	//printf("\n!!! Using of function \"%s\" is irrational !!!\n", __func__);

	node* tempNode = lst->head;
	node* nodeFound = NULL;
	while (tempNode != NULL) {
		if (strcmp(tempNode->str, str) == 0) {
			nodeFound = tempNode;
			break;
		}

		tempNode = tempNode->next;
	}

	ASSERT_OK(List, lst, __func__);
	return nodeFound;
}

