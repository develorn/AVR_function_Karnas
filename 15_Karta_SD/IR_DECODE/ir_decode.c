/*
 * ir_decode.c
 *
 *  Created on: 2010-09-13
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "ir_decode.h"

volatile uint8_t address;		// adres RC5
volatile uint8_t command;		// komenda RC5
volatile uint8_t toggle_bit;		// bit TOGGLE

// flaga = 1 - informuje, ¿e odebrany zosta³ nowy kod z pilota
// po odczytaniu danych ze zmiennych nale¿y wyzerowaæ flagê aby
// zosta³y przyjête kolejne kody z pilota
volatile uint8_t Ir_key_press_flag;

volatile uint8_t rc5cnt;

void ir_init() {
	DDR(IR_PORT) &= ~IR_IN;		// pin jako wejœcie
	PORT(IR_PORT) |= IR_IN;		// podci¹gniêcie pinu do VCC
	#if TIMER1_PRESCALER == 1
		TCCR1B |= (1<<CS10);         	// Timer1 / 1
	#endif
	#if TIMER1_PRESCALER == 8
		TCCR1B |= (1<<CS11);         	// Timer1 / 8
	#endif
	#if TIMER1_PRESCALER == 64
	  	TCCR1B |= (1<<CS11)|(1<<CS10); // Timer1 / 64
	#endif
	#if TIMER1_PRESCALER == 256
		TCCR1B |= (1<<CS12);         	// Timer1 / 256
	#endif

	TCCR1B &= ~(1<<ICES1);      	// Zbocze opadaj¹ce na ICP
	rc5cnt = 0;						// zerowanie licznika wystêpuj¹ych zboczy

	TIMSK |= (1<<TICIE1);        	// Przerwanie od ICP
	Ir_key_press_flag = 0;			// zerowanie flagi otrzymania kodu z pilota
}





// procedura obs³ugi przerwania ICP1
ISR(TIMER1_CAPT_vect) {

#define FRAME_RESTART 0
#define FRAME_OK 1
#define FRAME_END 2
#define FRAME_ERROR 3

	static uint16_t LastCapture;
	uint16_t PulseWidth;
	static uint8_t IrPulseCount;
	static uint16_t IrData;
	static uint8_t frame_status;


	PulseWidth = ICR1 - LastCapture;
	LastCapture = ICR1;

	TCCR1B ^= (1<<ICES1);	// zmiana zbocza wyzwalaj¹cego na przeciwne

	if (PulseWidth > MAX_BIT) rc5cnt = 0;

	if (rc5cnt > 0) frame_status = FRAME_OK;

	if (rc5cnt == 0) {
		IrData = 0;
		IrPulseCount = 0;
		TCCR1B |= (1<<ICES1);
		rc5cnt++;
		frame_status = FRAME_END;
	}

	if (frame_status == FRAME_OK) {

	    // gdy zak³ócenia (szpilki) - RESTART
		if(PulseWidth<MIN_HALF_BIT) frame_status = FRAME_RESTART;

			// gdy b³¹d ramki danych (mo¿e inny standard ni¿ RC5) RESTART
			if( PulseWidth > MAX_BIT ) frame_status = FRAME_RESTART;

			if (frame_status == FRAME_OK) 	{
				if (PulseWidth > MAX_HALF_BIT) rc5cnt++;

					if (rc5cnt > 1)
					if ( (rc5cnt % 2) == 0 ) {
						IrData = IrData << 1;
						if((TCCR1B & (1<<ICES1))) IrData |= 0x0001;
						IrPulseCount++;

						if (IrPulseCount > 12) 	{
							if (Ir_key_press_flag == 0) {
								command = IrData & 0b0000000000111111;
								address = (IrData & 0b0000011111000000) >> 6;
								toggle_bit = (IrData & 0b0000100000000000) >> 11;
							}
							frame_status = FRAME_RESTART;
							Ir_key_press_flag = 1;
						}
					}
					rc5cnt++;
			}
	}

	if (frame_status == FRAME_RESTART)	{
		rc5cnt = 0;
		TCCR1B &= ~(1<<ICES1);
	}
}

