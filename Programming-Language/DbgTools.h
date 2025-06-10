#ifndef DBGTOOLS_H
#define DBGTOOLS_H

#include "Parser.h"
#include "Lexer.h"
#include <math.h>

//This header file contains useful functions for debugging the compiler itself

//This is to make the process of debugging the AST easier to see if it is working properly
//This function assumes you are entering the root node of the AST
int AST_navigator(tokenList* list, AST** ast) {
	int shouldExit = false;
	int layer = 0;
	int node = 0;

	//Clear the screen and set cursor position to home
	printf("\x1b[2J\x1b[0;0H");

	printf("Current Node:\n");
	AST_print(list, ast);

	printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);

	while (!shouldExit) {
		int errorMessage = 0;
		if (GetAsyncKeyState(VK_ESCAPE) & 0x01) {
			shouldExit = true;
			continue;
		}

		if (GetAsyncKeyState(VK_UP) & 0x01) {
			//Clear the screen and set cursor position to home
			printf("\x1b[2J\x1b[0;0H");

			if (!AST_ascend(ast)) {
				errorMessage = 1;
			}
			else {
				layer--;
				node = (**ast).position;

				if ((**ast).prevNode != NULL) {
					printf("Parent Node:\n");
					AST_print(list, (AST**)(&(**ast).prevNode));
				}
			}

			//This is done for formatting purposes so that things look nice
			if (layer != 0) {
				printf("\n");
			}

			printf("Current Node:\n");
			AST_print(list, ast);

			printf("\n\nLayer: %d | Column: %d\n", layer, node);

			if (errorMessage == 1) {
				printf("\n\x1b[31mCannot Ascend any further; Root Node Reached\x1b[0m\n\n");
			}
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x01) {
			//Clear the screen and set cursor position to home
			printf("\x1b[2J\x1b[0;0H");

			//For simplicity, this function will always descend down to the first node in the list
			if (!AST_descend(ast, 0)) {
				errorMessage = 1;
			}
			else {
				layer++;
				node = 0;

				printf("Parent Node:\n");
				AST_print(list, (AST**)(&(**ast).prevNode));
			}

			//This is done for formatting purposes so that things look nice
			if (layer != 0) {
				printf("\n");
			}
			printf("Current Node:\n");
			AST_print(list, ast);

			printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);

			if (errorMessage == 1) {
				printf("\n\x1b[31mCannot descend any further\x1b[0m\n\n");
			}
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x01) {
			//Clear the screen and set cursor position to home
			printf("\x1b[2J\x1b[0;0H");

			if ((AST*)((**ast).prevNode) != NULL) {
				if ((**ast).position - 1 < 0) {
					errorMessage = 1;
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

				if (errorMessage == 1) {
					printf("\n\x1b[31mCannot move left anymore; First node in list reached\x1b[0m\n\n");
				}
			}
			else {
				printf("Current Node:\n");
				AST_print(list, ast);

				printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);

				printf("\n\x1b[31mCannot move left; This is the only node in the list\x1b[0m\n");
			}
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x01) {
			//Clear the screen and set cursor position to home
			printf("\x1b[2J\x1b[0;0H");

			if ((AST*)((**ast).prevNode) != NULL) {
				if ((**ast).position + 1 >= ((AST*)((**ast).prevNode))->list.len) {
					errorMessage = 1;
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

				if (errorMessage == 1) {
					printf("\n\x1b[31mCannot move right anymore; Last node in list reached\x1b[0m\n\n");
				}
			}
			else {

				printf("Current Node:\n");
				AST_print(list, ast);

				printf("\n\nLayer: %d | Column: %d\n", layer, (**ast).position);

				printf("\n\x1b[31mCannot move right; This is the only node in the list\x1b[0m\n\n");
			}
		}
	}

	return 0;
}

int Token_navigator(tokenList* list) {
	int index = 0;
	bool shouldContinue = true;

	//Clear the screen and set cursor position to home
	printf("\x1b[2J\x1b[0;0H");

	tokenList_print_individual(list->tokens[index]);
	printf("\nIndex: %d\n\n", index);

	while (shouldContinue) {
		if (GetAsyncKeyState(VK_ESCAPE) & 0x01) {
			shouldContinue = false;
			continue;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x01) {
			if (index + 1 < list->len) {
				index++;
				//Clear the screen and set cursor position to home
				printf("\x1b[2J\x1b[0;0H");
				tokenList_print_individual(list->tokens[index]);
				printf("\nIndex: %d\n\n", index);
			}
			else {
				printf("\x1b[2J\x1b[0;0H");
				tokenList_print_individual(list->tokens[index]);
				printf("\nIndex: %d\n\n", index);
				printf("\x1b[31mYou have reached the end of the token list\x1b[0m\n\n");
			}
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x01) {
			if (index - 1 >= 0) {
				index--;
				//Clear the screen and set cursor position to home
				printf("\x1b[2J\x1b[0;0H");
				tokenList_print_individual(list->tokens[index]);
				printf("\nIndex: %d\n\n", index);
			}
			else {
				printf("\x1b[2J\x1b[0;0H");
				tokenList_print_individual(list->tokens[index]);
				printf("\nIndex: %d\n\n", index);
				printf("\x1b[31mYou are at the first element in the token list\x1b[0m\n\n");
			}
		}
	}

	return 0;
}

void Debug_navigator_options(int num) {
	switch (num) {
	case 0:
		printf("-------------- Debug Navigator --------------\n");
		printf("| Token Navigator  <---                     |\n");
		printf("| AST Navigator                             |\n");
		printf("| Exit Program                              |\n");
		printf("---------------------------------------------\n");
		break;
	case 1:
		printf("-------------- Debug Navigator --------------\n");
		printf("| Token Navigator                           |\n");
		printf("| AST Navigator    <---                     |\n");
		printf("| Exit Program                              |\n");
		printf("---------------------------------------------\n");
		break;
	case 2:
		printf("-------------- Debug Navigator --------------\n");
		printf("| Token Navigator                           |\n");
		printf("| AST Navigator                             |\n");
		printf("| Exit Program     <---                     |\n");
		printf("---------------------------------------------\n");
	}
}

int Debug_navigator(tokenList* list, AST** ast) {
	printf("\x1b[2J\x1b[0;0H");
	printf("-------------- Debug Navigator --------------\n");
	printf("| Token Navigator  <---                     |\n");
	printf("| AST Navigator                             |\n");
	printf("| Exit Program                              |\n");
	printf("---------------------------------------------\n");

	bool shouldContinue = true;
	int index = 0;

	while (shouldContinue) {
		if (GetAsyncKeyState(VK_UP) & 0x01) {
			printf("\x1b[2J\x1b[0;0H");
			index = index - 1;
			if (index < 0) {
				index = 2;
			}
			Debug_navigator_options(index);
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x01) {
			printf("\x1b[2J\x1b[0;0H");
			index = (index + 1) % 3;
			Debug_navigator_options(index);
		}

		if (GetAsyncKeyState(VK_RETURN) & 0x01) {
			switch (index) {
			case 0:
				Token_navigator(list);
				printf("\x1b[2J\x1b[0;0H");
				printf("-------------- Debug Navigator --------------\n");
				printf("| Token Navigator  <---                     |\n");
				printf("| AST Navigator                             |\n");
				printf("| Exit Program                              |\n");
				printf("---------------------------------------------\n");
				index = 0;
				break;
			case 1:
				AST_navigator(list, ast);
				printf("\x1b[2J\x1b[0;0H");
				printf("-------------- Debug Navigator --------------\n");
				printf("| Token Navigator  <---                     |\n");
				printf("| AST Navigator                             |\n");
				printf("| Exit Program                              |\n");
				printf("---------------------------------------------\n");
				index = 0;
				break;
			case 2:
				shouldContinue = false;
				break;
			}
		}
	}
}


#endif
