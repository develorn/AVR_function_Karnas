/*
 * main.c
 *
 *  Created on: 2010-09-28
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "LCD/lcd44780.h"
#include "1Wire/ds18x20.h"
#include "d_led.h"
#include "I2C_TWI/i2c_twi.h"
#include "MKUART/mkuart.h"
#include "IR_DECODE/ir_decode.h"


//******************************* definicje preprocesora
#define PCF8583_ADDR 0xA2	// adres uk�adu RTC na I2C


#define LED1 (1<<PC3)	// zwyk�a dioda LED
#define LED2 (1<<PA7)	// pod�wietlenie LCD

#define LED1_ON PORTC &= ~LED1
#define LED1_OFF PORTC |= LED1
#define LED1_TOG PORTC ^= LED1
#define LED2_ON PORTA |= LED2
#define LED2_OFF PORTA &= ~LED2
#define LED2_TOG PORTA ^= LED2
#define LED1_DDR DDRC
#define LED2_DDR DDRA

#define KL1 (1<<PD3)
#define KL2 (1<<PD4)
#define KL3 (1<<PD5)
#define KL4 (1<<PD7)
#define KL_DDR DDRD
#define KL_PORT PORTD



//******************************* deklaracje funkcji
void display_temp(uint8_t x);
// konwersja liczby dziesi�tnej na BCD
uint8_t dec2bcd(uint8_t dec);
// konwersja liczby BCD na dziesi�tn�
uint8_t bcd2dec(uint8_t bcd);

/************** funkcja SuperDebounce do obs�ugi pojedynczych klawiszy ***************/
void SuperDebounce( volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) );

void kl1_press(void);
void kl1_rep(void);
void kl2_press(void);
void kl2_rep(void);


//******************************* definicje zmiennych globalnych i typ�w
uint8_t subzero, cel, cel_fract_bits;
uint8_t czujniki_cnt;		/* ilo�� czujnik�w na magistrali */

volatile uint8_t int0_flag=1;	// flaga zmieniana w przerwaniu i sprawdzana w p�tli g��wnej

// definiujemy sobie dla polepszenia czytelno�ci programu typ wyliczeniowy
// wskazuj�cy nam p�niej na odpowiednie indeksy w tablicy (buforze)
enum {ss=1, mm, hh};
uint8_t bufor[4];		// rezerwacja bufora 4 bajty
uint8_t sekundy, minuty, godziny;

// definicja znaku stopnia Celsiusza dla temperatury
uint8_t stopien[] = {12,18,18,12,32,32,32,32};

volatile uint16_t Timer1, Timer2, Timer3, Timer4;	/* timery programowe 100Hz */

uint8_t lcd_blink;	/* zmienna odpowiedzialna za za��czanie migotania pod�wietlenia LCD */
uint8_t lcd_cnt;	/* zmienna licznik do wy�wietlania na LCD */


/*----------------------------- g��wna funkcja programu  ---------------*/
int main(void) {


/*----------------------------- inicjalizacja kierunk�w pin�w  ---------------*/
	// ustalamy kierunki port�w klawiszy na wyj�ciowe
	KL_DDR 	&= ~(KL1|KL2|KL3|KL4);
	// w��czamy podci�ganie do VCC
	KL_PORT |= KL1|KL2|KL3|KL4;

	// ustalamy kierunki pin�w diod LED
	LED1_DDR |= LED1; LED2_DDR |= LED2;
	// wy��czay diod�
	LED1_OFF;
	// za��czamy pod�wietlenie
	LED2_ON;



/*----------------------------- inicjalizacja przerwa�  ---------------*/
	// Przerwanie INT0
	MCUCR |= (1<<ISC01);	// wyzwalanie zboczem opadaj�cym
	GICR |= (1<<INT0);		// odblokowanie przerwania
	PORTD |= (1<<PD2);		// podci�gni�cie pinu INT0 do VCC

	/* Timer2 � inicjalizacja przerwania co 10ms */
	TCCR2 	|= (1<<WGM21);			// tryb pracy CTC
	TCCR2 	|= (1<<CS22)|(1<<CS21)|(1<<CS20);	// preskaler = 1024
	OCR2 	= 108;					// przerwanie por�wnania co 10ms (100Hz)
	TIMSK 	= (1<<OCIE2);			// Odblokowanie przerwania CompareMatch


/*----------------------------- inicjalizacja modu��w programowych  ---------------*/
	/* inicjalizacja multipleksowania LED */
	d_led_init();
	/*ustawienie samych zer do wy�wietlania */
	cy1=0; cy2=0; cy3=0; cy4=0;

	/* inicjalizacja LCD */
	lcd_init();
	lcd_str("Start...");
	lcd_defchar(0x80,stopien); // definicja znaku stopnia w LCD

	/* inicjalizacja odbiornika podczerwieni RC5 */
	ir_init();

	/* inicjalizacja USART */
	USART_Init( __UBRR );


	/* globalne  odblokowanie przerwa� */
	sei();

	uart_puts("Projekt wielozadaniowy\r\n");

	/* sprawdzamy ile czujnik�w DS18xxx widocznych jest na magistrali */
	czujniki_cnt = search_sensors();
	DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );

	/* czekamy 750ms na dokonanie konwersji przez pod��czone czujniki */
	_delay_ms(1500);

	lcd_cls();

	/* dokonujemy odczytu temperatury z pierwszego czujnika o ile zosta� wykryty */
	/* wy�wietlamy temperatur� gdy czujnik wykryty */
	if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) display_temp(0);

	uint8_t tcl=0;

	char input;

	while(1) {

		SuperDebounce(&PIND, KL1, 20, 500, kl1_press, kl1_rep );
		SuperDebounce(&PIND, KL2, 80, 400, kl2_press, kl2_rep );

		if(!Timer2) {
				Timer2=25;
				if( lcd_blink ) LED2_TOG;
		}

		input = uart_getc();

		if(input=='1') LED2_ON;
		else
		if(input=='2') LED2_OFF;
		else
		if(input=='3') lcd_blink ^= 1;

		/* ****** ZDARZENIE Z UK�ADU RTC ********** */
		if ( int0_flag ) {
			//odczyt 4 bajt�w do bufora od adresu 0x01 z pami�ci RAM naszego RTC
			TWI_read_buf( PCF8583_ADDR, 0x01, 4, bufor );

			sekundy = bcd2dec( bufor[ss] );
			minuty = bcd2dec( bufor[mm] );
			godziny = bcd2dec( bufor[hh] );

			cy1 = bufor[mm]>>4;
			cy2 = bufor[mm]&0x0f;
			cy3 = bufor[ss]>>4;
			cy4 = bufor[ss]&0x0f;

			// wy�wietlenie czasu na LCD
			lcd_locate(0,8);
			if( godziny < 10 ) lcd_str("0");
			lcd_int(godziny);
			lcd_str(":");
			if( minuty < 10 ) lcd_str("0");
			lcd_int(minuty);
			lcd_str(":");
			if( sekundy < 10 ) lcd_str("0");
			lcd_int(sekundy);

			if(!tcl) DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL );

			if(tcl) {
				if( DS18X20_OK == DS18X20_read_meas(gSensorIDs[0], &subzero, &cel, &cel_fract_bits) ) display_temp(0);
			}

			if(++tcl>1) tcl=0;

			int0_flag=0;
		}

		/* ****** ZDARZENIE Z ODBIORNIKA PODCZERWIENI ********** */
		if(Ir_key_press_flag) {

			if( !address && 1==command ) lcd_blink = 1;	// za��czamy miganie pod�wietlenia LCD
			if( !address && 2==command ) lcd_blink = 0;	// wy��czamy miganie pod�wietlenia LCD
			if( !address && 4==command ) LED2_ON; // w��czamy pod�wietlenie LCD
			if( !address && 5==command ) LED2_OFF; // wy��czamy pod�wietlenie LCD

			lcd_locate(1,8);
			lcd_str_P(PSTR("rc5: "));
			lcd_int(command);
			lcd_str_P(PSTR("  "));

			uart_puts("rc5: adr=");
			uart_putint(address, 10);
			uart_puts("  cmd=");
			uart_putint(command, 10);
			uart_puts("\r\n");

			Ir_key_press_flag = 0;
			command=0xff;
			address=0xff;
		}

	}
}


// procedura obs�ugi przerwania INT 0
ISR( INT0_vect ) {
	int0_flag = 1;
}


ISR(TIMER2_COMP_vect)
{
	uint16_t n;

	n = Timer1;		/* 100Hz Timer1 */
	if (n) Timer1 = --n;
	n = Timer2;		/* 100Hz Timer2 */
	if (n) Timer2 = --n;
	n = Timer3;		/* 100Hz Timer3 */
	if (n) Timer3 = --n;
	n = Timer4;		/* 100Hz Timer4 */
	if (n) Timer4 = --n;
}



/************** funkcja SuperDebounce do obs�ugi pojedynczych klawiszy ***************
 * 							AUTOR: Miros�aw Karda�
 * ZALETY:
 * 		- nie wprowadza najmniejszego spowalnienia
 * 		- posiada funkcj� REPEAT (powtarzanie akcji dla d�u�ej wci�ni�tego przycisku)
 * 		- mo�na przydzieli� r�ne akcje dla trybu REPEAT i pojedynczego klikni�cia
 * 		- mo�na przydzieli� tylko jedn� akcj� wtedy w miejsce drugiej przekazujemy 0 (NULL)
 * 		- zabezpieczenie przed naci�ni�ciem dw�ch klawiszy jednocze�nie
 *
 * Wymagania:
 * 	Timer programowy utworzony w oparciu o Timer sprz�towy (przerwanie 100Hz)
 *
 * 	Parametry wej�ciowe - 6 :
 *  *KPIN - nazwa PINx portu na kt�rym umieszczony jest klawisz, np: PINB
 *  key_mask - maska klawisza np: (1<<PB3)
 *  rep_time - czas powtarzania funkcji rep_proc w trybie REPEAT
 *  rep_wait - czas oczekiwania do przej�cia do trybu REPEAT
 *  push_proc - wska�nik do w�asnej funkcji wywo�ywanej raz po zwolenieniu przycisku
 *  rep_proc - wska�nik do w�asnej funkcji wykonywanej w trybie REPEAT
 **************************************************************************************/
void SuperDebounce(volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(void), void (*rep_proc)(void) ) {

	enum KS {idle, debounce, go_rep, wait_rep, rep};

	static enum KS key_state;
	static uint8_t last_key;
	uint8_t key_press;

	// zabezpieczenie przed wykonywaniem tej samej funkcji dla
	// dw�ch klawiszy wciskanych jednocze�nie (zawsze b�dzie
	// wykonywana odpowiednia akcja dla tego, kt�ry zosta�
	// wci�ni�ty jako pierwszy
	if( last_key && last_key != key_mask ) return;

	key_press = !(*KPIN & key_mask);

	if( key_press && !key_state ) {
		key_state = debounce;
		Timer1 = 5;
	} else
	if( key_state  ) {
		if( key_press && debounce==key_state && !Timer1 ) {
			key_state = go_rep;
			Timer1=3;
			last_key = key_mask;
		} else
		if( !key_press && key_state>debounce && key_state<rep ) {
			if(push_proc) push_proc();						/* KEY_UP */
			key_state=idle;
			last_key = 0;
		} else
		if( key_press && go_rep==key_state && !Timer1 ) {
			if(!rep_time) rep_time=20;
			if(!rep_wait) rep_wait=150;
			key_state = wait_rep;
			Timer1=rep_wait;
		} else
		if( key_press && wait_rep==key_state && !Timer1 ) {
			key_state = rep;
		} else
		if( key_press && rep==key_state && !Timer1 ) {
			Timer1 = rep_time;
			if(rep_proc) rep_proc();						/* KEY_REP */
		}
	}

	if( key_state>=wait_rep && !key_press ) {
		key_state = idle;
		last_key = 0;
	}
}



void kl1_press(void) {
	LED1_ON;
}

void kl1_rep(void) {
	LED1_TOG;
}

void kl2_press(void) {
	LED1_OFF;
}

void kl2_rep(void) {
	lcd_locate(0,0);
	lcd_str_P(PSTR("cnt "));
	lcd_int(lcd_cnt++);
	lcd_str_P(PSTR("  "));
}




/* wy�wietlanie temperatury na pozycji X w drugiej linii LCD */
void display_temp(uint8_t x) {
	lcd_locate(1,x);
	if(subzero) lcd_str("-");	/* je�li subzero==1 wy�wietla znak minus (temp. ujemna) */
	else lcd_str(" ");	/* je�li subzero==0 wy�wietl spacj� zamiast znaku minus (temp. dodatnia) */
	lcd_int(cel);	/* wy�wietl dziesi�tne cz�ci temperatury  */
	lcd_str(".");	/* wy�wietl kropk� */
	lcd_int(cel_fract_bits); /* wy�wietl dziesi�tne cz�ci stopnia */
	lcd_str("\x80""C "); /* wy�wietl znak jednostek (C - stopnie Celsiusza) */
}


// konwersja liczby dziesi�tnej na BCD
uint8_t dec2bcd(uint8_t dec) {
return ((dec / 10)<<4) | (dec % 10);
}

// konwersja liczby BCD na dziesi�tn�
uint8_t bcd2dec(uint8_t bcd) {
    return ((((bcd) >> 4) & 0x0F) * 10) + ((bcd) & 0x0F);
}
