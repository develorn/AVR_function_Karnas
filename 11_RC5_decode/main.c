/*
 * main.c
 *
 *  Created on: 2010-09-13
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "LCD/lcd44780.h"
#include "IR_DECODE/ir_decode.h"


int main(void) {

	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7); /* pod�wietlenie wy�wietlacza LCD */

	lcd_init();	/* inicjalizacja LCD */
	ir_init();	/* inicjalizacja dekodowania IR */

	sei();	/* w��czamy globalne przerwania */

	lcd_locate(0,0);
	lcd_str_P(PSTR("Kody pilota RC5")); /* wy�wietl napis w zerowej linii LCD */

	/* p�tla niesko�czona */
	while(1) {

		if(Ir_key_press_flag) {	/* je�li odebrano prawid�owe kody z pilota */

			if( !address ) {		/* je�li adres == 0 */
				lcd_locate(1,0);
				lcd_str("key: ");
				lcd_int(command);	/* wy�wietl w drugiej linii LCD kod klawisza */
				lcd_str("   ");
			}

			/* je�li wci�ni�ty konkretny przycik vol_up */
			if( !address && vol_up==command ) {
				lcd_locate(1,10);
				lcd_str("vol_up");	/* wy�wietl jego nazw� */
			} else {
				lcd_locate(1,10);
				lcd_str("      ");	/* je�li inny skasuj pole */
			}

			/* wyzerowanie flagi odbioru oraz warto�ci odebranych kod�w */
			Ir_key_press_flag=0;
			command=0xff;
			address=0xff;
		}

	}
}
