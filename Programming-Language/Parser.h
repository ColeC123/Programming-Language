#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "Lexer.h"

enum UP_RELATION_CONSTANTS {
	UREL_BODY = 0,
	UREL_CONDITION = 1,
	UREL_IF_BODY = 2,
	UREL_ELSE_BODY = 3,
	//There could be some situations where it might be useful to have no upwards relation indicated, but I can't think
	//of any right now so I've added it just in case
	UREL_IRRELEVENT = 4,
};

enum AST_TYPES {
	AST_ASSIGN = 0,
	AST_LOOP_WHILE = 1,
	AST_LOOP_FOR = 2,
	AST_ADD = 3,
	AST_SUBTRACT = 4,
	AST_MULTIPLY = 5,
	AST_DIVIDE = 6,
	//The first node of the program in the AST
	AST_ROOT = 7,
};

typedef struct AST_List {
	//This has to be a void pointer because for some reason visual studio doesn't recognize the AST struct as existing
	//yet since it doesn't come before this struct, and if I place the AST struct before this one then the visual studio
	//won't be able to recognize the AST_List struct in the AST struct. This is annoying, but can be easily fixed by using
	//void pointers and then casting to AST*
	void* arr;
	int len;
	int __size;
} AST_List;

// The abstract syntax tree used to determine the semantics of the string of tokens output by the lexer
// Every single node should be treated as a pointer in order for the functions to work properly, even the root node
// Example decleration of an AST would be AST* ast; AST_init(&ast);
typedef struct AST {
	//The index will be the index of a token with the list of tokens passed into the parser function
	//This will simplify the process of freeing up memory at the end of program because the data for the tokens
	//is stored in an dynamic array (and each of the tokens themselves may or may not have data to be freed), whereas 
	// the freeing of memory in an abstract syntax tree is a bit more complicated
	int index;
	//upRelation stores what the current node's relation to the node above it is. For instnace, this would indicate
	//whether it is part of the condition of the previous node or is part of the body of the previous node
	int upRelation;
	//type refers to what function or purpose the current node serves, such as whether it is a binary operator, a variable, etc.
	int type;
	AST_List list;
	//Must be treated as a void pointer becaues visual studio compiler doesn't let you reference the pointer of a struct inside of
	//itself
	void* prevNode;
} AST;

void AST_List_init(AST_List* list) {
	list->len = 0;
	list->__size = 1;
	list->arr = NULL;
}

void AST_init(AST** ast) {
	*ast = (AST*)malloc(sizeof(AST));
	AST_List_init(&(*ast)->list);
	(*ast)->index = -1;
	(*ast)->type = AST_ROOT;
	(*ast)->upRelation = -1;
}

void AST_List_destroy(AST_List* list) {
	free(list->arr);
	list->len = 0;
	list->__size = 1;
	list->arr = NULL;
}

void AST_List_append(AST_List* list, AST node) {
	if (list->len + 1 >= list->__size) {
		list->__size *= 2;

		AST* test = (AST*)realloc(list->arr, list->__size * sizeof(AST));

		if (test == NULL) {
			printf("Failed to allocate memory in AST_List_append\n");
			exit(-1);
		}

		list->arr = test;
	}

	((AST*)list->arr)[list->len] = node;
	list->len++;
}

void AST_List_pop(AST_List* list) {
	if (list->len - 1 <= list->__size / 2) {
		list->__size /= 2;

		//Due to rounding, list->__size could end up with the value 0, which would cause this function to break
		if (list->__size <= 0) {
			list->__size = 1;
		}

		AST* test = (AST*)realloc(list->arr, list->__size * sizeof(AST));

		if (test == NULL) {
			printf("Failed to allocate memory in AST_List_pop\n");
			exit(-1);
		}

		list->arr = test;
	}

	list->len--;
}

void AST_descend(AST* ast, int index) {

}

int parser(tokenList* list, AST* ast) {

}

#endif