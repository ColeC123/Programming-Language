#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "Strings.h"

enum TYPE {
	//+, -, *, /, etc.
	OPERATOR = 0,
	//Numbers, characters, strings, etc.
	LITERAL = 1,
	//Variables
	IDENTIFIER = 2,
	//Stuff like if, while, etc.
	KEYWORD = 3,
	//Stuff like (, ;, {, etc.
	PUNCTUATOR = 4,
};

enum LITERAL {
	INT_LITERAL = 0,
	FLOAT_LITERAL = 1,
	STRING_LITERAL = 2
};

enum KEYWORD {
	INT = 0,
	FLOAT = 1,
	STRING = 2
};

//It is important that no altering string operations are done to these, as
//they can contain only constant values
string operators[] = {
	{.str = "+", .len = 1, .__size = 2},
	{.str = "-", .len = 1, .__size = 2},
	{.str = "*", .len = 1, .__size = 2},
	{.str = "/", .len = 1, .__size = 2},
	{.str = "=", .len = 1, .__size = 2},
	{.str = "<", .len = 1, .__size = 2},
	{.str = ">", .len = 1, .__size = 2},
};

//It is important that no altering string operations are done to these, as
//they can contain only constant values
string keywords[] = {
	{.str = "int", .len = 3, .__size = 4},
	{.str = "float", .len = 5, .__size = 6},
	{.str = "string", .len = 6, .__size = 7}
};

//It is important that no altering string operations are done to these, as
//they can contain only constant values
string punctuators[] = {
	{.str = ";", .len = 1, .__size = 2}
};

#define NUM_OPERATORS sizeof(operators) / sizeof(operators[0])
#define NUM_KEYWORDS sizeof(keywords) / sizeof(keywords[0])
#define NUM_PUNCTUATORS sizeof(punctuators) / sizeof(punctuators[0])

typedef struct token {
	long long val;
	enum TYPE type;
	//Extra data, primarily for use with literals
	int mdata;
} token;

typedef struct tokenList {
	token* tokens;
	int len;
	int __size;
} tokenList;

void tokenList_init(tokenList* list);

void tokenList_destroy(tokenList* list);

int tokenList_append(tokenList* list, token tok);

int tokenList_pop(tokenList* list);

int lexer(tokenList* list, string* input);

void tokenList_print(tokenList* list);

void token_interpret_type(token tok);

void token_interpret_val(token tok);

void token_interpret_mdata(token tok);

// Returns true if current index is a keyword of the language
int lexer_is_keyword(string* str, int index, int index2);

int main(void) {
	string s1;
	string_init(&s1, NULL);
	string_load_file("C:\\Users\\colec\\C Programs\\Assembly\\text.txt", &s1);
	tokenList list;
	tokenList_init(&list);

	lexer(&list, &s1);
	return 0;
}

int lexer(tokenList* list, string* input) {
	string s1, s2;
	// Some files include the /r character that returns the cursor back to the start of the line
	// In order to make parsing easier, this character will be removed
	string_init(&s1, "\r");
	string_init(&s2, NULL);
	while (string_find_replace(input, &s1, &s2));
	printf("%s\n\n", input->str);

	string tempstr;
	string_init(&tempstr, NULL);

	int quoteCounter = 0;
	int prevQuoteCounter;
	int quoteIndices[2];
	bool wasCharacter = false;
	for (int i = 0; i < input->len; i++) {
		prevQuoteCounter = quoteCounter;
		// Skip over white space and newlines
		if (input->str[i] == ' ' || input->str[i] == '\n') {
			continue;
		}
		else if (input->str[i] == '"') {
			quoteIndices[quoteCounter % 2] = i;
			quoteCounter++;
		}

		if (quoteCounter % 2 == 0 && prevQuoteCounter % 2 == 1) {
			//Needs to be allocated on heap since this data needs to be persistent
			//Clean up will be handled later on
			string* s1 = (string*)malloc(sizeof(string));
			string_init(s1, NULL);
			string_substr(s1, input, quoteIndices[0] + 1, quoteIndices[1]);
			tokenList_append(list, (token) { .type = (enum TYPE)LITERAL, .val = (long long)s1, .mdata = (enum LITERAL)STRING });
		}

		//Only check for keywords, operators, etc. if not currently inside of string
		if (quoteCounter % 2 == 0 && input->str[i] != '"') {
			for (int j = 0; j < NUM_KEYWORDS; j++) {
				if (lexer_is_keyword(input, i, j)) {
					if (tempstr.str != NULL) {
						//Needs to be allocated on heap since this data needs to be persistent
						//Clean up will be handled later on
						string* s2 = (string*)malloc(sizeof(string));
						string_init(s2, NULL);
						string_copy(s2, &tempstr);
						string_destroy(&tempstr);
						tokenList_append(list, (token) { .type = (enum TYPE)LITERAL, .val = (long long)s2, .mdata = -1 });
					}

					tokenList_append(list, (token) { .type = (enum TYPE)KEYWORD, .val = j, .mdata = -1 });
					//Subtract one to account for the addition of i at the end of the iteration
					i = i + keywords[j].len - 1;
					goto exit_if1;
				}
			}

			for (int j = 0; j < NUM_OPERATORS; j++) {
				if (string_substr_cmp(input, i, &operators[j])) {
					if (tempstr.str != NULL) {
						//Needs to be allocated on heap since this data needs to be persistent
						//Clean up will be handled later on
						string* s3 = (string*)malloc(sizeof(string));
						string_init(s3, NULL);
						string_copy(s3, &tempstr);
						string_destroy(&tempstr);
						tokenList_append(list, (token) { .type = (enum TYPE)LITERAL, .val = (long long)s3, .mdata = -1 });
					}

					tokenList_append(list, (token) { .type = (enum TYPE)OPERATOR, .val = j, .mdata = -1 });
					//Subtract one to account for the addition of i at the end of the iteration
					i = i + operators[j].len - 1;
					goto exit_if1;
				}
			}

			string_append(&tempstr, input->str[i]);

		exit_if1:
			continue;
		}
	}

	if (tempstr.str != NULL) {
		//Needs to be allocated on heap since this data needs to be persistent
		//Clean up will be handled later on
		string* s4 = (string*)malloc(sizeof(string));
		string_init(s4, NULL);
		string_copy(s4, &tempstr);
		string_destroy(&tempstr);
		tokenList_append(list, (token) { .type = (enum TYPE)LITERAL, .val = (long long)s4, .mdata = -1 });
	}
	string_destroy(&tempstr);

	tokenList_print(list);

	string_destroy(&s1);
	string_destroy(&s2);

	return 0;
}

void tokenList_init(tokenList* list) {
	list->tokens = NULL;
	list->len = 0;
	list->__size = 1;
}

void tokenList_destroy(tokenList* list) {
	free(list->tokens);
	list->len = 0;
	list->__size = 1;
	list->tokens = NULL;
}

int tokenList_append(tokenList* list, token tok) {
	if (list->len + 1 >= list->__size) {
		list->__size *= 2;

		token* test = (token*)realloc(list->tokens, list->__size * sizeof(token));

		if (test == NULL) {
			printf("Failed to allocate memory in tokenList_append\n");
			exit(-1);
		}

		list->tokens = test;
	}

	list->tokens[list->len] = tok;
	list->len++;

	return 0;
}

int tokenList_pop(tokenList* list) {
	if (list->len - 1 <= list->__size / 2) {
		list->__size /= 2;

		token* test = (token*)realloc(list->tokens, list->__size * sizeof(token));

		if (test == NULL) {
			printf("Failed to allocate memory in tokenList_pop\n");
			exit(-1);
		}

		list->tokens = test;
	}

	list->len--;
}

void tokenList_print(tokenList* list) {
	for (int i = 0; i < list->len; i++) {
		token_interpret_type(list->tokens[i]);
		token_interpret_val(list->tokens[i]);
		token_interpret_mdata(list->tokens[i]);
		printf("\n");
	}
}

void token_interpret_type(token tok) {
	switch (tok.type) {
	case (enum TYPE)OPERATOR:
		printf("TYPE: OPERATOR\n");
		break;
	case (enum TYPE)LITERAL:
		printf("TYPE: LITERAL\n");
		break;
	case (enum TYPE)IDENTIFIER:
		printf("TYPE: IDENTIFIER\n");
		break;
	case (enum TYPE)KEYWORD:
		printf("TYPE: KEYWORD\n");
		break;
	case (enum TYPE)PUNCTUATOR:
		printf("TYPE: PUNCTUATOR\n");
		break;
	default:
		printf("TYPE: ERROR\n");
		break;
	}
}

void token_interpret_val(token tok) {
	switch (tok.type) {
	case (enum TYPE)OPERATOR:
		printf("VAL: %s\n", operators[tok.val].str);
		break;
	case (enum TYPE)LITERAL:
		string* ptr = tok.val;
		printf("VAL: %s\n", ptr->str);
		break;
		/*case (enum TYPE)IDENTIFIER:
			return "IDENTIFIER";*/
	case (enum TYPE)KEYWORD:
		printf("VAL: %s\n", keywords[tok.val].str);
		break;
		/*case (enum TYPE)PUNCTUATOR:
			return "PUNCTUATOR";*/
	default:
		printf("Val: ERROR");
		break;
	}
}

void token_interpret_mdata(token tok) {
	switch (tok.mdata) {
	case (enum LITERAL)INT_LITERAL:
		printf("MDATA: INT_LITERAL\n");
		break;
	case (enum LITERAL)FLOAT_LITERAL:
		printf("MDATA: FLOAT_LITERAL\n");
		break;
	case (enum LITERAL)STRING_LITERAL:
		printf("MDATA: STRING_LITERAL\n");
		break;
	default:
		printf("MDATA: NONE\n");
		break;
	}
}

int lexer_is_keyword(string* str, int index, int index2) {
	if (index == 0) {
		return string_substr_cmp(str, index, &keywords[index2]);
	}
	else {
		int end = index + keywords[index2].len;
		return string_substr_cmp(str, index, &keywords[index2])
			&& (str->str[index - 1] == ' ' || str->str[index - 1] == '\n')
			&& (str->str[end] == ' ' || str->str[end] == '\n' || str->str[end] == '\0');
	}
}

