/*
 * main.c
 *
 *  Created on: 2010-09-03
 *       Autor: Miros�aw Karda�
 *
 *       ATmega32 - taktowana wewn. oscylatorem 8MHz
 *
 *		 Kalibracja OSCCAL
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



#include "MKUART/mkuart.h"



int main(void) {

	USART_Init( __UBRR );			// inicjalizacja UART

	sei();							// globalne odblokowanie przerwa�

	uint8_t licznik,  pm = licznik = OSCCAL-20;	// zmienne pomocnicze do wizualizacji OSCCAL

	// p�tla niesko�czona
	while(1) {

		uart_puts("Test UART, wartosc OSCCAL = ");	// wy�lij tekst
		uart_putint(licznik, 10);	// wy�lij liczb�
		uart_putc('\r');			// wy�lij znak CR (enter)
		uart_putc('\n');			// wy�lij znak LF (nowa linia)
		_delay_ms(500);				// odczekaj 0,5 sekundy
		OSCCAL = licznik++;			// zwi�ksz warto�� bajtu kalibracyjnego o 1

		// badamy kalibracj� tylko w granicach +- 20 w stosunku do
		// tej jaka by�a ustawiona fabrycznie
		if(licznik > pm+40) licznik=pm;

	}

}









