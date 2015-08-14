/*
 * generics.h
 *
 *  Created on: Aug 13, 2015
 *      Author: root
 */

#ifndef GENERICS_H_
#define GENERICS_H_

#define __(generic, type) generic##__##type

#define import_header(generic, type)\
		generic##_setup(__(generic, type), type)\
		generic##_interface(__(generic, type), type)\

#define import_body(generic, type)\
		generic##_impl(__(generic, type), type)
#endif /* GENERICS_H_ */
