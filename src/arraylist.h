/*
 * list.h
 *
 *  Created on: Aug 13, 2015
 *      Author: root
 */

#ifndef ARRAYLIST_H_
#define ARRAYLIST_H_

#include "generics.h"

#define list(type) __(list, type)

#define list_setup(list, type)\
	typedef struct {\
		int size, capacity;\
		type* values;\
	} list;

#define list_interface(list, type)

#define list_impl(list, type)\
		list* list_new_##type(){\
			list* li = malloc(sizeof(list));\
			li->size = 0;\
			li->capacity = 16;\
			li->values = malloc(li->capacity * sizeof(type));\
			return li;\
		}\
		void list_set_##type(list* li, int loc, type el){\
			li->values[loc] = el;\
		}\
		type list_get_##type(list* li, int loc){\
			return li->values[loc];\
		}\
		void list_add_##type(list* li, type el){\
			if(li->size == li->capacity){\
				li->capacity *= 2;\
				li->values = realloc(li->values, li->capacity * sizeof(type));\
			}\
			li->values[li->size] = el;\
			li->size++;\
		}\
		void list_free_##type(list* li){\
			if(li->values) free(li->values);\
			if(li) free(li);\
		}\

#endif /* ARRAYLIST_H_ */
