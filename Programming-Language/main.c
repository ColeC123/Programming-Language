#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "Strings.h"
#include "DynamicArray.h"
#include "Lexer.h"
#include "Parser.h"

int main(void) {
	string s1;
	string_init(&s1, NULL);
	string_load_file("C:\\Users\\colec\\C Programs\\Assembly\\text.txt", &s1);
	tokenList list;
	tokenList_init(&list);

	lexer(&list, &s1);
	tokenList_print(&list);

	AST ast;
	AST_init(&ast);

	return 0;
}
