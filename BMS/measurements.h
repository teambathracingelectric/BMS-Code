/*
 * measurements.h
 *
 *  Created on: 12 Apr 2018
 *      Author: emf3d
 */

#ifndef BMS_MEASUREMENTS_H_
#define BMS_MEASUREMENTS_H_

/*
 * Contain an int function that sets up the measurement routine in the FreeRTOS framework.
 * Measures:
 * 		-	Cell voltages and temperatures
 * 		-	Pack Current
 * 		-	Ambient Temperature
 * Loads data into a buffer intended to be accessed by a single function that can be process
 * 	and loaded into a database array.
 */

// Include files
#include "sys_common.h"

typedef enum BMS_MEASUREMENT_TYPE_RETURN
{
	PASS,
	FAIL,
	COMMS_ERROR
}BMS_MEASUREMENT_TYPE_RETURN_T;

// Start main functions
BMS_MEASUREMENT_TYPE_RETURN_T bms_measurements_ctor();
BMS_MEASUREMENT_TYPE_RETURN_T bms_measurements_dtor();

// Measurement functions
BMS_MEASUREMENT_TYPE_RETURN_T


#endif /* BMS_MEASUREMENTS_H_ */
