#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "Strings.h"
#include "DynamicArray.h"

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
	//For things that the first pass of the lexer could not determine. The second stage of the lexer will
	//infer types based on the info of other tokens to properly tokenize expressions for example
	TYPE_UNDEFINED = 5,
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

void tokenList_insert(tokenList* list, int index, token tok);

void tokenList_remove(tokenList* list, int index);

int lexer(tokenList* list, string* input);

void tokenList_print(tokenList* list);

void token_interpret_type(token tok);

void token_interpret_val(token tok);

void token_interpret_mdata(token tok);

// Returns true if current index is a keyword of the language
int lexer_is_keyword(string* str, int index, int index2);

int is_keyword_variable_type(int type);

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

	string* tempstr = (string*)malloc(sizeof(string));
	string_init(tempstr, NULL);

	//Used to determine if currently inside a quote by using the modulus operator
	int quoteCounter = 0;
	//Used to determine if at a boundary where a quote ends and the rest of the code continues
	int prevQuoteCounter;
	//Used to make getting the substring of just the quote easier
	int quoteIndices[2];

	// This is the main loop that iterates through the given string and does the actual lexing
	for (int i = 0; i < input->len; i++) {
		prevQuoteCounter = quoteCounter;
		// Skip over white space and newlines to save time
		if (input->str[i] == ' ' || input->str[i] == '\n') {
			continue;
		}
		else if (input->str[i] == '"') {
			quoteIndices[quoteCounter % 2] = i;
			quoteCounter++;
		}

		//When quoteCounter % 2 == 0, that means that the program is not between 2 quotation marks, and if prevQuoteCounter % 2 == 1
		//that means the program was previously inside quotes. That means the string inside the quotes can now be added
		//as a token to the list
		if (quoteCounter % 2 == 0 && prevQuoteCounter % 2 == 1) {
			//Needs to be allocated on heap since this data needs to be persistent
			//Clean up will be handled later on
			string* s1 = (string*)malloc(sizeof(string));
			string_init(s1, NULL);
			string_substr(s1, input, quoteIndices[0] + 1, quoteIndices[1]);
			tokenList_append(list, (token) { .type = LITERAL, .val = (long long)s1, .mdata = STRING_LITERAL });
		}

		//Only check for keywords, operators, etc. if not currently inside of a string
		if (quoteCounter % 2 == 0 && input->str[i] != '"') {
			for (int j = 0; j < NUM_KEYWORDS; j++) {
				if (lexer_is_keyword(input, i, j)) {
					if (tempstr->str != NULL) {
						//Needs to be allocated on heap since this data needs to be persistent
						//Clean up will be handled later on
						string* s2 = (string*)malloc(sizeof(string));
						string_init(s2, NULL);
						string_copy(s2, tempstr);
						string_destroy(tempstr);
						tokenList_append(list, (token) { .type = TYPE_UNDEFINED, .val = (long long)s2, .mdata = -1 });
					}

					tokenList_append(list, (token) { .type = KEYWORD, .val = j, .mdata = -1 });
					//Subtract one to account for the addition of i at the end of the iteration
					i = i + keywords[j].len - 1;
					goto exit_if1;
				}
			}

			for (int j = 0; j < NUM_OPERATORS; j++) {
				if (string_substr_cmp(input, i, &operators[j])) {
					if (tempstr->str != NULL) {
						//Needs to be allocated on heap since this data needs to be persistent
						//Clean up will be handled later on
						string* s3 = (string*)malloc(sizeof(string));
						string_init(s3, NULL);
						string_copy(s3, tempstr);
						string_destroy(tempstr);
						tokenList_append(list, (token) { .type = TYPE_UNDEFINED, .val = (long long)s3, .mdata = -1 });
					}

					tokenList_append(list, (token) { .type = OPERATOR, .val = j, .mdata = -1 });
					//Subtract one to account for the addition of i at the end of the iteration
					i = i + operators[j].len - 1;
					goto exit_if1;
				}
			}

			//tempstr picks up all the characters not identified as keywords, operators, etc. by the lexer that are also not strings
			//since strings are handled separately, and will be appended as an unidentified token that will be determined by the parser
			string_append(tempstr, input->str[i]);

		exit_if1:
			continue;
		}
	}

	//Make sure that any extra characters picked up by tempstr are added as a token
	//Also, for a reason I haven't been able to figure out, if the text file ends with a string tempstr.str isn't NULL
	//which is why this if statement checks if the most recent token is a string so that a bad token isn't added to the list
	if (list->len > 0 && tempstr->str != NULL && list->tokens[list->len - 1].type != LITERAL && list->tokens[list->len - 1].mdata != STRING_LITERAL) {
		tokenList_append(list, (token) { .type = TYPE_UNDEFINED, .val = (long long)tempstr, .mdata = -1 });
	}

	//This list will contain the identifiers found from the tokens output from the code above
	string_list identifiers;
	string_list_init(&identifiers);

	//This iteration will look for identifiers, and append them to a list of identifiers
	//The reason the loop starts at 1 is because it has to look at the previous element, and if that happened at index 0
	//an index out of bounds error would occur
	for (int i = 1; i < list->len; i++) {
		//If the current token type is undefined and the previous type is a keyword that is also a variable type, then according
		//to the heuristic I am using that would make the current token an identifier
		if (list->tokens[i].type == TYPE_UNDEFINED && list->tokens[i-1].type == KEYWORD && is_keyword_variable_type(list->tokens[i-1].val)) {
			string_list_append(&identifiers, *((string*)list->tokens[i].val));
			list->tokens[i].type = IDENTIFIER;
		}
	}

	//Ideally in here there would be some sort of pass that checks to make sure that multiple identifiers of the same
	//name haven't been used more than once and would spit out some sort of error, but that can be added later

	for (int i = 0; i < identifiers.len; i++) {
		printf("Identifier %d: %s\n", i + 1, identifiers.strings[i].str);
	}
	printf("\n");

	//This loop properly identifies int_literals, float_literals, and identifiers from the remaining tokens that
	//were marked as TYPE_UNDEFINED because they could not be determined in the first stage of the lexer
	for (int i = 0; i < list->len; i++) {
		if (list->tokens[i].type == TYPE_UNDEFINED) {
			for (int j = 0; j < identifiers.len; j++) {
				int identifier_index = string_find((string*)list->tokens[i].val, &identifiers.strings[j]);

				if (identifier_index != -1) {
					list->tokens[i].type = IDENTIFIER;
					break;
				}
			}

			int int_count = 0;
			for (int j = 0; j < ((string*)list->tokens[i].val)->len; j++) {
				if (((string*)list->tokens[i].val)->str[j] >= 48 && ((string*)list->tokens[i].val)->str[j] <= 57) {
					int_count++;
				}
				else {
					break;
				}
			}

			if (int_count == ((string*)list->tokens[i].val)->len) {
				//Make copy of string in token before freeing contents of that token to make space for the integer
				string s4;
				string_init(&s4, ((string*)list->tokens[i].val)->str);

				//Free the string pointer inside the string
				string_destroy((string*)list->tokens[i].val);
				//Free the actual struct since it was allocated on the heap
				free((string*)list->tokens[i].val);

				list->tokens[i].val = atoll(s4.str);
				list->tokens[i].type = LITERAL;
				list->tokens[i].mdata = INT_LITERAL;

				string_destroy(&s4);
			}
		}
	}

	tokenList_print(list);

	//Just the strings pointer itself needs to be freed, but the str pointer in each string needs to be preserved
	//because they are referenced in the token list and will be freed at a later time in the program
	free(identifiers.strings);
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

void tokenList_insert(tokenList* list, int index, token tok) {
	if (list->len + 1 >= list->__size) {
		list->__size *= 2;

		token* test = (token*)realloc(list->tokens, list->__size * sizeof(token));

		if (test == NULL) {
			printf("Failed to allocate memory in tokenList_append\n");
			exit(-1);
		}

		list->tokens = test;
	}

	list->len++;

	token prev = list->tokens[index];
	token curr;
	list->tokens[index] = tok;

	for (int i = index + 1; i < list->len; i++) {
		curr = list->tokens[i];
		list->tokens[i] = prev;
		prev = curr;
	}
}

void tokenList_remove(tokenList* list, int index) {
	if (list->len - 1 <= list->__size / 2) {
		list->__size /= 2;

		token* test = (token*)realloc(list->tokens, list->__size * sizeof(token));

		if (test == NULL) {
			printf("Failed to allocate memory in tokenList_pop\n");
			exit(-1);
		}

		list->tokens = test;
	}

	for (int i = index; i < list->len - 1; i++) {
		list->tokens[i] = list->tokens[i + 1];
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
	case (enum TYPE)TYPE_UNDEFINED:
		printf("TYPE: UNDEFINED\n");
		break;
	default:
		printf("TYPE: ERROR\n");
		break;
	}
}

void token_interpret_val(token tok) {
	string* ptr;
	double temp;
	switch (tok.type) {
	case (enum TYPE)OPERATOR:
		printf("VAL: %s\n", operators[tok.val].str);
		break;
	case (enum TYPE)LITERAL:
		if (tok.mdata == STRING_LITERAL) {
			ptr = tok.val;
			printf("VAL: %s\n", ptr->str);
		}
		else if (tok.mdata == INT_LITERAL) {
			printf("VAL: %lld\n", tok.val);
		}
		else if (tok.mdata == FLOAT_LITERAL) {
			temp = *(double*)(&tok.val);
			printf("VAL: %lf", temp);
		}
		break;
	case (enum TYPE)IDENTIFIER:
		ptr = tok.val;
		printf("VAL: %s\n", ptr->str);
		break;
	case (enum TYPE)KEYWORD:
		printf("VAL: %s\n", keywords[tok.val].str);
		break;
		/*case (enum TYPE)PUNCTUATOR:
			return "PUNCTUATOR";*/
	case (enum TYPE)TYPE_UNDEFINED:
		ptr = tok.val;
		printf("VAL: %s\n", ptr->str);
		break;
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

int is_keyword_variable_type(int type) {
	if (type < 3) {
		return true;
	}
	else {
		return false;
	}
}
