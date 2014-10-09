/*
 * main.c
 *
 *  Created on: 2010-09-03
 *       Autor: Miros³aw Kardaœ
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

	sei();							// globalne odblokowanie przerwañ

	uint8_t licznik,  pm = licznik = OSCCAL-20;	// zmienne pomocnicze do wizualizacji OSCCAL

	// pêtla nieskoñczona
	while(1) {

		uart_puts("Test UART, wartosc OSCCAL = ");	// wyœlij tekst
		uart_putint(licznik, 10);	// wyœlij liczbê
		uart_putc('\r');			// wyœlij znak CR (enter)
		uart_putc('\n');			// wyœlij znak LF (nowa linia)
		_delay_ms(500);				// odczekaj 0,5 sekundy
		OSCCAL = licznik++;			// zwiêksz wartoœæ bajtu kalibracyjnego o 1

		// badamy kalibracjê tylko w granicach +- 20 w stosunku do
		// tej jaka by³a ustawiona fabrycznie
		if(licznik > pm+40) licznik=pm;

	}

}









