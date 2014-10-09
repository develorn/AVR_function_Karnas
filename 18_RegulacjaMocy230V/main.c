/*
 * main.c
 *
 *  Created on: 2010-09-27
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


/*********************** D I M M E R  -   U S T A W I E N I A   ***************************/
#define LICZBA_KROKOW 200

// czas trwania szpilki (Detekcja ZERA) wyra¿ony w milisekundach np: 1,2,... albo np: 0.5
#define SZPILKA_MS 1.8
/*********************** D I M M E R  -   U S T A W I E N I A   ***************************/

#define __OCR_STEP ((F_CPU/8)*((1000-((SZPILKA_MS*100)/2))/LICZBA_KROKOW))/100000

//#define procent(x) ((x*(LICZBA_KROKOW-1))/100)

static uint8_t procent(uint8_t pr) {
	return (pr * (LICZBA_KROKOW-1))/100;
}


#define T_ZAR1_ON PORTC |= (1<<PC0)
#define T_ZAR1_OFF PORTC &= ~(1<<PC0)

#define T_ZAR2_ON PORTC |= (1<<PC1)
#define T_ZAR2_OFF PORTC &= ~(1<<PC1)

#define T_ZAR3_ON PORTC |= (1<<PC2)
#define T_ZAR3_OFF PORTC &= ~(1<<PC2)

volatile uint8_t kroki;
volatile uint8_t kanal1;
volatile uint8_t kanal2;
volatile uint8_t kanal3;

volatile uint8_t ika1;
volatile uint8_t ika2;
volatile uint8_t ika3;


int main(void) {

	DDRC |= (1<<PC0)|(1<<PC1)|(1<<PC2);
	T_ZAR1_OFF;
	T_ZAR2_OFF;
	T_ZAR3_OFF;

	// Przerwanie INT0
	MCUCR |= (1<<ISC01)|(1<<ISC00);	// zbocze narastaj¹ce
	GICR |= (1<<INT0);				// odblokowanie INT0


	// TIMER2
#define TIMER2_START TCCR2 |= (1<<CS21)	// prescaler=8
#define TIMER2_STOP TCCR2 &= ~(1<<CS21)	// prescaler off
	TCCR2 |= (1<<WGM21);				// tryb CTC
	TIMSK |= (1<<OCIE2);				// odblokowanie przerwania COMPARE MATCH
	OCR2 = __OCR_STEP;


	sei();

	kanal1 = procent(10);
	kanal2 = procent(60);
	kanal3 = procent(90);
	_delay_ms(3000);


#define SZYBKOSC_SCIEMNIACZA 15
	uint8_t i,k;
	while(1) {
		for( i=0; i<101; i++ ) {
			kanal1 = procent(i);
			kanal2 = procent(100-i);
			kanal3 = procent(i);
			_delay_ms(SZYBKOSC_SCIEMNIACZA);
		}
		_delay_ms(2000);
		for( k=0, i=100; k<101; k++,i-- ) {
			kanal1 = procent(i);
			kanal2 = procent(100-i);
			kanal3 = procent(i);
			_delay_ms(SZYBKOSC_SCIEMNIACZA);
		}
		_delay_ms(2000);
	}
}


ISR(INT0_vect) {
	TIMER2_STOP;
	T_ZAR1_OFF; T_ZAR2_OFF; T_ZAR3_OFF;

	kroki=LICZBA_KROKOW;

	/* podwójne buforowanie, synchronizacja do 50Hz */
	ika1=kanal1;
	ika2=kanal2;
	ika3=kanal3;

	TCNT2 = 0;
	TIMER2_START;
}

ISR(TIMER2_COMP_vect) {
	if(ika1 && kroki == ika1) T_ZAR1_ON;
	if(ika2 && kroki == ika2) T_ZAR2_ON;
	if(ika3 && kroki == ika3) T_ZAR3_ON;
	kroki--;
}
