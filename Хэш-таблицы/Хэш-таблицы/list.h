#pragma once

const int OK = 0;
const int VALIDATION_FAILS = -1;

const int NULL_POINTER = -1;
const int NULL_VALUE_POINTER = 1;
const int INCORRECT_INSIDE_POINTER = 2;
const int INCORRECT_SIZE = 3;

struct node {
	char* str;
	node* prev;
	node* next;
};

struct list {
	int size;
	node* head;
	node* tail; 
};

int assertList(list* lst, const char* function);
int assertListParameters(list* lst, const char* function, bool status, const char* error);
int errorListPrint(FILE* errorFile, const int code);
int dumpList(list* lst, FILE* fileName);
node* initNode();
node* initNode(char* str, node* prev, node* next);
list* initList();
int deleteList(list* lst);
char* deleteNode(list* lst, node* nd);
int cleanList(list* lst);
char* insertBefore(list* lst, node* nd, char* str);
char* insertAfter(list* lst, node* nd, char* str);
char* insertInEmpty(list* lst, char* str);
char* pushFront(list* lst, char* str);
char* pushBack(list* lst, char* str);
char* popFront(list* lst);
char* popBack(list* lst);
node* nodeByNumber(list* lst, int pos);
node* findList(list* lst, char* str);
