/*
 * main.c
 *
 *  Created on: 2010-09-07
 *       Autor: Miros�aw Karda�
 *
 *       Program korzysta ze programowej obs�ugi I2C
 *       Umo�liwia zapis danych do RTC oraz EEPROM a tak�e
 *       oczyt z tych urz�dze�
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "LCD/lcd44780.h"
#include "I2C_SOFT/i2c_soft.h"

#define PCF8583_ADDR 0xA2

#define _24C04_ADDR 0xA8



uint8_t tekst[] = "EEPROM";

uint8_t bo[ sizeof(tekst)+1 ]; 	// bufor pomocniczy na odczyt z EEPROM

volatile uint8_t int0_flag=1;	// flaga zmieniana w przerwaniu i sprawdzana w p�tli g��wnej

// konwersja liczby dziesi�tnej na BCD
uint8_t dec2bcd(uint8_t dec);
// konwersja liczby BCD na dziesi�tn�
uint8_t bcd2dec(uint8_t bcd);

// odczyt danych z pami�ci EEPROM
void EI2C_read_buf(u08 device, u16 subAddr, u16 len, u08 *buf);
// zapis danych do pami�ci EEPROM
void EI2C_write_buf(u08 device, u16 subAddr, u16 len, u08 *buf);

int main(void) {

	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7); // pod�wietlenie wy�wietlacza LCD

	// Przerwanie INT0
	MCUCR |= (1<<ISC01);	// wyzwalanie zboczem opadaj�cym
	GICR |= (1<<INT0);		// odblokowanie przerwania
	PORTD |= (1<<PD2);		// podci�gni�cie pinu INT0 do VCC


	// definiujemy sobie dla polepszenia czytelno�ci programu typ wyliczeniowy
	// wskazuj�cy nam p�niej na odpowiednie indeksy w tablicy (buforze)
	enum {ss=1, mm, hh};
	uint8_t bufor[4];		// rezerwacja bufora 4 bajty
	uint8_t sekundy, minuty, godziny;


	i2c_init();
	lcd_init();

	sei();

	lcd_str_P(PSTR("start..."));

	// Ustawianie czasu na godzin�: 18:34:27
	/*
	bufor[0] = 0;			// setne cz�ci sekundy
	bufor[1] = dec2bcd(27);	// sekundy
	bufor[2] = dec2bcd(34);	// minuty
	bufor[3] = dec2bcd(18);	// godziny
	// zapis 4 bajt�w z bufora pod adres 0x01 w pami�ci RAM naszego RTC
	I2C_write_buf( PCF8583_ADDR, 0x01, 4, bufor );
	*/

	// zapis tekstu do pami�ci EEPROM od adresu 253, dlatego aby tekst
	// zosta� zapisany w jednym i drugim banku pami�ci
	EI2C_write_buf( _24C04_ADDR, 253, sizeof(tekst), tekst );

	while(1) {

		if ( int0_flag ) {
			//odczyt 4 bajt�w do bufora od adresu 0x01 z pami�ci RAM naszego RTC
			I2C_read_buf( PCF8583_ADDR, 0x01, 4, bufor );

			sekundy = bcd2dec( bufor[ss] );
			minuty = bcd2dec( bufor[mm] );
			godziny = bcd2dec( bufor[hh] );

			// wy�wietlenie czasu na LCD
			lcd_locate(1,0);
			if( godziny < 10 ) lcd_str("0");
			lcd_int(godziny);
			lcd_str(":");
			if( minuty < 10 ) lcd_str("0");
			lcd_int(minuty);
			lcd_str(":");
			if( sekundy < 10 ) lcd_str("0");
			lcd_int(sekundy);

			// odczyt z EEPROM
			EI2C_read_buf( _24C04_ADDR, 253, sizeof(tekst), bo );

			// wy�wietlenie napisu z EEPROM na LCD
			lcd_locate(0, 9);
			lcd_str( (char*)bo );

			int0_flag=0;
		}

	}
}


// procedura obs�ugi przerwania INT 0
ISR( INT0_vect ) {
	int0_flag = 1;
}


// konwersja liczby dziesi�tnej na BCD
uint8_t dec2bcd(uint8_t dec) {
return ((dec / 10)<<4) | (dec % 10);
}

// konwersja liczby BCD na dziesi�tn�
uint8_t bcd2dec(uint8_t bcd) {
    return ((((bcd) >> 4) & 0x0F) * 10) + ((bcd) & 0x0F);
}


// odczyt danych z pami�ci EEPROM
void EI2C_read_buf(u08 device, u16 subAddr, u16 len, u08 *buf) {

	while (len--) {
		I2C_START();
		i2cPutbyte(device | ((subAddr>>8)<<1) );
		i2cPutbyte(subAddr);
		I2C_REP_START();
		i2cPutbyte(device + 1);
		*buf++ = i2cGetbyte( NACK );
		I2C_STOP();
		subAddr++;
	}


}

// zapis danych do pami�ci EEPROM
void EI2C_write_buf(u08 device, u16 subAddr, u16 len, u08 *buf) {

	while (len--) {
		I2C_START();
		// ustawienie 9 bitu adresu pami�ci EEPROM w ramach
		// sprz�towego adresu urz�dzenia na pozycji bitu 1 (nr.2)
		i2cPutbyte( device | ((subAddr>>8)<<1) );
		i2cPutbyte(subAddr);

		i2cPutbyte(*buf++);

		I2C_STOP();
		_delay_ms(5); // oczekiwanie na zapis
		subAddr++;
	}
}

