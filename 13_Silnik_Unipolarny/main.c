/*
 * main.c
 *
 *  Created on: 2010-09-14
 *       Autor: Miros�aw Karda�
 *
 *       F_CPU = 11,0592MHz
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* definicje pin�w steruj�cych tranzystorami w ULN2803 */
#define T1 (1<<PD0)
#define T2 (1<<PD1)
#define T3 (1<<PD2)
#define T4 (1<<PD3)


/* definicje krok�w steruj�cych prac� silnika */
#define KROK1 PORTD |= T1; PORTD &= ~(T2|T3|T4)
#define KROK2 PORTD |= T2; PORTD &= ~(T1|T3|T4)
#define KROK3 PORTD |= T3; PORTD &= ~(T1|T2|T4)
#define KROK4 PORTD |= T4; PORTD &= ~(T1|T2|T3)


void kroki_lewo(void);
void kroki_prawo(void);



volatile uint8_t s1_flag;	/* flaga tykni�cia timera co 1 sekund� */
volatile uint8_t sekundy;	/* licznik sekund 0-59 */

volatile uint8_t ms5_flag;
volatile uint8_t ms5_cnt;

enum edir {lewo, prawo};
enum edir dir = lewo;


int main(void) {

	DDRD |= T1 | T2 | T3 | T4;
	PORTD &= !(T1|T2|T3|T4);

	/* ustawienie TIMER0 dla F_CPU=11,0592MHz */
	TCCR0 |= (1<<WGM01);				/* tryb CTC */
	TCCR0 |= (1<<CS02)|(1<<CS00);		/* preskaler = 1024 */
	OCR0 = 54;							/* dodatkowy podzia� przez 54 (rej. przepe�nienia) */
	TIMSK |= (1<<OCIE0);				/* zezwolenie na przerwanie CompareMatch */
	/* przerwanie wykonywane z cz�stotliwo�ci� ok 5ms (20 razy na sekund�) */
	/* do naszych cel�w nie musi by� to bardzo dok�adne 5ms */

	sei();	/* w��czamy globalne przerwania */

	while(1) {

		if(ms5_flag) { 	/* sprawdzamy flag�, rozdzielczo�� 5ms */

			/* je�li zmienna dir==lewo (0) - obracaj w lewo */
			if(lewo==dir) kroki_lewo();
			/* je�li zmienna dir==prawo (1) - obracaj w prawo */
			else kroki_prawo();

			ms5_flag = 0;	/* zerujemy flag� */
		}

		if(s1_flag) {	/* sprawdzamy flag�, rozdzielczo�� 1s */

			/* r�wno co dwie sekundy zmieniamy kierunek na przeciwny */
			if( !(sekundy%2) ) dir ^= 1;

			s1_flag = 0;	/* zerujemy flag� */
		}

	}
}

/* funkcja wykonuj�ca cyklicznie kroki (obr�t w lewo) */
void kroki_lewo(void) {
	static uint8_t kr;

	if( kr == 0 ) { KROK1; }
	if( kr == 1 ) { KROK2; }
	if( kr == 2 ) { KROK3; }
	if( kr == 3 ) { KROK4; }

	if( ++kr > 3 ) kr=0;
}

/* funkcja wykonuj�ca cyklicznie kroki (obr�t w prawo) */
void kroki_prawo(void) {
	static uint8_t kr;

	if( kr == 0 ) { KROK4; }
	if( kr == 1 ) { KROK3; }
	if( kr == 2 ) { KROK2; }
	if( kr == 3 ) { KROK1; }

	if( ++kr > 3 ) kr=0;
}



/* ================= PROCEDURA OBS�UGI PRZERWANIA � COMPARE MATCH */
/* pe�ni funkcj� timera programowego wyznaczaj�cego podstaw� czasu = 5ms oraz 1s */
ISR(TIMER0_COMP_vect)
{
	ms5_flag = 1;	/* ustawiamy flag� co 5ms */

	if(++ms5_cnt>199) {	/* gdy licznik ms > 199 (min�a 1 sekunda) */
		s1_flag=1;	/* ustaw flag� tykni�cia sekundy */
		sekundy++;	/* zwi�ksz licznik sekund */
		if(sekundy>59) sekundy=0; /* je�li ilo�� sekund > 59 - wyzeruj */
		ms5_cnt=0;	/* wyzeru licznik setnych ms */
	}
}
