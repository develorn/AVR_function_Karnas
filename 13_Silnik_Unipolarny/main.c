/*
 * main.c
 *
 *  Created on: 2010-09-14
 *       Autor: Miros³aw Kardaœ
 *
 *       F_CPU = 11,0592MHz
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* definicje pinów steruj¹cych tranzystorami w ULN2803 */
#define T1 (1<<PD0)
#define T2 (1<<PD1)
#define T3 (1<<PD2)
#define T4 (1<<PD3)


/* definicje kroków steruj¹cych prac¹ silnika */
#define KROK1 PORTD |= T1; PORTD &= ~(T2|T3|T4)
#define KROK2 PORTD |= T2; PORTD &= ~(T1|T3|T4)
#define KROK3 PORTD |= T3; PORTD &= ~(T1|T2|T4)
#define KROK4 PORTD |= T4; PORTD &= ~(T1|T2|T3)


void kroki_lewo(void);
void kroki_prawo(void);



volatile uint8_t s1_flag;	/* flaga tykniêcia timera co 1 sekundê */
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
	OCR0 = 54;							/* dodatkowy podzia³ przez 54 (rej. przepe³nienia) */
	TIMSK |= (1<<OCIE0);				/* zezwolenie na przerwanie CompareMatch */
	/* przerwanie wykonywane z czêstotliwoœci¹ ok 5ms (20 razy na sekundê) */
	/* do naszych celów nie musi byæ to bardzo dok³adne 5ms */

	sei();	/* w³¹czamy globalne przerwania */

	while(1) {

		if(ms5_flag) { 	/* sprawdzamy flagê, rozdzielczoœæ 5ms */

			/* jeœli zmienna dir==lewo (0) - obracaj w lewo */
			if(lewo==dir) kroki_lewo();
			/* jeœli zmienna dir==prawo (1) - obracaj w prawo */
			else kroki_prawo();

			ms5_flag = 0;	/* zerujemy flagê */
		}

		if(s1_flag) {	/* sprawdzamy flagê, rozdzielczoœæ 1s */

			/* równo co dwie sekundy zmieniamy kierunek na przeciwny */
			if( !(sekundy%2) ) dir ^= 1;

			s1_flag = 0;	/* zerujemy flagê */
		}

	}
}

/* funkcja wykonuj¹ca cyklicznie kroki (obrót w lewo) */
void kroki_lewo(void) {
	static uint8_t kr;

	if( kr == 0 ) { KROK1; }
	if( kr == 1 ) { KROK2; }
	if( kr == 2 ) { KROK3; }
	if( kr == 3 ) { KROK4; }

	if( ++kr > 3 ) kr=0;
}

/* funkcja wykonuj¹ca cyklicznie kroki (obrót w prawo) */
void kroki_prawo(void) {
	static uint8_t kr;

	if( kr == 0 ) { KROK4; }
	if( kr == 1 ) { KROK3; }
	if( kr == 2 ) { KROK2; }
	if( kr == 3 ) { KROK1; }

	if( ++kr > 3 ) kr=0;
}



/* ================= PROCEDURA OBS£UGI PRZERWANIA – COMPARE MATCH */
/* pe³ni funkcjê timera programowego wyznaczaj¹cego podstawê czasu = 5ms oraz 1s */
ISR(TIMER0_COMP_vect)
{
	ms5_flag = 1;	/* ustawiamy flagê co 5ms */

	if(++ms5_cnt>199) {	/* gdy licznik ms > 199 (minê³a 1 sekunda) */
		s1_flag=1;	/* ustaw flagê tykniêcia sekundy */
		sekundy++;	/* zwiêksz licznik sekund */
		if(sekundy>59) sekundy=0; /* jeœli iloœæ sekund > 59 - wyzeruj */
		ms5_cnt=0;	/* wyzeru licznik setnych ms */
	}
}
