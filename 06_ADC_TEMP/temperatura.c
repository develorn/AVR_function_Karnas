/*
 * temperatura.c
 *
 *  Created on: 2010-04-11
 *      Author: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <string.h>

#include "temperatura.h"



stemperatura temperatura;

uint8_t nSensors;




void copy_sensor_to_slot(uint8_t sid_idx, uint8_t slot_idx)
{
	memcpy(&temperatura.id[0], &gSensorIDs[sid_idx][0], 8);
}

uint8_t slot_is_empty(void)
{
	register uint8_t i;
	for(i=0;i<8;i++)
	{
		if ( !(temperatura.id[i] == 0x00 || temperatura.id[i] == 0xFF) ) return 0;
	}
	return 1;
}

void init_t_sensor(void)
{
	nSensors = search_sensors();
	copy_sensor_to_slot(0,0);
}

