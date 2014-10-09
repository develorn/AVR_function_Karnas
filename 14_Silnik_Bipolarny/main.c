/*
 * main.c
 *
 *  Created on: 2010-09-14
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* przydatne definicje pinów steruj¹cych */
#define B1 (1<<PD0)
#define A1 (1<<PD1)
#define A2 (1<<PD2)
#define B2 (1<<PD3)

/* definicje kroków steruj¹cych prac¹ silnika */
#define KROK1 PORTD |= A1|B1; PORTD &= ~(A2|B2)
#define KROK2 PORTD |= A2|B1; PORTD &= ~(A1|B2)
#define KROK3 PORTD |= A2|B2; PORTD &= ~(A1|B1)
#define KROK4 PORTD |= A1|B2; PORTD &= ~(A2|B1)

static void silnik_stop(void);
static void kroki_lewo(void);
static void kroki_prawo(void);


volatile uint8_t s1_flag;	/* flaga tykniêcia timera co 1 sekundê */
volatile uint8_t sekundy;	/* licznik sekund 0-59 */

volatile uint8_t ms2_flag;
volatile uint16_t ms2_cnt;

enum edir {lewo, prawo, stop};
enum edir dir = lewo;



int main(void) {

	/* ustawiamy piny steruj¹ce L293D jako wyjœcia */
	DDRD |= A1|A2|B1|B2;

	silnik_stop();


	/* ustawienie TIMER0 dla F_CPU=11,0592MHz */
	TCCR0 |= (1<<WGM01);				/* tryb CTC */
	TCCR0 |= (1<<CS02)|(1<<CS00);		/* preskaler = 1024 */
	OCR0 = 22;							/* dodatkowy podzia³ przez 22 (rej. przepe³nienia) */
	TIMSK |= (1<<OCIE0);				/* zezwolenie na przerwanie CompareMatch */
	/* przerwanie wykonywane z czêstotliwoœci¹ ok 2ms (500 razy na sekundê) */
	/* do naszych celów nie musi byæ to bardzo dok³adne 2ms */

	sei();	/* w³¹czamy globalne przerwania */

	while(1) {

		if(ms2_flag) { 	/* sprawdzamy flagê, rozdzielczoœæ 2ms */

			/* jeœli zmienna dir==lewo (0) - obracaj w lewo */
			if(lewo==dir) kroki_lewo();
			/* jeœli zmienna dir==prawo (1) - obracaj w prawo */
			if(prawo==dir) kroki_prawo();

			ms2_flag = 0;	/* zerujemy flagê */
		}

		if(s1_flag) {	/* sprawdzamy flagê, rozdzielczoœæ 1s */

			/* równo co dwie sekundy zmieniamy kierunek na przeciwny */
			if( 0 == (sekundy%9) ) dir = lewo;
			if( 3 == (sekundy%9) ) dir = prawo;
			if( 6 == (sekundy%9) ) {
				dir = stop;
				silnik_stop();
			}


			s1_flag = 0;	/* zerujemy flagê */
		}

	}
}

static void silnik_stop(void) {
	PORTD &= ~(A1|A2|B1|B2);
}

/* funkcja wykonuj¹ca cyklicznie kroki (obrót w lewo) */
static void kroki_lewo(void) {
	static uint8_t kr;

	if( kr == 0 ) { KROK1; }
	if( kr == 1 ) { KROK2; }
	if( kr == 2 ) { KROK3; }
	if( kr == 3 ) { KROK4; }

	if( ++kr > 3 ) kr=0;
}

/* funkcja wykonuj¹ca cyklicznie kroki (obrót w prawo) */
static void kroki_prawo(void) {
	static uint8_t kr;

	if( kr == 0 ) { KROK4; }
	if( kr == 1 ) { KROK3; }
	if( kr == 2 ) { KROK2; }
	if( kr == 3 ) { KROK1; }

	if( ++kr > 3 ) kr=0;
}



/* ================= PROCEDURA OBS£UGI PRZERWANIA – COMPARE MATCH */
/* pe³ni funkcjê timera programowego wyznaczaj¹cego podstawê czasu = 2ms oraz 1s */
ISR(TIMER0_COMP_vect)
{
	ms2_flag = 1;	/* ustawiamy flagê co 2ms */

	if(++ms2_cnt>499) {	/* gdy licznik ms > 499 (minê³a 1 sekunda) */
		s1_flag=1;	/* ustaw flagê tykniêcia sekundy */
		sekundy++;	/* zwiêksz licznik sekund */
		if(sekundy>59) sekundy=0; /* jeœli iloœæ sekund > 59 - wyzeruj */
		ms2_cnt=0;	/* wyzeruj licznik setnych ms */
	}
}
