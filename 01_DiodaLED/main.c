/*
 * main.c
 *
 *  Created on: 2010-03-27
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <util/delay.h>

// definicje dla preprocesora

// dioda LED
#define LED_PIN (1<<PC7)			// definicja pinu do którego pod³¹czona jest dioda
#define LED_TOG PORTC ^= LED_PIN	// makrodefinicja – zmiana stanu diody

// klawisz K1
#define KEY_PIN (1<<PC6)


// ********************************************************* 1-sza wersja
int main(void)
{
	// ****** inicjalizacja *********
	DDRC |= LED_PIN;		// kierunek pinu PC7 – wyjœciowy
	DDRC &= ~KEY_PIN;		// kierunek piny PC6 - wejœciowu
	PORTC |= KEY_PIN;       // podci¹gniêcie wejœcia do VCC

	// ****** pêtla g³ówna programu  *********
	while(1)
	{
		if( !(PINC & KEY_PIN) )
		{
			_delay_ms(80);
			if( !(PINC & KEY_PIN) )
			{
				LED_TOG;
			}
		}
	}
}

