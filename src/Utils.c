/*
 * Utils.c
 *
 *  Created on: Aug 13, 2015
 *      Author: root
 */

#include <dirent.h>
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

char* render_double(double d) {
	char* c = malloc(40);
	c[0] = '\0';
	sprintf(c, "%f", d);
	return c;
}

int cmp_double(const void* _a, const void* _b) {
	const double* a = _a;
	const double* b = _b;
	if (*a > *b)
		return 1;
	if (*a < *b)
		return -1;
	return 0;
}

void foreach_in_dir(char* dir_string, void (apply_to_all)(char*)) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(dir_string)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name[0] == '.') {
				if (ent->d_name[1] == '\0')
					continue;
				if (ent->d_name[1] == '.' && ent->d_name[2] == '\0')
					continue;
			}
			char * file = malloc(strlen(dir_string) + strlen(ent->d_name) + 2);
			file[0] = '\0';
			strcat(file, dir_string);
			strcat(file, "/");
			strcat(file, ent->d_name);
			apply_to_all(file);
			free(file);
		}
		closedir(dir);
	} else {
		perror("");
		exit(-1);
	}

}
