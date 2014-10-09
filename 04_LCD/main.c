/*
 * main.c
 *
 *  Created on: 2010-03-31
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "LCD/lcd44780.h"

// poni¿sz¹ linijkê czasami trzeba wpisaæ w eclipse przed definicjami
// zmiennych w pamiêci EEPROM, ¿eby nie podkreœla³ sk³adni jako b³êdnej
#define EEMEM __attribute__((section(".eeprom")))

char PROGMEM tab1[] = {"FLASH"};
char EEMEM tab2[] = {"EEPROM"};

uint8_t znak_L[] = {16,16,18,20,24,16,31,0};		// wzór znaku litery £ w pamiêci RAM
uint8_t znak_o[] EEMEM = {4,32,14,17,17,17,14,0};	// wzór znaku litery ó w pamiêci EEPROM
uint8_t znak_buzka[] PROGMEM = {14,17,27,17,17,21,17,14};	// wzór znaku buŸki w pamiêci FLASH
uint8_t znak_termo[] PROGMEM = {4,10,10,10,17,31,31,14};	// wzór znaku termometru w pamiêci FLASH

volatile uint8_t pwm1, pwm2, pwm3;

int main(void)
{
	DDRD |= (1<<PD0);
	PORTD |= (1<<PD0);

	lcd_init();

	lcd_str_P(tab1);				// napis z pamiêci FLASH
	lcd_locate(0,10);
	lcd_str_P( PSTR("Linia1") );	// napis z pamiêci FLASH
	lcd_locate(1,0);
	lcd_str_E(tab2);				// napis z pamiêci EEPROM
	lcd_locate(1,10);
	lcd_str("Linia2");				// napis z pamiêci RAM

	// za³adowanie znaków do pamiêci CGRAM
	lcd_defchar(0x80, znak_L);
	lcd_defchar_E(0x81, znak_o);
	lcd_defchar_P(0x82, znak_buzka);
	lcd_defchar_P(0x83, znak_termo);


	// wyœwietlenie w³asnych znaków na LCD
	lcd_locate(0,7);
	lcd_str("\x80\x81");
	lcd_locate(1,8);
	lcd_str("\x82");
	lcd_locate(1,7);
	lcd_str("\x83");

	while(1);
}

