/*
 * temperatura.h
 *
 *  Created on: 2010-04-11
 *      Author: Miros³aw Kardaœ
 */

#include "1wire/ds18x20.h"

#ifndef TEMPERATURA_H_
#define TEMPERATURA_H_




typedef struct {
	uint8_t id[8];
	uint8_t subzero;
	uint8_t cel;
	uint8_t cel_frac_bits;
	uint8_t status;
	int wartosc;
} stemperatura;

extern stemperatura temperatura;







void init_t_sensor(void);
uint8_t slot_is_empty(void);

#endif /* TEMPERATURA_H_ */
