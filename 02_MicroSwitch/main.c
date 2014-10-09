/*
 * main.c
 *
 *  Created on: 2010-03-27
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <util/delay.h>

// *************   definicje dla preprocesora
// dioda LED
#define LED_PIN (1<<PC7)			// definicja pinu do którego pod³¹czona jest dioda
#define LED_TOG PORTC ^= LED_PIN	// makrodefinicja – zmiana stanu diody

// klawisz K1
#define KEY_PIN (1<<PC6)
#define KEY_DOWN !(PINC & KEY_PIN)

uint8_t klawisz_wcisniety(void);	// deklaracja funkcji

// *************************************************** pêtla g³ówna main()
int main(void)
{
	// ****** inicjalizacja *********
	DDRC |= LED_PIN;		// kierunek pinu PC7 – wyjœciowy
	PORTC |= LED_PIN;		// wy³¹czenie diody LED
	DDRC &= ~KEY_PIN;		// kierunek pinu PC6 - wejœciowy
	PORTC |= KEY_PIN;		// podci¹gniêcie pinu do VCC

	// ****** pêtla g³ówna programu  *********
	while(1)
	{
		if(  klawisz_wcisniety()  )  	// jeœli klawisz wciœniêty
		{
			 LED_TOG;		// zmieñ stan diody na przeciwny
			_delay_ms(200);	// pauza 200ms
		}
	}
}
//******************************************************** koniec main()

// definicja funkcji
uint8_t klawisz_wcisniety(void)
{
	if( KEY_DOWN )					// klawisz wciœniêty ?
	{
		_delay_ms(80);				// czas drgañ styków
		if( KEY_DOWN ) return 1;   	// jeœli wciœniêty?  zakoñcz funkcjê - rezultat = 1
	}

	return 0;	// jeœli nie wciœniêty klawisz, zakoñcz funkcjê, rezultat = 0
}

