/*
 * main.c
 *
 *  Created on: 2010-03-27
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>
#include <util/delay.h>

// definicje dla preprocesora

// dioda LED
#define LED_PIN (1<<PC7)			// definicja pinu do kt�rego pod��czona jest dioda
#define LED_TOG PORTC ^= LED_PIN	// makrodefinicja � zmiana stanu diody

// klawisz K1
#define KEY_PIN (1<<PC6)


// ********************************************************* 1-sza wersja
int main(void)
{
	// ****** inicjalizacja *********
	DDRC |= LED_PIN;		// kierunek pinu PC7 � wyj�ciowy
	DDRC &= ~KEY_PIN;		// kierunek piny PC6 - wej�ciowu
	PORTC |= KEY_PIN;       // podci�gni�cie wej�cia do VCC

	// ****** p�tla g��wna programu  *********
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

