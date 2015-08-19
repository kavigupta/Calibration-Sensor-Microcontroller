/*
 * list.h
 *
 *  Created on: Aug 13, 2015
 *      Author: root
 */

#ifndef LIST_H_
#define LIST_H_

#define list(type) __(list, type)

#define list_setup(list, type)\
	typedef struct {\
		int size, capacity;\
		type* values;\
	} list;

#define list_interface(list, type)\
		list* list_new_##type();\
		void list_set_##type(list* li, int loc, type el);\
		type list_get_##type(list* li, int loc);\
		void list_add_##type(list* li, type el);\
		void list_add_all_##type(list* li, list* to_add);\
		list* list_clone_##type(list* li);\
		void list_print_##type(list* li, char* (render)(type t));\
		void list_free_##type(list* li);\

#define list_impl(list, type)\
		list* list_new_##type(){\
			list* li = malloc(sizeof(list));\
			li->size = 0;\
			li->capacity = 16;\
			li->values = calloc(li->capacity, sizeof(type));\
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
				type* new_vals = realloc(li->values, li->capacity * sizeof(type));\
				if(!(new_vals)) free(li->values);\
				li->values = new_vals;\
			}\
			li->values[li->size] = el;\
			li->size++;\
		}\
		void list_add_all_##type(list* li, list* to_add){\
			int i;\
			for(i = 0; i<to_add->size; i++){\
				list_add_##type(li, to_add->values[i]);\
			}\
		}\
		list* list_clone_##type(list* li){\
			int i;\
			list* clone = list_new_##type();\
			for(i = 0; i<li->size; i++){\
				list_add_##type(clone, li->values[i]);\
			}\
			return clone;\
		}\
		void list_print_##type(list* li, char* (render)(type t)){\
			int i;\
			printf("[");\
			for(i = 0; i<li->size; i++){\
				char* rendered = render(li->values[i]);\
				printf("%s, ", rendered);\
				free(rendered);\
			}\
			printf("]\n");\
		}\
		void list_free_##type(list* li){\
			if(li->values) free(li->values);\
			if(li) free(li);\
		}\

#endif /* LIST_H_ */
