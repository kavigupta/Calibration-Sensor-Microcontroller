/*
 * Utils.c
 *
 *  Created on: Aug 13, 2015
 *      Author: root
 */

#include <stdlib.h>
#include <string.h>

char* utils_concat(char* a, char* b) {
	char* mem = malloc(strlen(a) + strlen(b) + 1);
	mem[0] = '\0';
	strcat(mem, a);
	strcat(mem, b);
	return mem;
}
