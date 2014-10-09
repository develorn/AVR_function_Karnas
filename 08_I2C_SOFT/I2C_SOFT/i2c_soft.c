/*
 * i2c_twi.c
 *
 *  Created on: 2010-09-07
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <util/delay.h>


#include "i2c_soft.h"


void I2C_START (void) {
		I2C_SDL_LO;
		QDEL;
		I2C_SCL_LO;
}

void I2C_REP_START (void) {
	HDEL;
	I2C_SCL_HI;
	I2C_SDL_LO;
	QDEL;
	I2C_SCL_LO;
}

void I2C_STOP(void) {
	I2C_SDL_LO;
	HDEL;
	I2C_SCL_HI;
	QDEL;
	I2C_SDL_HI;
	HDEL;
}

u08 i2cPutbyte(u08 b) {
	u08 i=0x80;

	do {
		if ( b & i ) I2C_SDL_HI;
		else I2C_SDL_LO;

		I2C_SCL_TOGGLE;
	} while ( i >>= 1 );		// wysy³anie bajtu

	I2C_SDL_HI;			// pozostawienie SDA w stanie wysokim

	DDR(SDAPORT) &= ~(1<<SDA);
	HDEL;
	I2C_SCL_HI;
	b = PIN(SDAPORT) & (1<<SDA);	// sprawdzenie bitu ACK

	HDEL;
	I2C_SCL_LO;
	DDR(SDAPORT) |= (1<<SDA);
	HDEL;
	return (b == 0);	//zwrócenie ACK jako rezultatu funkcji
}


u08 i2cGetbyte(uint8_t ack) {
	u08 i;
	u08 c,b = 0;

	I2C_SDL_HI;
	DDR(SDAPORT) &= ~(1<<SDA);

	for(i=8;i>0;i--) {
		HDEL;
		I2C_SCL_HI;
		c = PIN(SDAPORT) & (1<<SDA);
		b <<= 1;
		if(c) b |= 1;
		HDEL;
		I2C_SCL_LO;
	}

	DDR(SDAPORT) |= (1<<SDA);

	if (ack) I2C_SDL_LO;		// wyœlij ACK
	else I2C_SDL_HI;		// wyœlij NAK

	I2C_SCL_TOGGLE;
	I2C_SDL_HI;
	return b;
}

// Inicjalizacja komunikacji I2C
void i2c_init(void) {
	DDR(SDAPORT) |= (1<<SDA);
	DDR(SCLPORT) |= (1<<SCL);
	I2C_SDL_HI;
	I2C_SCL_HI;
}

// Wys³anie sekwencji bajtów z bufora [buf] do urz¹dzenia [device] pod adres [adr]
void I2C_write_buf(u08 device, u08 adr, u08 len, u08 *buf) {

	I2C_START();      		// i2c START
	i2cPutbyte(device); 		// wys³anie adresu urz¹dzenia
	i2cPutbyte(adr);		// wys³anie adresu pamiêci do zapisu

	// wys³anie danych
	while (len--)
		i2cPutbyte(*buf++);

	I2C_STOP();			// i2c STOP
}

// Odebranie sekwencji bajtów do bufora [buf] z urz¹dzenia [deice] spod adresu [adr]
void I2C_read_buf(u08 device, u08 adr, u08 len, u08 *buf) {

	I2C_START();			// i2c START
	i2cPutbyte(device);		// wys³anie adresu urz¹dzenia
	i2cPutbyte(adr);   		// wys³anie adresu pamiêci do odczytu

	I2C_REP_START();		// repeated START

	// wys³anie adresu urz¹dzenia z ustawieniem bitu odczytu
	i2cPutbyte(device | 1);

	// odbieranie danych
	while (len--) *buf++ = i2cGetbyte(len ? 1 : 0);

	I2C_STOP();			// i2c STOP
}
