/*
 * Test.h
 *
 *  Created on: Aug 18, 2015
 *      Author: root
 */

#ifndef TEST_H_
#define TEST_H_

#include "Vector.h"

void test_matrix_mathematics();
void test_calibration_with_45dgup_data();
int test_approximate_equality_double(double a, double b);
int test_approximate_equality_vect(Vector a, Vector b);

#endif /* TEST_H_ */
