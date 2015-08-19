/*
 * Utils.h
 *
 *  Created on: Aug 13, 2015
 *      Author: root
 */

#ifndef UTILS_H_
#define UTILS_H_

char* utils_concat(char* a, char* b);
int cmp_double(const void* a, const void* b);
char* render_double(double d);
/**
 * Applies the given function to every path in the given directory
 */
void foreach_in_dir(char* dir_string, void (apply_to_all)(char*));

#endif /* UTILS_H_ */
