#ifndef STRINGS_H
#define STRINGS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct string {
	char* str;
	int len;
	// Actual size of the data stored in memory
	int __size;
} string;

// the init pointer must be null terminated, otherwise this function is unsafe
int string_init(string* str, char* init) {
	if (init != NULL) {
		str->len = 0;
		for (int i = 0; init[i] != '\0'; i++) {
			str->len++;
		}
		// Add one to account for null terminator at the end of the string for easy compliance with c functions, like printf
		str->__size = str->len + 1;
		str->str = (char*)malloc(str->__size * sizeof(char));

		if (str->str == NULL) {
			printf("Failed to allocate memory for initialization of string\n");
			exit(-1);
		}

		for (int i = 0; i < str->len; i++) {
			str->str[i] = init[i];
		}
		str->str[str->len] = '\0';
	}
	else {
		str->len = 0;
		str->__size = 1;
		str->str = NULL;
	}
	return 0;
}

int string_set(string* str, char* set) {
	if (set != NULL) {
		free(str->str);

		str->len = 0;
		for (int i = 0; set[i] != '\0'; i++) {
			str->len++;
		}

		str->__size = str->len + 1;
		str->str = (char*)malloc(str->__size * sizeof(char));

		for (int i = 0; i < str->len; i++) {
			str->str[i] = set[i];
		}
		str->str[str->len] = '\0';
	}
	else {
		str->str = NULL;
		str->len = 0;
		str->__size = 1;
	}

	return 0;
}

int string_copy(string* dest, string* src) {
	free(dest->str);
	dest->len = src->len;
	dest->__size = src->__size;
	dest->str = (char*)malloc(dest->__size * sizeof(char));

	if (dest->str == NULL) {
		printf("Failed to allocate memory in string_copy function\n");
		exit(-1);
	}

	for (int i = 0; i < dest->len; i++) {
		dest->str[i] = src->str[i];
	}
	dest->str[dest->len] = '\0';
	return 0;
}

int string_concat(string* firstHalf, string* secondHalf) {
	// Double the size of the array in memory to reduce calls to realloc
	if (firstHalf->__size < firstHalf->len + secondHalf->len + 1) {
		char* test = (char*)realloc(firstHalf->str, 2 * (firstHalf->len + secondHalf->len + 1) * sizeof(char));

		if (test == NULL) {
			printf("Failed to reallocate memory for string_concat function\n");
			exit(-1);
		}
		firstHalf->str = test;
		firstHalf->__size = 2 * (firstHalf->len + secondHalf->len + 1);
	}

	for (int i = 0; i < secondHalf->len; i++) {
		firstHalf->str[firstHalf->len + i] = secondHalf->str[i];
	}
	firstHalf->len = firstHalf->len + secondHalf->len;
	firstHalf->str[firstHalf->len] = '\0';
	return 0;
}

int string_load_file(char* path, string* str) {
	FILE* fptr = fopen(path, "rb");

	if (fptr == NULL) {
		printf("Failed to open file with string_load_file\n");
		exit(-1);
	}

	int fsize;
	fseek(fptr, 0L, SEEK_END);
	fsize = ftell(fptr) / sizeof(char);
	fseek(fptr, 0L, SEEK_SET);

	free(str->str);
	str->len = fsize;
	str->__size = str->len + 1;
	str->str = (char*)malloc(str->__size * sizeof(char));

	if (str->str == NULL) {
		printf("Failed to allocate memory for string_load_file\n");
		exit(-1);
	}

	fread(str->str, sizeof(char), str->len, fptr);
	str->str[str->len] = '\0';

	fclose(fptr);
	return 0;
}

int string_substr(string* dest, string* src, int from, int to) {
	if (from == to) {
		return 0;
	}

	if (dest->__size < to - from + 1) {
		char* test = (char*)realloc(dest->str, 2 * (to - from + 1) * sizeof(char));

		if (test == NULL) {
			printf("Failed to Reallocate memory in string_substr\n");
			exit(-1);
		}
		dest->str = test;
		dest->__size = 2 * (to - from + 1);
	}
	dest->len = to - from;

	for (int i = 0; i < dest->len; i++) {
		dest->str[i] = src->str[i + from];
	}
	dest->str[dest->len] = '\0';
	return 0;
}

int string_find_replace(string* dest, string* find, string* replace) {
	if (dest->__size < dest->len - find->len + replace->len + 1) {
		char* test = (char*)realloc(dest->str, 2 * (dest->len - find->len + replace->len + 1) * sizeof(char));

		if (test == NULL) {
			printf("Failed to Reallocate memory in string_find_replace\n");
			exit(-1);
		}
		dest->str = test;
		dest->__size = 2 * (dest->len - find->len + replace->len + 1);
	}

	int index = -1;
	for (int i = 0; i < dest->len; i++) {
		if (dest->str[i] == find->str[0]) {
			int count = 1;
			for (int j = i + 1; j < i + find->len; j++) {
				if (dest->str[j] == find->str[j - i]) {
					count++;
				}
				else {
					break;
				}
			}

			if (count == find->len) {
				index = i;
				break;
			}
		}
	}

	// Indicate that the find string was not found anywhere in the dest string
	if (index == -1) {
		return false;
	}

	string s1, s2;
	string_init(&s1, NULL);
	string_init(&s2, NULL);

	string_substr(&s1, dest, 0, index);
	string_substr(&s2, dest, index + find->len, dest->len);

	int count = 0;
	for (int i = 0; i < s1.len; i++) {
		dest->str[count] = s1.str[i];
		count++;
	}
	for (int i = 0; i < replace->len; i++) {
		dest->str[count] = replace->str[i];
		count++;
	}
	for (int i = 0; i < s2.len; i++) {
		dest->str[count] = s2.str[i];
		count++;
	}
	dest->str[count] = '\0';

	string_destroy(&s1);
	string_destroy(&s2);

	dest->len = dest->len - find->len + replace->len + 1;
	return true;
}

int string_destroy(string* input) {
	free(input->str);
	input->str = NULL;
	input->len = 0;
	input->__size = 1;
	return 0;
}

int string_write_file(char* path, string* input) {
	FILE* fptr = fopen(path, "wb");

	if (fptr == NULL) {
		printf("Failed to open or create file in string_write_file\n");
		exit(-1);
	}

	fwrite(input->str, sizeof(char), input->len, fptr);

	fclose(fptr);
	return 0;
}

// Returns true if the substring of large [start, start + small.len) is equal to the string of small, and returns false if otherwise
int string_substr_cmp(string* large, int start, string* small) {
	for (int i = 0; i < small->len; i++) {
		if (i + start >= large->len) {
			return false;
		} else if (large->str[i + start] != small->str[i]) {
			return false;
		}
	}

	return true;
}

int string_append(string* str, char letter) {
	if (str->len + 2 >= str->__size) {
		str->__size *= 2;
		char* test = (char*)realloc(str->str, str->__size * sizeof(char));

		if (test == NULL) {
			printf("string_append failed to allocate memory\n");
			exit(-1);
		}

		str->str = test;
	}

	str->str[str->len] = letter;
	str->len++;
	str->str[str->len] = '\0';
	return 0;
}

#endif