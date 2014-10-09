/*
 * main.c
 *
 *  Created on: 2010-09-14
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <util/delay.h>

/* przydatne definicje pinów steruj¹cych */
#define WE_A PD1
#define WE_B PD0

/* definicje poleceñ steruj¹cych prac¹ silnika */
#define DC_LEWO PORTD &= ~(1<<WE_A); PORTD |= (1<<WE_B)
#define DC_PRAWO PORTD |= (1<<WE_A); PORTD &= ~(1<<WE_B)
#define DC_STOP PORTD &= ~(1<<WE_A); PORTD &= ~(1<<WE_B)


int main(void) {

	/* ustawiamy piny steruj¹ce L293D jako wyjœcia */
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
