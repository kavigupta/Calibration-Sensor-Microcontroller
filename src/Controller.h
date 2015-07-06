/*
 * Magnetometer.h
 *
 *  Created on: Jul 2, 2015
 *      Author: root
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "IO.h"
#include "Matrix.h"
#include "Vector.h"

extern Matrix THETA_CALIBRATION;
Matrix THETA_CALIBRATION;
extern Vector G_CALIBRATION;
Vector G_CALIBRATION;

void calibrate(DataSet data);
Matrix Theta(Vector mag);
Vector orientation(Matrix Theta);
Vector normalized_accel(Matrix Theta, Vector unnormalized_accel);

#endif /* CONTROLLER_H_ */
