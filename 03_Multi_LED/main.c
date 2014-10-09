/*
 * main.c
 *
 *  Created on: 2010-03-30
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>		// do³¹czenie g³ównego systemowego  pliku nag³ówkowego
#include <avr/interrupt.h>
#include <util/delay.h>



#include "d_led.h"		// do³¹czenie naszego pliku nag³ówkowego (obs³uga LED)

int main(void)
{
	// ****** inicjalizacja *********
	d_led_init();   		// inicjalizacja wyœwietlacza multipleksowanego

	// testowa inicjalizacja zmiennych oraz liczb maj¹cych siê wyœwietlaæ na wyrw. LED
	cy1=NIC;
	cy2=4;
	cy3=1;
	cy4=NIC;

	sei();			// w³¹czenie globalnego zezwolenia na przerwania



	//	DDRA |= (1<<PA5);
	//	while(1)
	//	{
	//		PORTA ^= (1<<PA5);
	//		_delay_ms(1000);
	//	}

	uint16_t licznik=6000;
	uint8_t d1,d2,d3,d4;

		//**********************  pêtla g³ówna
		while(1)
		{
			licznik--;

			d1=licznik/1000;
			if(d1) cy1=d1; else cy1=NIC;
			d2=(licznik-(d1*1000))/100;
			if(d2) cy2=d2; else cy2=(licznik>999)?0:NIC;
			d3=(licznik-(d1*1000)-(d2*100))/10;
			if(d3) cy3=d3; else cy3=(licznik>99)?0:NIC;
			d4=(licznik-(d1*1000)-(d2*100)-(d3*10));
			cy4=d4;
			_delay_ms(10);
	//		if(licznik>9999) licznik=0;
			if(!licznik) licznik=6000;
		}
}

