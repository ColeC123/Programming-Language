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

	AST* ast;
	AST_init(&ast);

	AST_append(&ast, (AST) { .token_index = 0, .type = AST_ASSIGN, .upRelation = UREL_BODY, .token_index = 0 });
	AST_append(&ast, (AST) { .token_index = 0, .type = AST_ADD, .upRelation = UREL_IF_BODY, .token_index = 1 });
	AST_append(&ast, (AST) { .token_index = 0, .type = AST_SUBTRACT, .upRelation = UREL_ELSE_BODY, .token_index = 2 });
	AST_append(&ast, (AST) { .token_index = 0, .type = AST_MULTIPLY, .upRelation = UREL_CONDITION, .token_index = 3 });
	AST_append(&ast, (AST) { .token_index = 0, .type = AST_DIVIDE, .upRelation = UREL_IRRELEVENT, .token_index = 4 });

	AST_navigator(&list, &ast);

	return 0;
}
