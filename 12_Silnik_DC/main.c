/*
 * main.c
 *
 *  Created on: 2010-09-14
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>
#include <util/delay.h>

/* przydatne definicje pin�w steruj�cych */
#define WE_A PD1
#define WE_B PD0

/* definicje polece� steruj�cych prac� silnika */
#define DC_LEWO PORTD &= ~(1<<WE_A); PORTD |= (1<<WE_B)
#define DC_PRAWO PORTD |= (1<<WE_A); PORTD &= ~(1<<WE_B)
#define DC_STOP PORTD &= ~(1<<WE_A); PORTD &= ~(1<<WE_B)


int main(void) {

	/* ustawiamy piny steruj�ce L293D jako wyj�cia */
	DDRD |= (1<<WE_A)|(1<<WE_B);

	while(1) {
		DC_PRAWO;
		_delay_ms(1000);
		DC_STOP;
		_delay_ms(1000);
		DC_LEWO;
		_delay_ms(1000);
		DC_STOP;
		_delay_ms(1000);
	}
}
