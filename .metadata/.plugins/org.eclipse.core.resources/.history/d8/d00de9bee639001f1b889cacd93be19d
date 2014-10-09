/*
 * main1.c
 *
 *  Created on: 2010-03-27
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <util/delay.h>


// definicje dla preprocesora
#define LED_PIN (1<<PC7)			// definicja pinu do którego pod³¹czona jest dioda
#define LED_ON PORTC &= ~LED_PIN	// makrodefinicja – za³¹czenie diody
#define LED_OFF PORTC |= LED_PIN	// makrodefinicja – wy³¹czenie diody
#define LED_TOG PORTC ^= LED_PIN	// makrodefinicja – zmiana stanu diody

// ********************************************************* 1-sza wersja
int main(void)
{
	// ****** inicjalizacja *********
	DDRC |= LED_PIN;		// kierunek pinu PC7 – wyjœciowy

	// ****** pêtla g³ówna programu  *********
	while(1)
	{
		LED_ON;				// zapal diodê
		_delay_ms(1000);	// oczekiwanie 1s (1000ms)
		LED_OFF;			// zgaœ diodê
		_delay_ms(1000);	// oczekiwanie 1s
	}
}
