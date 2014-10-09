/*
 * main.c
 *
 *  Created on: 2010-04-05
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// definicje zmiennych do sterowania 6 kana�ami programowych PWM
// zmienne typu uint8_t, rozdzielczo�� 8-bitowa
volatile uint8_t pwm1, pwm2, pwm3, pwm4, pwm5, pwm6;

// g��wna funkcja programu main()
int main(void)
{
	//***** SPRZ�TOWY PWM - 1 KANA� OC0 (PB3) *******
	// ustawienie ko�c�wki OC0 (PB3) sprz�towy PWM jako WYJ�CIE
	DDRB |= (1<<PB3);
	// ustawienia TIMER2 w Fast PWM
	TCCR0 |= (1<<WGM01)|(1<<WGM00);		// tryb Fast PWM
	TCCR0 |= (1<<COM01);				// clear at TOP
	TCCR0 |= (1<<CS00);					// preskaler = 1
	OCR0=255;							// wygaszenie diody w kanale PWM


	//***** PROGRAMOWY PWM - 6 KANA��W *******
	// ustawienie pin�w kana��w programowych PWM jako WYJ�CIA
	DDRC |= (1<<PC0)|(1<<PC1)|(1<<PC2);
	DDRD |= (1<<PD7)|(1<<PD6)|(1<<PD2);
	// wy��czenie diod LED pod��czonych katodami do wyj��
	PORTC |= (1<<PC0)|(1<<PC1)|(1<<PC2);
	PORTD |= (1<<PD7)|(1<<PD6)|(1<<PD2);

	// ustawienia TIMER2 w tryb CTC
	TCCR2 |= (1<<WGM21);	// tryb  CTC
	TCCR2 |= (1<<CS20);		// preskaler = 1
	OCR2 = 200;				// dodatkowy podzia� cz�sttotliwo�ci przez 200
	TIMSK |= (1<<OCIE2);	// zezwolenie na przerwanie CompareMatch

	sei();				// odblokowanie globalne przerwa�
	uint8_t i;			// definicja zmiennej i na potrzeby p�tli for()


	while(1)
	{
		// p�tla rozja�niaj�ca 6 diod LED
		for(i=0;i<255;i++)
		{
			OCR0=i;		// ta dioda b�dzie ulega�a �ciemnianiu w tej p�tli
			pwm1=i;
			pwm2=i;
			pwm3=i;
			pwm4=i;
			pwm5=i;
			pwm6=i;

			// W zwi�zku z du�� nieliniowo�ci� zale�no�ci pr�du diod LED
			// i jasno�ci �wiecenia, wprowadzone jest zmienne op�nienie
			// przy warto�ciach bliskich zera aby zminimalizowa� ten efekt
			if(i>50) _delay_ms(3);
			else _delay_ms(10);
		}
		// ca�kowite rozja�nienie diody LED przez 100ms
		OCR0=255;
		// roz�wietlenie na maksimum przez 100ms
		pwm1=255;
		pwm2=255;
		pwm3=255;
		pwm4=255;
		pwm5=255;
		pwm6=255;
		_delay_ms(500);

		// p�tla stopniowo �ciemniaj�ca 6 diod LED
		for(i=255;i;i--)
		{
			OCR0=i;		// ta dioda b�dzie ulega�a rozja�nianiu w tej p�tli
			pwm1=i;
			pwm2=i;
			pwm3=i;
			pwm4=i;
			pwm5=i;
			pwm6=i;

			// W zwi�zku z du�� nieliniowo�ci� zale�no�ci pr�du diod LED
			// i jasno�ci �wiecenia, wprowadzone jest zmienne op�nienie
			// przy warto�ciach bliskich zera aby zminimalizowa� ten efekt
			if(i>50) _delay_ms(3);
			else _delay_ms(10);
		}
		// ca�kowite rozja�nienie diody LED przez 100ms
		OCR0=0;
		// ca�kowite wygaszenie diod LED przez 100ms
		pwm1=0;
		pwm2=0;
		pwm3=0;
		pwm4=0;
		pwm5=0;
		pwm6=0;
		_delay_ms(500);
	}

}


// cia�o procedury obs�ugi przerwania Compare Match Timera2
ISR( TIMER2_COMP_vect )
{
	static uint8_t cnt; // definicja naszego licznika PWM

	// bezpo�rednie sterowanie wyj�ciami kana��w PWM
	if(cnt>=pwm1) PORTC |= (1<<PC0); else PORTC &= ~(1<<PC0);
	if(cnt>=pwm2) PORTC |= (1<<PC1); else PORTC &= ~(1<<PC1);
	if(cnt>=pwm3) PORTC |= (1<<PC2); else PORTC &= ~(1<<PC2);
	if(cnt>=pwm4) PORTD |= (1<<PD7); else PORTD &= ~(1<<PD7);
	if(cnt>=pwm5) PORTD |= (1<<PD6); else PORTD &= ~(1<<PD6);
	if(cnt>=pwm6) PORTD |= (1<<PD2); else PORTD &= ~(1<<PD2);

	cnt++;	// zwi�kszanie licznika o 1
}


