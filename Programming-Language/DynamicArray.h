#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Vector {
	void* arr;
	int len;
	int __size;
	//Stores the sizeof(type, struct, etc.)
	int __element_size;
	//This is the function that is used to properly set the value of elements in the array
	void (*assigner)(void*, void*, double);
} Vector;

int vector_init(Vector* vec, int element_size, void (*assigner)(void*, void*)) {
	vec->arr = NULL;
	vec->len = 0;
	vec->__size = 1;
	vec->__element_size = element_size;
	vec->assigner = assigner;
	return 0;
}

//The assigner function is created separetely for each data type that you want to work this function
//The first void* parameter should accept a pointer to the element to be modified, and the
//second void* parameter should be the value the pointer to the element gets if it is a pointer to a struct
//or some sort of integer. If the value is a double or float, it should be passed in the 3rd parameter spot

//Below are a few examples of how the assigner function could work using just basic types in c
void assign_int(void* element, void* val, double val2) {
	*((int*)element) = val;
}

void assign_char(void* element, void* val, double val2) {
	*((char*)element) = val;
}

void assign_float(void* element, void* val, double val2) {
	*((float*)element) = val2;
}

void assign_double(void* element, void* val, double val2) {
	*((double*)element) = val2;
}

void assign_long(void* element, void* val, double val2) {
	*((long*)element) = val;
}

//val should be used to pass parameters in any case except for 
int vector_append(Vector* vec, void* val, double decimal_num) {
	if (vec->len + 1 >= vec->__size) {
		vec->__size *= 2;

		void* test = (void*)realloc(vec->arr, vec->__size * vec->__element_size);

		if (test == NULL) {
			printf("Failed to allocate memory in vector_append function\n");
			exit(-1);
		}

		vec->arr = test;
	}

	vec->assigner((void*)((long long)vec->arr + (vec->len * vec->__element_size)), val, decimal_num);
	vec->len++;
	return 0;
}

int vector_pop(Vector* vec) {
	if (vec->len - 1 <= vec->__size / 2) {
		vec->__size /= 2;

		//If the size of the list somehow ends up at 0, then it will be set back to one
		if (vec->__size <= 0) {
			vec->__size = 1;
		}

		void* test = (void*)realloc(vec->arr, vec->__size * vec->__element_size);

		if (test == NULL) {
			printf("Failed to allocate memory for vector_pop function\n");
			exit(-1);
		}

		vec->arr = test;
	}

	vec->len--;
	return 0;
}

//WARNING: If each element of the array contains a pointer in it, then it is up to you to first free that memory
//since vector_destroy only cleans up the memory for the elements it points to, but not the memory that might
//be pointed to by pointers in each individual element. An example where this would potentially come up is with
//lists of lists
int vector_destroy(Vector* vec) {
	free(vec->arr);
	vec->arr = NULL;
	vec->len = 0;
	vec->__size = 1;
	return 0;
}


#endif