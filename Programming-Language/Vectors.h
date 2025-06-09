#ifndef VECTORS_H
#define VECTORS_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Vector_Int {
	int* vec;
	int len;
	int __size;
} Vector_Int;

int Vector_Int_Init(Vector_Int* vec) {
	vec->vec = NULL;
	vec->len = 0;
	vec->__size = 1;
	return 0;
}

int Vector_Int_Append(Vector_Int* vec, int num) {
	if (vec->len + 1 >= vec->__size) {
		vec->__size *= 2;

		int* test = (int*)realloc(vec->vec, vec->__size * sizeof(int));

		if (test == NULL) {
			printf("Failed to allocate memory in Vector_Int_Append\n");
			exit(-1);
		}

		vec->vec = test;
	}

	vec->vec[vec->len] = num;
	vec->len++;
	return 0;
}

int Vector_Int_Pop(Vector_Int* vec) {
	if (vec->len - 1 <= vec->__size / 2) {
		vec->__size /= 2;

		//vec->__size could end up as being equal to 0, which is bad because when plugged back into realloc that would cause
		//it to function like free instead of actually allocating memory
		if (vec->__size <= 0) {
			vec->__size = 1;
		}

		int* test = (int*)realloc(vec->vec, vec->__size * sizeof(int));

		if (test == NULL) {
			printf("Failed to allocate memory in Vector_Int_Pop\n");
			exit(-1);
		}

		vec->vec = test;
	}

	vec->len--;
	return 0;
}

int Vector_Int_Destroy(Vector_Int* vec) {
	free(vec->vec);
	vec->vec = NULL;
	vec->len = 0;
	vec->__size = 1;
	return 0;
}

#endif