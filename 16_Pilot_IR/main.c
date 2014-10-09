/*
 * main.c
 *
 *  Created on: 2010-09-24
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <util/delay.h>

// Tutaj definiujemy jaki adres b�dzie emitowa� pilot IR
#define _ADDRESS_RC5 0		// TV = 0

// przydzielamy dowolne kody (command) do klawiszy 1-5
#define _CMD1_RC5 1
#define _CMD2_RC5 2
#define _CMD3_RC5 3
#define _CMD4_RC5 4
#define _CMD5_RC5 5

// definicje dla obs�ugi diody LED
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
	// ustawienie PORTB.2 (OC0A) jako wyj�cie
	LED_DDR 	|= 	(LED);
	LED_PORT	|=	(LED);

	// ******* KLAWISZE
	// ustawienie PORTD.2 , 3 , 4 i 5 jako wyj�cia
	K_DDR 	|= 	(KEY1|KEY2|KEY3|KEY4|KEY5);
	// ustawienie stanu niskiego
	K_PORT 	&= 	~(KEY1|KEY2|KEY3|KEY4|KEY5);

	// kontrolne zapalenia diody LED na 255ms po w��czeniu zasilania (w�o�eniu baterii do pilota)
	LED_ON;
	_delay_ms(255);
	LED_OFF;

	//OSCCAL = 0x66;


	// **************************************************************************************
	// TIMER0
	// - u�ywany do generowania fali no�nej dla IR
	// za�adowanie OCR0A warto�ci� do generowania no�nej ok 36kHz = 110
	OCR0A =  110;
	// ustawienie Timer0 w tryb = 2 - CTC - CompareA
	TCCR0A |= (1<<WGM01);	// tryb CTC
	TCCR0B |= (1<<CS00);	// preskaler = 1
	//****************************************************************************************

	// Timer1 - s�u�y do odmierzania op�nie� (z dok�adno�ci� wielokrotno�ci 1us)
	// jego preskaler ustawiany jest na 8 podczas za��czania
	// u�ywana jest wtedy jeo flaga OCF0A
	// nie korzystamy z przerwa�
	// ustawiamy Timer1 w tryb = 4 - CTC
	TCCR1B |= (1<<WGM12);

	// wy��czenie komparatora analogowego
	// konieczne ze wzgl�du na maksymalne obni�enie poboru pr�du
	ACSR |= (1<<ACD);

	// Turn off WDT
	WDTCSR = 0x00;

	// ustawienie trybu POWER-DOWN
	MCUCR |= ((1<<SM0)|(1<<SM1));

	// ustawienie pinu PORTD.2 (INT0) jako wej�cie
	DDRD &= ~(1<<PD2);
	// podci�gni�cie do 1
	PORTD |= (1<<PD2);
	// zezwolenie na przerwania INT0
	GIMSK |= (1<<INT0);
	// skasowanie flagi wyst�pienia przerwania INT0
	EIFR |= (1<<INTF0);

	// globalne zezwolenie na przerwania
	sei();

	while (1) {
		// wprowadzenie procesora w tryb POWER-DOWN
		sleep_mode();
	}
}

//------------------------------------------------- RC5 - START
// dok�adna p�tla op�niaj�ca = wielokrotno�ci 1us
// w oparciu o Timer1, taktowanie 8MHz, preskaler = 8
void czekaj_us(uint16_t usekundy) {
	OCR1A = usekundy;
	TIFR |= (1<<OCF1A);
	TCCR1B |= (1<<CS11);
	while ( !( TIFR & (1<<OCF1A) ) ) {};
	TCCR1B &= ~(1<<CS11);
}
// przes�anie bitu o warto�ci 1
void send_rc5_one() {
	czekaj_us(889);
	TCCR0A |= (1<<COM0A0);
	czekaj_us(889);
	TCCR0A &= ~(1<<COM0A0);
}
// przes�anie bitu o warto�ci 0
void send_rc5_zero() {
	TCCR0A |= (1<<COM0A0);
	czekaj_us(889);
	TCCR0A &= ~(1<<COM0A0);
	czekaj_us(889);
}

// przes�anie kompletnej ramki RC5
void send_rc5(uint8_t adr, uint8_t cmd, uint8_t tog) {
	uint16_t data = 0;
	uint8_t i=15;

	// Musimy uformowa� ramk� RC5
	// w tym celu przesuwamy bity do wys�ania w lew� stron�
	data |= ( (1<<15)|(1<<14)|(tog<<13)|(adr<<8)|(cmd<<2) );
	/* data = 0bssTaaaaaccccccxx
	 *   	  	||||adr||cmd |
	 *   		|||
	 *   		||+-- bit TOGGLE
	 *			||
	 *			++--- 2 bity Startu
	*/

	// wysy�amy kolejno 14 bit�w
	// 2-bity startu, 1-bit Toggle, 5-bit�w adresu, 6-bit�w komendy
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
	// kierunek port�w klawiszy. Zostaj� ustawione jako wej�cia
	// natomiast pin PORTD.2 (INT0) jako wyj�cie ze stanem niskim
	// dzi�ki temu dzia�a obs�uga klawiszy

	// ustawienie PORTD.2 , 3 , 4 i 5 jako wej�cia
	K_DDR 	&= 	~(KEY1|KEY2|KEY3|KEY4|KEY5);
	// podci�gni�cie wej�� do 1
	K_PORT 	|= 	(KEY1|KEY2|KEY3|KEY4|KEY5);

	// przestawienie PORTD.2 jako wyj�cie
	DDRD |= (1<<PD2);
	// ustawienie stanu niskiego - do obs�ugi klawiszy
	PORTD &= ~(1<<PD2);

	if( (keys & KEYS_MASK) != KEYS_MASK ) {	// sprawdzamy czy wci�ni�ty jakikolwiek klawisz
		_delay_ms(50);			// eliminacja drga� styk�w
		keys = PINB;
		if( (keys & KEYS_MASK) != KEYS_MASK ) {	// sprawdzamy czy wci�ni�ty jakikolwiek klawisz

			// przed wys�aniem kodu jakiegokolwiek klawisza zmiana
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

			} while ( (keys & KEYS_MASK) != KEYS_MASK );	/* powtarzaj gdy wci�ni�ty */
		}
	}

	// przywracamy ustawienia

	// ustawienie pinu PORTD.2 (INT0) jako wej�cie
	DDRD &= ~(1<<PD2);
	// podci�gni�cie do 1
	PORTD |= (1<<PD2);

	// ******* KLAWISZE
	// ustawienie PORTD.2 , 3 , 4 i 5 jako wyj�cia
	K_DDR 	|= 	(KEY1|KEY2|KEY3|KEY4|KEY5);
	// ustawienie stanu niskiego
	K_PORT 	&= 	~(KEY1|KEY2|KEY3|KEY4|KEY5);

	EIFR |= (1<<INTF0);

}














