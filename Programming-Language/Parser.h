#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "Lexer.h"
#include <Windows.h>
#include <stdbool.h>

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
	//The token_index will be the index of a token with the list of tokens passed into the parser function
	//This will simplify the process of freeing up memory at the end of program because the data for the tokens
	//is stored in an dynamic array (and each of the tokens themselves may or may not have data to be freed), whereas 
	// the freeing of memory in an abstract syntax tree is a bit more complicated
	int token_index;
	//upRelation stores what the current node's relation to the node above it is. For instnace, this would indicate
	//whether it is part of the condition of the previous node or is part of the body of the previous node
	int upRelation;
	//type refers to what function or purpose the current node serves, such as whether it is a binary operator, a variable, etc.
	int type;
	AST_List list;
	//Must be treated as a void pointer becaues visual studio compiler doesn't let you reference the pointer of a struct inside of
	//itself
	void* prevNode;
	//The location of the AST node in the list of AST nodes
	int position;
} AST;

void AST_List_init(AST_List* list) {
	list->len = 0;
	list->__size = 1;
	list->arr = NULL;
}

void AST_init(AST** ast) {
	*ast = (AST*)malloc(sizeof(AST));
	AST_List_init(&(*ast)->list);
	(*ast)->token_index = -1;
	(*ast)->type = AST_ROOT;
	(*ast)->upRelation = -1;
	(*ast)->prevNode = NULL;
	(*ast)->position = 0;
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

	node.position = list->len;
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

int AST_descend(AST** ast, int index) {
	if ((*ast)->list.arr != NULL && index < (*ast)->list.len) {
		*ast = &((AST*)(**ast).list.arr)[index];
		return true;
	}
	else {
		return false;
	}
}

int AST_ascend(AST** ast) {
	if ((**ast).prevNode != NULL) {
		*ast = (AST*)(**ast).prevNode;
		return true;
	}
	else {
		return false;
	}
}

//Appends an AST node to the list of AST nodes in the current node
//The double pointer for ast is unnecessary, but is used for consistency with the other functions
//Note: the list in the node being appended can be left as null since that initialization of the list will be handled internally
//by the function
void AST_append(AST** ast, AST node) {
	//Initialize the list in the node so it is set up properly for potential later use
	AST_List_init(&node.list);
	//Make sure that before the node is appended, it points to the current node which will be the previous node for the node being appended
	node.prevNode = *ast;
	AST_List_append(&(**ast).list, node);
}

void AST_pop(AST** ast) {
	AST_List_pop(&(**ast).list);
}

void AST_print(tokenList* list, AST** ast) {
	tokenList_print_individual(list->tokens[(**ast).token_index]);
	switch ((**ast).upRelation) {
	case UREL_BODY:
		printf("UREL: BODY\n");
		break;
	case UREL_CONDITION:
		printf("UREL: CONDITION\n");
		break;
	case UREL_IF_BODY:
		printf("UREL: IF BODY\n");
		break;
	case UREL_ELSE_BODY:
		printf("UREL: ELSE BODY\n");
		break;
	case UREL_IRRELEVENT:
		printf("UREL: IRRELEVENT\n");
		break;
	default:
		printf("UREL: ERROR\n");
	}
}

//This is to make the process of debugging the AST easier to see if it is working properly
//This function assumes you are entering the root node of the AST
int AST_navigator(tokenList* list, AST** ast) {
	bool shouldExit = false;
	int layer = 0;
	int node = 0;

	//Clear the screen and set cursor position to home
	printf("\x1b[2J\x1b[0;0H");

	printf("\nCurrent Node:\n");
	AST_print(list, ast);

	printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);

	while (!shouldExit) {
		if (GetAsyncKeyState(VK_ESCAPE) & 0x01) {
			shouldExit = true;
			continue;
		}

		if (GetAsyncKeyState(VK_UP) & 0x01) {
			//Clear the screen and set cursor position to home
			printf("\x1b[2J\x1b[0;0H");

			if (!AST_ascend(ast)) {
				printf("Cannot Ascend any further; Root Node Reached\n\n");
			}
			else {
				layer--;
				node = (**ast).position;
				printf("Parent Node:\n");
				AST_print(list, (AST**)(&(**ast).prevNode));
			}

			printf("\nCurrent Node:\n");
			AST_print(list, ast);

			printf("\n\nLayer: %d | Column: %d\n", layer, node);
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x01) {
			//Clear the screen and set cursor position to home
			printf("\x1b[2J\x1b[0;0H");

			//For simplicity, this function will always descend down to the first node in the list
			if (!AST_descend(ast, 0)) {
				printf("Cannot descend any further\n\n");
			}
			else {
				layer++;
				node = 0;
			}

			printf("Parent Node:\n");
			AST_print(list, (AST**)(&(**ast).prevNode));

			printf("\nCurrent Node:\n");
			AST_print(list, ast);

			printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x01) {
			//Clear the screen and set cursor position to home
			printf("\x1b[2J\x1b[0;0H");

			if ((AST*)((**ast).prevNode) != NULL) {
				if ((**ast).position - 1 <= 0) {
					printf("Cannot move left anymore; First node in list reached\n\n");
				}
				else {
					node--;
					//This looks complicated, but all it is doing is setting the current node to be equal to the next node in the list
					//What complicates this process is that this list is stored in the previous node, so you have to get the node from the list
					//in the previous node, and since void pointers had to be used that means anywhere a void pointer shows up it has to be cast
					//to an AST pointer in order for the code to work correctly so the program knows how to retrieve the data from the AST
					//structure properly
					*ast = &(((AST*)(((AST*)(**ast).prevNode)->list.arr))[node]);
				}

				printf("Parent Node:\n");
				AST_print(list, (AST**)(&(**ast).prevNode));

				printf("\nCurrent Node:\n");
				AST_print(list, ast);

				printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);
			}
			else {
				printf("Cannot move left; This is the only node in the list\n\n");

				printf("\nCurrent Node:\n");
				AST_print(list, ast);

				printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);
			}
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x01) {
			//Clear the screen and set cursor position to home
			printf("\x1b[2J\x1b[0;0H");

			if ((AST*)((**ast).prevNode) != NULL) {
				if ((**ast).position + 1 >= ((AST*)((**ast).prevNode))->list.len) {
					printf("Cannot move right anymore; Last node in list reached\n\n");
				}
				else {
					node++;
					*ast = &(((AST*)(((AST*)(**ast).prevNode)->list.arr))[node]);
				}

				printf("Parent Node:\n");
				AST_print(list, (AST**)(&(**ast).prevNode));

				printf("\nCurrent Node:\n");
				AST_print(list, ast);

				printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);
			}
			else {
				printf("Cannot move right; This is the only node in the list\n\n");

				printf("\nCurrent Node:\n");
				AST_print(list, ast);

				printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);
			}
		}
	}


}

int parser(tokenList* list, AST** ast) {

}

#endif