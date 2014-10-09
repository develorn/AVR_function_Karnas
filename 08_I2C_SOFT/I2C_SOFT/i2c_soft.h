/*
 * i2c_twi.h
 *
 *  Created on: 2010-09-07
 *       Autor: Miros�aw Karda�
 */

#ifndef I2C_SOFT_H_
#define I2C_SOFT_H_

#define ACK 1
#define NACK 0

#include "i2c_soft_cfg.h"

// Makra upraszczaj�ce dost�p do port�w
// *** PORT
#define PORT(x) XPORT(x)
#define XPORT(x) (PORT##x)
// *** PIN
#define PIN(x) XPIN(x)
#define XPIN(x) (PIN##x)
// *** DDR
#define DDR(x) XDDR(x)
#define XDDR(x) (DDR##x)

// utworzenie typu u08, odpowiednika uint8_t aby kod by� bardziej czytelny
typedef unsigned char  u08;
typedef unsigned short u16;

// i2c op�nienie �wier� bitu
#define QDEL	_delay_loop_1(1)
// i2c op�nienie p� bitu
#define HDEL	_delay_loop_1(2)

#define I2C_SDL_LO      PORT(SDAPORT) &= ~(1<<SDA)
#define I2C_SDL_HI      PORT(SDAPORT) |= (1<<SDA)

#define I2C_SCL_LO      PORT(SCLPORT) &= ~(1<<SCL)
#define I2C_SCL_HI      PORT(SCLPORT) |= (1<<SCL)

#define I2C_SCL_TOGGLE  HDEL; I2C_SCL_HI; HDEL; I2C_SCL_LO


// funkcje

void I2C_START (void);		// i2c START
void I2C_REP_START (void);	// i2c REPEATED START
void I2C_STOP(void);		// i2c STOP

// inicjalizacja pin�w I2C
void i2c_init(void);

// i2c - wys�anie pojedynczego bajtu
u08 i2cPutbyte(u08 b);

// i2c - odebranie pojedynczego bajtu z ptwierdzeniem ack=1 lub bez ack=0
u08 i2cGetbyte(uint8_t ack);

// I2C - wysy�a dane <buf> do urz�dzenia <device> pod adres <adr>
void I2C_write_buf(u08 device, u08 adr, u08 len, u08 *buf);

// I2C - odbiera dane <buf> z urz�dzenia <device> spod adresu <adr>
void I2C_read_buf(u08 device, u08 adr, u08 len, u08 *buf);


#endif /* I2C_TWI_H_ */
