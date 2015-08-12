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

/**
 * Uses the given data set to set the THETA and G values of calibration.
 */
void cntrl_calibrate(JoinedDataSet data);
/**
 * Gets the theta matrix associated with the given magenetic field direction,
 * which is used by get_orientation and get_adjusted_accel
 */
Matrix cntrl_get_theta(Vector mag);
/**
 * Gets the orientation of the device, given the rotation matrix
 */
Vector cntrl_get_orientation(Matrix Theta);
/**
 * Subtracts out gravity from the given acceleration vector, yielding
 * the relative-to-ground acceleration of the device
 */
Vector cntrl_get_adjusted_accel(Matrix Theta, Vector unadjusted_accel);

#endif /* CONTROLLER_H_ */
