/*
 * main.c
 *
 *  Created on: 2010-03-27
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>
#include <util/delay.h>

// *************   definicje dla preprocesora
// dioda LED
#define LED_PIN (1<<PC7)			// definicja pinu do kt�rego pod��czona jest dioda
#define LED_TOG PORTC ^= LED_PIN	// makrodefinicja � zmiana stanu diody

// klawisz K1
#define KEY_PIN (1<<PC6)
#define KEY_DOWN !(PINC & KEY_PIN)

uint8_t klawisz_wcisniety(void);	// deklaracja funkcji

// *************************************************** p�tla g��wna main()
int main(void)
{
	// ****** inicjalizacja *********
	DDRC |= LED_PIN;		// kierunek pinu PC7 � wyj�ciowy
	PORTC |= LED_PIN;		// wy��czenie diody LED
	DDRC &= ~KEY_PIN;		// kierunek pinu PC6 - wej�ciowy
	PORTC |= KEY_PIN;		// podci�gni�cie pinu do VCC

	// ****** p�tla g��wna programu  *********
	while(1)
	{
		if(  klawisz_wcisniety()  )  	// je�li klawisz wci�ni�ty
		{
			 LED_TOG;		// zmie� stan diody na przeciwny
			_delay_ms(200);	// pauza 200ms
		}
	}
}
//******************************************************** koniec main()

// definicja funkcji
uint8_t klawisz_wcisniety(void)
{
	if( KEY_DOWN )					// klawisz wci�ni�ty ?
	{
		_delay_ms(80);				// czas drga� styk�w
		if( KEY_DOWN ) return 1;   	// je�li wci�ni�ty?  zako�cz funkcj� - rezultat = 1
	}

	return 0;	// je�li nie wci�ni�ty klawisz, zako�cz funkcj�, rezultat = 0
}

