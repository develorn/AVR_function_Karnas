/*
 * main.c
 *
 *  Created on: 2010-09-24
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <util/delay.h>

// Tutaj definiujemy jaki adres bêdzie emitowa³ pilot IR
#define _ADDRESS_RC5 0		// TV = 0

// przydzielamy dowolne kody (command) do klawiszy 1-5
#define _CMD1_RC5 1
#define _CMD2_RC5 2
#define _CMD3_RC5 3
#define _CMD4_RC5 4
#define _CMD5_RC5 5

// definicje dla obs³ugi diody LED
#define LED_PORT PORTB
#define LED_PIN PINB
#define LED_DDR DDRB
#define LED_PIN_NR 2
#define LED (1<<LED_PIN_NR)

#define LED_OFF 	PORTB 	|= 	LED
#define LED_ON 		PORTB 	&= 	~LED


// definicje klawiszy
#define K_PORT PORTB
#define K_PIN PINB
#define K_DDR DDRB

#define KEY1 (1<<PB1)
#define KEY2 (1<<PB3)
#define KEY3 (1<<PB4)
#define KEY4 (1<<PB5)
#define KEY5 (1<<PB6)

#define KEYS_MASK (KEY1|KEY2|KEY3|KEY4|KEY5)

uint8_t address;
uint8_t command;

/**************** MAIN **********************/
int main(void) {

	// ****** DIODA LED i IR
	// ustawienie PORTB.2 (OC0A) jako wyjœcie
	LED_DDR 	|= 	(LED);
	LED_PORT	|=	(LED);

	// ******* KLAWISZE
	// ustawienie PORTD.2 , 3 , 4 i 5 jako wyjœcia
	K_DDR 	|= 	(KEY1|KEY2|KEY3|KEY4|KEY5);
	// ustawienie stanu niskiego
	K_PORT 	&= 	~(KEY1|KEY2|KEY3|KEY4|KEY5);

	// kontrolne zapalenia diody LED na 255ms po w³¹czeniu zasilania (w³o¿eniu baterii do pilota)
	LED_ON;
	_delay_ms(255);
	LED_OFF;

	//OSCCAL = 0x66;


	// **************************************************************************************
	// TIMER0
	// - u¿ywany do generowania fali noœnej dla IR
	// za³adowanie OCR0A wartoœci¹ do generowania noœnej ok 36kHz = 110
	OCR0A =  110;
	// ustawienie Timer0 w tryb = 2 - CTC - CompareA
	TCCR0A |= (1<<WGM01);	// tryb CTC
	TCCR0B |= (1<<CS00);	// preskaler = 1
	//****************************************************************************************

	// Timer1 - s³u¿y do odmierzania opóŸnieñ (z dok³adnoœci¹ wielokrotnoœci 1us)
	// jego preskaler ustawiany jest na 8 podczas za³¹czania
	// u¿ywana jest wtedy jeo flaga OCF0A
	// nie korzystamy z przerwañ
	// ustawiamy Timer1 w tryb = 4 - CTC
	TCCR1B |= (1<<WGM12);

	// wy³¹czenie komparatora analogowego
	// konieczne ze wzglêdu na maksymalne obni¿enie poboru pr¹du
	ACSR |= (1<<ACD);

	// Turn off WDT
	WDTCSR = 0x00;

	// ustawienie trybu POWER-DOWN
	MCUCR |= ((1<<SM0)|(1<<SM1));

	// ustawienie pinu PORTD.2 (INT0) jako wejœcie
	DDRD &= ~(1<<PD2);
	// podci¹gniêcie do 1
	PORTD |= (1<<PD2);
	// zezwolenie na przerwania INT0
	GIMSK |= (1<<INT0);
	// skasowanie flagi wyst¹pienia przerwania INT0
	EIFR |= (1<<INTF0);

	// globalne zezwolenie na przerwania
	sei();

	while (1) {
		// wprowadzenie procesora w tryb POWER-DOWN
		sleep_mode();
	}
}

//------------------------------------------------- RC5 - START
// dok³adna pêtla opóŸniaj¹ca = wielokrotnoœci 1us
// w oparciu o Timer1, taktowanie 8MHz, preskaler = 8
void czekaj_us(uint16_t usekundy) {
	OCR1A = usekundy;
	TIFR |= (1<<OCF1A);
	TCCR1B |= (1<<CS11);
	while ( !( TIFR & (1<<OCF1A) ) ) {};
	TCCR1B &= ~(1<<CS11);
}
// przes³anie bitu o wartoœci 1
void send_rc5_one() {
	czekaj_us(889);
	TCCR0A |= (1<<COM0A0);
	czekaj_us(889);
	TCCR0A &= ~(1<<COM0A0);
}
// przes³anie bitu o wartoœci 0
void send_rc5_zero() {
	TCCR0A |= (1<<COM0A0);
	czekaj_us(889);
	TCCR0A &= ~(1<<COM0A0);
	czekaj_us(889);
}

// przes³anie kompletnej ramki RC5
void send_rc5(uint8_t adr, uint8_t cmd, uint8_t tog) {
	uint16_t data = 0;
	uint8_t i=15;

	// Musimy uformowaæ ramkê RC5
	// w tym celu przesuwamy bity do wys³ania w lew¹ stronê
	data |= ( (1<<15)|(1<<14)|(tog<<13)|(adr<<8)|(cmd<<2) );
	/* data = 0bssTaaaaaccccccxx
	 *   	  	||||adr||cmd |
	 *   		|||
	 *   		||+-- bit TOGGLE
	 *			||
	 *			++--- 2 bity Startu
	*/

	// wysy³amy kolejno 14 bitów
	// 2-bity startu, 1-bit Toggle, 5-bitów adresu, 6-bitów komendy
	do {
		if ( !(data & ( 1 << i )) ) send_rc5_zero();
		else send_rc5_one();
	} while(--i>1);
}
//------------------------------------------------- RC5 - END


//*************** PRZERWANIE INT0 ***************************
ISR(INT0_vect) {

	uint8_t keys = PINB;
	static uint8_t toggle_bit;

	// tylko na czas przerwania INT0 - przestawiony zostaje
	// kierunek portów klawiszy. Zostaj¹ ustawione jako wejœcia
	// natomiast pin PORTD.2 (INT0) jako wyjœcie ze stanem niskim
	// dziêki temu dzia³a obs³uga klawiszy

	// ustawienie PORTD.2 , 3 , 4 i 5 jako wejœcia
	K_DDR 	&= 	~(KEY1|KEY2|KEY3|KEY4|KEY5);
	// podci¹gniêcie wejœæ do 1
	K_PORT 	|= 	(KEY1|KEY2|KEY3|KEY4|KEY5);

	// przestawienie PORTD.2 jako wyjœcie
	DDRD |= (1<<PD2);
	// ustawienie stanu niskiego - do obs³ugi klawiszy
	PORTD &= ~(1<<PD2);

	if( (keys & KEYS_MASK) != KEYS_MASK ) {	// sprawdzamy czy wciœniêty jakikolwiek klawisz
		_delay_ms(50);			// eliminacja drgañ styków
		keys = PINB;
		if( (keys & KEYS_MASK) != KEYS_MASK ) {	// sprawdzamy czy wciœniêty jakikolwiek klawisz

			// przed wys³aniem kodu jakiegokolwiek klawisza zmiana
			// stanu bitu toggle
			toggle_bit ^= (1<<0);

			do {

				if( !(keys & KEY1) ) command = _CMD1_RC5;
				else
				if( !(keys & KEY2) ) command = _CMD2_RC5;
				else
				if( !(keys & KEY3) ) command = _CMD3_RC5;
				else
				if( !(keys & KEY4) ) command = _CMD4_RC5;
				else
				if( !(keys & KEY5) ) command = _CMD5_RC5;

				address = _ADDRESS_RC5;
				send_rc5(address, command, toggle_bit);

				_delay_ms(115);	// gap

				keys = PINB;

			} while ( (keys & KEYS_MASK) != KEYS_MASK );	/* powtarzaj gdy wciœniêty */
		}
	}

	// przywracamy ustawienia

	// ustawienie pinu PORTD.2 (INT0) jako wejœcie
	DDRD &= ~(1<<PD2);
	// podci¹gniêcie do 1
	PORTD |= (1<<PD2);

	// ******* KLAWISZE
	// ustawienie PORTD.2 , 3 , 4 i 5 jako wyjœcia
	K_DDR 	|= 	(KEY1|KEY2|KEY3|KEY4|KEY5);
	// ustawienie stanu niskiego
	K_PORT 	&= 	~(KEY1|KEY2|KEY3|KEY4|KEY5);

	EIFR |= (1<<INTF0);

}














