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

#include "1Wire/ds18x20.h"

void display_temp(uint8_t x);

uint8_t czujniki_cnt;		/* ilo�� czujnik�w na magistrali */
volatile uint8_t s1_flag;	/* flaga tykni�cia timera co 1 sekund� */
volatile uint8_t sekundy;	/* licznik sekund 0-59 */

uint8_t subzero, cel, cel_fract_bits;

int main(void) {

	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7); /* pod�wietlenie wy�wietlacza LCD */

	/* ustawienie TIMER0 dla F_CPU=11,0592MHz */
	TCCR0 |= (1<<WGM01);				/* tryb CTC */
	TCCR0 |= (1<<CS02)|(1<<CS00);		/* preskaler = 1024 */
	OCR0 = 108;							/* dodatkowy podzia� przez 108 (rej. przepe�nienia) */
	TIMSK |= (1<<OCIE0);				/* zezwolenie na przerwanie CompareMatch */
	/* przerwanie wykonywane z cz�stotliwo�ci� ok 10ms (100 razy na sekund�) */
	/* do naszych cel�w nie musi by� to bardzo dok�adne 10ms */

	lcd_init();	/* inicjalizacja LCD */

	/* sprawdzamy ile czujnik�w DS18xxx widocznych jest na magistrali */
	czujniki_cnt = search_sensors();

	/* wysy�amy rozkaz wykonania pomiaru temperatury
	 * do wszystkich czujnik�w na magistrali 1Wire
	 * zak�adaj�c, �e zasilane s� w trybie NORMAL,
	 * gdyby by� to tryb Parasite, nale�a�oby u�y�
	 * jako pierwszego prarametru DS18X20_POWER_PARASITE */
	DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );

	/* czekamy 750ms na dokonanie konwersji przez pod��czone czujniki */
	_delay_ms(750);

	/* dokonujemy odczytu temperatury z pierwszego czujnika o ile zosta� wykryty */
	/* wy�wietlamy temperatur� gdy czujnik wykryty */
	if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) display_temp(0);
	else {
		lcd_locate(1,0);
		lcd_str(" error ");	/* wy�wietlamy informacj� o b��dzie je�li np brak czujnika lub b��d odczytu */
	}

	/* dokonujemy odczytu temperatury z pierwszego czujnika o ile zosta� wykryty */
	if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[1], &subzero, &cel, &cel_fract_bits) ) display_temp(9);
	else {
		lcd_locate(1,9);
		lcd_str(" error ");
	}

	sei();	/* w��czamy globalne przerwania */

	lcd_locate(0,0);
	lcd_str_P(PSTR("  T1       T2")); /* wy�wietl napisy w zerowej linii LCD */

	/* p�tla niesko�czona */
	while(1) {

		if(s1_flag) {	/* sprawdzanie flagi tykni�� timera programowego co 1 sekund� */

			/* co trzy sekundy gdy reszta z dzielenia modulo 3 == 0 sprawdzaj ilo�� dost�pnych czujnik�w */
			if( 0 == (sekundy%3) ) czujniki_cnt = search_sensors();

			/* co trzy sekundy gdy reszta z dzielenia modulo 3 == 1 wysy�aj rozkaz pomiaru do czujnik�w */
			if( 1 == (sekundy%3) ) DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );

			/* co trzy sekundy gdy reszta z dzielenia modulo 3 == 2 czyli jedn� sekund� po rozkazie konwersji
			 *  dokonuj odczytu i wy�wietlania temperatur z 2 czujnik�w je�li s� pod��czone, je�li nie
			 *  to poka� komunikat o b��dzie
			 */
			if( 2 == (sekundy%3) ) {
				if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) display_temp(0);
				else {
					lcd_locate(1,0);
					lcd_str(" error ");
				}

				if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[1], &subzero, &cel, &cel_fract_bits) ) display_temp(9);
				else {
					lcd_locate(1,9);
					lcd_str(" error ");
				}
			}

			/* zerujemy flag� aby tylko jeden raz w ci�gu sekundy wykona� operacje */
			s1_flag=0;
		} /* koniec sprawdzania flagi */
	} /* koniec p�tli niesko�czonej */
}

/* wy�wietlanie temperatury na pozycji X w drugiej linii LCD */
void display_temp(uint8_t x) {
	lcd_locate(1,x);
	if(subzero) lcd_str("-");	/* je�li subzero==1 wy�wietla znak minus (temp. ujemna) */
	else lcd_str(" ");	/* je�li subzero==0 wy�wietl spacj� zamiast znaku minus (temp. dodatnia) */
	lcd_int(cel);	/* wy�wietl dziesi�tne cz�ci temperatury  */
	lcd_str(".");	/* wy�wietl kropk� */
	lcd_int(cel_fract_bits); /* wy�wietl dziesi�tne cz�ci stopnia */
	lcd_str(" C "); /* wy�wietl znak jednostek (C - stopnie Celsiusza) */
}


/* ================= PROCEDURA OBS�UGI PRZERWANIA � COMPARE MATCH */
/* pe�ni funkcj� timera programowego wyznaczaj�cego podstaw� czasu = 1s */
ISR(TIMER0_COMP_vect)
{
	static uint8_t cnt=0;	/* statyczna zmienna cnt do odliczania setnych ms */

	if(++cnt>99) {	/* gdy licznik ms > 99 (min�a 1 sekunda) */
		s1_flag=1;	/* ustaw flag� tykni�cia sekundy */
		sekundy++;	/* zwi�ksz licznik sekund */
		if(sekundy>59) sekundy=0; /* je�li ilo�� sekund > 59 - wyzeruj */
		cnt=0;	/* wyzeru licznik setnych ms */
	}
}



