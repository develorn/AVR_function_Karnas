/*
 * main.c
 *
 *  Created on: 2010-09-13
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "LCD/lcd44780.h"

#include "1Wire/ds18x20.h"

void display_temp(uint8_t x);

uint8_t czujniki_cnt;		/* iloœæ czujników na magistrali */
volatile uint8_t s1_flag;	/* flaga tykniêcia timera co 1 sekundê */
volatile uint8_t sekundy;	/* licznik sekund 0-59 */

uint8_t subzero, cel, cel_fract_bits;

int main(void) {

	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7); /* podœwietlenie wyœwietlacza LCD */

	/* ustawienie TIMER0 dla F_CPU=11,0592MHz */
	TCCR0 |= (1<<WGM01);				/* tryb CTC */
	TCCR0 |= (1<<CS02)|(1<<CS00);		/* preskaler = 1024 */
	OCR0 = 108;							/* dodatkowy podzia³ przez 108 (rej. przepe³nienia) */
	TIMSK |= (1<<OCIE0);				/* zezwolenie na przerwanie CompareMatch */
	/* przerwanie wykonywane z czêstotliwoœci¹ ok 10ms (100 razy na sekundê) */
	/* do naszych celów nie musi byæ to bardzo dok³adne 10ms */

	lcd_init();	/* inicjalizacja LCD */

	/* sprawdzamy ile czujników DS18xxx widocznych jest na magistrali */
	czujniki_cnt = search_sensors();

	/* wysy³amy rozkaz wykonania pomiaru temperatury
	 * do wszystkich czujników na magistrali 1Wire
	 * zak³adaj¹c, ¿e zasilane s¹ w trybie NORMAL,
	 * gdyby by³ to tryb Parasite, nale¿a³oby u¿yæ
	 * jako pierwszego prarametru DS18X20_POWER_PARASITE */
	DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );

	/* czekamy 750ms na dokonanie konwersji przez pod³¹czone czujniki */
	_delay_ms(750);

	/* dokonujemy odczytu temperatury z pierwszego czujnika o ile zosta³ wykryty */
	/* wyœwietlamy temperaturê gdy czujnik wykryty */
	if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) display_temp(0);
	else {
		lcd_locate(1,0);
		lcd_str(" error ");	/* wyœwietlamy informacjê o b³êdzie jeœli np brak czujnika lub b³¹d odczytu */
	}

	/* dokonujemy odczytu temperatury z pierwszego czujnika o ile zosta³ wykryty */
	if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[1], &subzero, &cel, &cel_fract_bits) ) display_temp(9);
	else {
		lcd_locate(1,9);
		lcd_str(" error ");
	}

	sei();	/* w³¹czamy globalne przerwania */

	lcd_locate(0,0);
	lcd_str_P(PSTR("  T1       T2")); /* wyœwietl napisy w zerowej linii LCD */

	/* pêtla nieskoñczona */
	while(1) {

		if(s1_flag) {	/* sprawdzanie flagi tykniêæ timera programowego co 1 sekundê */

			/* co trzy sekundy gdy reszta z dzielenia modulo 3 == 0 sprawdzaj iloœæ dostêpnych czujników */
			if( 0 == (sekundy%3) ) czujniki_cnt = search_sensors();

			/* co trzy sekundy gdy reszta z dzielenia modulo 3 == 1 wysy³aj rozkaz pomiaru do czujników */
			if( 1 == (sekundy%3) ) DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );

			/* co trzy sekundy gdy reszta z dzielenia modulo 3 == 2 czyli jedn¹ sekundê po rozkazie konwersji
			 *  dokonuj odczytu i wyœwietlania temperatur z 2 czujników jeœli s¹ pod³¹czone, jeœli nie
			 *  to poka¿ komunikat o b³êdzie
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

			/* zerujemy flagê aby tylko jeden raz w ci¹gu sekundy wykonaæ operacje */
			s1_flag=0;
		} /* koniec sprawdzania flagi */
	} /* koniec pêtli nieskoñczonej */
}

/* wyœwietlanie temperatury na pozycji X w drugiej linii LCD */
void display_temp(uint8_t x) {
	lcd_locate(1,x);
	if(subzero) lcd_str("-");	/* jeœli subzero==1 wyœwietla znak minus (temp. ujemna) */
	else lcd_str(" ");	/* jeœli subzero==0 wyœwietl spacjê zamiast znaku minus (temp. dodatnia) */
	lcd_int(cel);	/* wyœwietl dziesiêtne czêœci temperatury  */
	lcd_str(".");	/* wyœwietl kropkê */
	lcd_int(cel_fract_bits); /* wyœwietl dziesiêtne czêœci stopnia */
	lcd_str(" C "); /* wyœwietl znak jednostek (C - stopnie Celsiusza) */
}


/* ================= PROCEDURA OBS£UGI PRZERWANIA – COMPARE MATCH */
/* pe³ni funkcjê timera programowego wyznaczaj¹cego podstawê czasu = 1s */
ISR(TIMER0_COMP_vect)
{
	static uint8_t cnt=0;	/* statyczna zmienna cnt do odliczania setnych ms */

	if(++cnt>99) {	/* gdy licznik ms > 99 (minê³a 1 sekunda) */
		s1_flag=1;	/* ustaw flagê tykniêcia sekundy */
		sekundy++;	/* zwiêksz licznik sekund */
		if(sekundy>59) sekundy=0; /* jeœli iloœæ sekund > 59 - wyzeruj */
		cnt=0;	/* wyzeru licznik setnych ms */
	}
}



