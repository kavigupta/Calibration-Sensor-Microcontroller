/*
 * Utils.c
 *
 *  Created on: Aug 13, 2015
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* utils_concat(char* a, char* b) {
	char* mem = malloc(strlen(a) + strlen(b) + 1);
	mem[0] = '\0';
	strcat(mem, a);
	strcat(mem, b);
	return mem;
}

int cmp_double(const void* _a, const  void* _b){
	const double* a = _a;
	const double* b = _b;
	if(*a > *b) return 1;
	if(*a < *b) return -1;
	return 0;
}
