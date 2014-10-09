/*
 * main.c
 *
 *  Created on: 2010-09-25
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "LCD/lcd44780.h"


#define LED1 (1<<PC6)
#define LED2 (1<<PC7)
#define LED3 (1<<PC5)
#define LED1_ON PORTC &= ~LED1
#define LED1_OFF PORTC |= LED1
#define LED2_ON PORTC &= ~LED2
#define LED2_OFF PORTC |= LED2

#define LED3_OFF PORTC |= LED3
#define LED3_TOG PORTC ^= LED3

#define KL1 (1<<PB0)
#define KL2 (1<<PB1)

#define NULL 0
#define DEFAULT 0

volatile uint16_t Timer1, Timer2;	/* timery programowe 100Hz */

uint8_t init;


/************** funkcja SuperDebounce do obs³ugi pojedynczych klawiszy ***************/
void SuperDebounce(uint8_t * key_state, volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) );

void led1_xor(void) {
	PORTC ^= LED1;
}

void led2_xor(void) {
	PORTC ^= LED2;
}

void inicjalizuj_lcd(void) {
	if(!init) {
		DDRA |= (1<<PA7);
		PORTA |= (1<<PA7); /* podœwietlenie wyœwietlacza LCD */
		lcd_init();
		lcd_str("Start...");
		init=1;
	}
}

void licznik_na_lcd(void) {
	static uint8_t licznik;
	if(init) {
		lcd_locate(1,0);
		lcd_str("licznik: ");
		lcd_int(licznik++);
		lcd_str("  ");
	}
}

int main(void) {

	DDRC |= LED1|LED2|LED3;
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;

	PORTB |= KL1|KL2;

	/* Timer2 – inicjalizacja przerwania co 10ms */
	TCCR2 	= (1<<WGM21);			// tryb pracy CTC
	TCCR2 	= (1<<CS22)|(1<<CS20);	// preskaler = 1024
	OCR2 	= 108;					// przerwanie porównania co 10ms (100Hz)
	TIMSK 	= (1<<OCIE2);			// Odblokowanie przerwania CompareMatch

	sei();


	while(1) {

		uint8_t k1, k2;

		SuperDebounce(&k1, &PINB, KL1, 20, 500, led1_xor, led1_xor );

		SuperDebounce(&k2, &PINB, KL2, DEFAULT, DEFAULT, inicjalizuj_lcd, licznik_na_lcd );

		if( !Timer2 ) {
			Timer2=100;
			LED3_TOG;
		}

	}
}



/************** funkcja SuperDebounce do obs³ugi pojedynczych klawiszy ***************
 * 							AUTOR: Miros³aw Kardaœ
 * ZALETY:
 * 		- nie wprowadza najmniejszego spowalnienia
 * 		- posiada funkcjê REPEAT (powtarzanie akcji dla d³u¿ej wciœniêtego przycisku)
 * 		- mo¿na przydzieliæ ró¿ne akcje dla trybu REPEAT i pojedynczego klikniêcia
 * 		- mo¿na przydzieliæ tylko jedn¹ akcjê wtedy w miejsce drugiej przekazujemy 0 (NULL)
 *
 * Wymagania:
 * 	Timer programowy utworzony w oparciu o Timer sprzêtowy (przerwanie 100Hz)
 *
 * 	Parametry wejœciowe:
 * 	*key_state - wskaŸnik na zmienn¹ w pamiêci RAM (1 bajt)
 *  *KPIN - nazwa PINx portu na którym umieszczony jest klawisz, np: PINB
 *  key_mask - maska klawisza np: (1<<PB3)
 *  rep_time - czas powtarzania funkcji rep_proc w trybie REPEAT
 *  rep_wait - czas oczekiwania do przejœcia do trybu REPEAT
 *  push_proc - wskaŸnik do w³asnej funkcji wywo³ywanej raz po zwolenieniu przycisku
 *  rep_proc - wskaŸnik do w³asnej funkcji wykonywanej w trybie REPEAT
 **************************************************************************************/
void SuperDebounce(uint8_t * key_state, volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) ) {

	enum {idle, debounce, go_rep, wait_rep, rep};

	if(!rep_time) rep_time=20;
	if(!rep_wait) rep_wait=150;

	uint8_t key_press = !(*KPIN & key_mask);

	if( key_press && !*key_state ) {
		*key_state = debounce;
		Timer1 = 15;
	} else
	if( *key_state  ) {

		if( key_press && debounce==*key_state && !Timer1 ) {
			*key_state = 2;
			Timer1=5;
		} else
		if( !key_press && *key_state>1 && *key_state<4 ) {
			if(push_proc) push_proc();						/* KEY_UP */
			*key_state=idle;
		} else
		if( key_press && go_rep==*key_state && !Timer1 ) {
			*key_state = wait_rep;
			Timer1=rep_wait;
		} else
		if( key_press && wait_rep==*key_state && !Timer1 ) {
			*key_state = rep;
		} else
		if( key_press && rep==*key_state && !Timer1 ) {
			Timer1 = rep_time;
			if(rep_proc) rep_proc();						/* KEY_REP */
		}
	}
	if( *key_state>=3 && !key_press ) *key_state = idle;
}



ISR(TIMER2_COMP_vect)
{
	uint16_t n;

	n = Timer1;		/* 100Hz Timer1 */
	if (n) Timer1 = --n;
	n = Timer2;		/* 100Hz Timer2 */
	if (n) Timer2 = --n;
}





