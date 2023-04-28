#ifndef RA_HS3001_H_
#define RA_HS3001_H_
#include "../sensorsData.h"
/*
 * RX_HS3001.h
 *
 *  Created on: 21-Feb-2022
 *      Author: 27779
 */

/* See Developer Assistance in the project */
void g_comms_i2c_bus0_quick_setup(void);
void hs3001_get();
void g_hs300x_sensor0_quick_setup(void);
void getHS3001Data(st_sensorsHS_t *hsData);


/* Utility struct for sensor data*/
typedef struct
{
	float       gs_demo_humidity; /* Global variable to hold humidty reading*/
	float       gs_demo_temperature; /* Global variable to hold temperature reading*/
}sensor_demo_data_t;

#endif
