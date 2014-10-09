/*
 * main.c
 *
 *  Created on: 2010-03-18
 *      Author: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>


// MAKRA
#include "c:/eclipse/LIB_MK/makra_mk.h"

#include "IR_DECODE/ir_decode.h"
#include "LCD/lcd44780.h"

#include "I2C_TWI/i2c_twi.h"


#define PCF8583_ADDR 0xA2

#define BUZ_ON PORTC |= (1<<PC3);
#define BUZ_OFF PORTC &= ~(1<<PC3);

#define LCD_LED (1<<PB3)
#define LCD_ON PORTB |= LCD_LED;
#define LCD_OFF PORTB &= ~LCD_LED;
#define LCD_TOG PORTB ^= LCD_LED;

#define LED (1<<PA0)
#define LED_OFF PORTA |= LED;
#define LED_ON PORTA &= ~LED;
#define LED_TOG PORTA ^= LED;


#define TOSTRING( x ) STRINGX( x )
#define STRINGX( x ) #x

//#define TEST

#include "SDCARD/integer.h"
//#include "rtc.h"
#include "SDCARD/diskio.h"
#include "SDCARD/ff.h"
#include "SDCARD/ffconf.h"
//#include "el_chan/integer.h"



int tab[2] = {1,2};

long long int *w;


uint8_t sekundy, minuty, godziny;


uint8_t SDCH1[] = {14,16,12,2,28,32,21,32};		/* znaczek S */
uint8_t SDCH2[] = {28,18,18,18,28,32,21,32};	/* znaczek D */
uint8_t SD_IN[] = {32,31,31,31,31,31,15,7};		/* karta w slocie */
uint8_t SDOUT[] = {32,21,32,17,32,17,8,5};		/* karta wyjêta */

#ifndef TEST
typedef struct {
	WORD	year;	/* 2000..2099 */
	BYTE	month;	/* 1..12 */
	BYTE	mday;	/* 1.. 31 */
	BYTE	wday;	/* 1..7 */
	BYTE	hour;	/* 0..23 */
	BYTE	min;	/* 0..59 */
	BYTE	sec;	/* 0..59 */
} RTC;
#endif

#define CD (1<<PC2)

volatile WORD Timer;		/* 100Hz increment timer */

volatile uint8_t in0_flag;

DWORD get_fattime ()
{
	RTC rtc;


	/* Get local time */
	//rtc_gettime(&rtc);

	rtc.year = 2010;
	rtc.month = 3;
	rtc.mday = 20;
	rtc.hour = 10;
	rtc.min = 05;
	rtc.sec = 12;


	/* Pack date and time into a DWORD variable */
	return	  ((DWORD)(rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.mday << 16)
			| ((DWORD)rtc.hour << 11)
			| ((DWORD)rtc.min << 5)
			| ((DWORD)rtc.sec >> 1);
}


// konwersja liczby dziesiêtnej na BCD
uint8_t dec2bcd(uint8_t dec);
// konwersja liczby BCD na dziesiêtn¹
uint8_t bcd2dec(uint8_t bcd);


int wartosc=1268;

u08 kom_on;

void ledy()
{
	for(uint8_t m=0;m<5;m++)
	for(uint8_t k=1; k<(1<<7);k<<=1)
	{
		PORTA=~k;
		_delay_ms(20);
	}
}

void beep(uint8_t ms)
{
	BUZ_ON;
	while(ms--) _delay_ms(1);
	BUZ_OFF;
}

float fun(int tab[])
{
	double a=2.45;
	char z=6;
	float w;
	z=a;
	w=((z/a)*232.65)/3.78;
	if(w==2.66)	tab[0]=w*5.77;
	else tab[0]=5;
	lcd_cls();
	lcd_int(w);
	while(1);
	return w;
}

int main(void)
{

	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7);

	DDRC |= (1<<PC3);
	BUZ_OFF;

	DDRB |= LCD_LED;

	DDRA = 0xFF;
	PORTA = 0xFF;

	LED_OFF;

	PORTC |= CD;

	TCCR2 |= (1<<WGM21);
	OCR2 = 78;
	TCCR2 |= (1<<CS22)|(1<<CS21)|(1<<CS20);
	TIMSK |= (1<<OCIE2);

	MCUCR |= (1<<ISC01);
	GICR |= (1<<INT0);
	PORTD |= (1<<PD2);

	DDRC |= (1<<PC7);

	PORTC |= (1<<PC7);

#define SCK PB7
#define MOSI PB5
#define CS PB4



	//tab[0] = (int)(NULL);
	//if(tab[0]==(int)NULL) PORTB=0;

	DDRB |= (1<<CS)|(1<<MOSI)|(1<<SCK)|(1<<CS);
	PORTB |= (1<<CS);

	SPCR |= (1<<SPE)|(1<<MSTR);

	// Przerwanie INT0
	MCUCR |= (1<<ISC01);	// wyzwalanie zboczem opadaj¹cym
	GICR |= (1<<INT0);		// odblokowanie przerwania
	PORTD |= (1<<PD2);		// podci¹gniêcie pinu INT0 do VCC

	static char rxbuf[16];
	static FIL fil_obj;


	ir_init();
	lcd_init();


	lcd_defchar(0x80, SD_IN);
	lcd_defchar(0x81, SDOUT);
	lcd_defchar(0x82, SDCH1);
	lcd_defchar(0x83, SDCH2);

	//UART_init();
	sei();

	//fun(&tab[1]);

	char buf[5];
//	zapis[0] = 0;
//	TWI_write_buf(PCF8583_ADDR, 0, 1, buf);
//
//	zapis[0] = dec2bcd(0);
//	zapis[1] = dec2bcd(40);
//	zapis[2] = dec2bcd(10);
//	TWI_write_buf(PCF8583_ADDR, 2, 3, (uint8_t*)buf);

	TWI_read_buf(PCF8583_ADDR, 1, 4, (uint8_t*)buf);
	sekundy = bcd2dec( buf[1] );
	minuty = bcd2dec( buf[2] );
	godziny = bcd2dec( buf[3] );

	RTC rtc;
	rtc.hour = bcd2dec(buf[3]);
	rtc.min = bcd2dec(buf[2]);
	rtc.sec = bcd2dec(buf[1]);
	//rtc_settime(&rtc);

	LCD_ON;

	static char mbuf[40];



	_delay_ms(500);
	disk_status(STA_NOINIT);


	static FATFS FATFS_Obj;

	if( !(PINC & CD) )
	{
		lcd_cls();
		lcd_locate(0,13);
		lcd_str("\x82\x83\x80");
		lcd_locate(1,0);
		lcd_str("SD INIT wait...");
		_delay_ms(2000);
		lcd_cls();
		lcd_locate(0,13);
		lcd_str("\x82\x83\x80");
		disk_initialize(0);
		f_mount(0, &FATFS_Obj);

		// write
		//f_unlink("fox1");

//		f_open(&fil_obj, "beton",   FA_CREATE_ALWAYS | FA_WRITE );
//		f_puts("betonowy plik",&fil_obj);

		f_open(&fil_obj, "beton",   FA_WRITE );


		f_lseek(&fil_obj, fil_obj.fsize);
		f_puts("1",&fil_obj);


		//f_printf(&fil_obj, "LiczbaX %d", Timer);
		f_close(&fil_obj);
		f_mount(0, NULL);

		//while(1);

		uint8_t a;
		a=disk_initialize(0);
		if(a!=FR_OK)
		{
			lcd_cls();
			lcd_str("SD INIT error: ");
			lcd_int(a);
			_delay_ms(2000);
			lcd_cls();
		}

		a=f_mount(0, &FATFS_Obj);
		if(a!=FR_OK)
		{
			lcd_cls();
			lcd_str("SD MOUNT error: ");
			lcd_int(a);
			_delay_ms(3000);
			lcd_cls();
		}
		// read
		if(f_open(&fil_obj, "beton", FA_READ)==FR_OK)
		{
			if(f_gets(rxbuf,sizeof(rxbuf),&fil_obj)>0) beep(1);
			f_close(&fil_obj);
		}
		else beep(2);


		f_mount(0, NULL);
	}


	//beep(2);

	lcd_str_P(PSTR("Start..."));

	if( !(PINC & CD) )
	{
		lcd_locate(1,1);
		lcd_str("[");
		lcd_str(rxbuf);
		lcd_str("]");
		_delay_ms(2000);
		lcd_cls();
	}


	//ledy();

	//PORTA=0xFF;



	while(1)
	{

		if(in0_flag)
		{
			TWI_read_buf(PCF8583_ADDR, 1, 4, (uint8_t*)mbuf);

			sekundy = bcd2dec( mbuf[1] );
			minuty = bcd2dec( mbuf[2] );
			godziny = bcd2dec( mbuf[3] );

			// wyœwietlenie czasu na LCD
			lcd_locate(1,0);
			if( godziny < 10 ) lcd_str("0");
			lcd_int(godziny);
			lcd_str(":");
			if( minuty < 10 ) lcd_str("0");
			lcd_int(minuty);
			lcd_str(":");
			if( sekundy < 10 ) lcd_str("0");
			lcd_int(sekundy);

			rtc.hour=mbuf[3];
			rtc.min=mbuf[2];
			rtc.sec=mbuf[1];

			in0_flag=0;
		}

		if(!kom_on)
		{
			if( !(PINC & CD) )
			{
				lcd_locate(0,13);
				//lcd_str("SD Card IN SLOT");
				lcd_str("\x82\x83\x80");
				kom_on=1;
				//beep(20);
			}
		}


		if(kom_on)
		{
			if( (PINC & CD) )
			{
				lcd_locate(0,13);
				//lcd_str("SD Card OUT    ");
				lcd_str("\x82\x83\x81");
				kom_on=0;
				//beep(100);
			}
		}

		if(Ir_key_press_flag)
		{
			if(command==teletext_red )
			{
				//lcd_cls();
				lcd_locate(1,1);
				lcd_str_P(PSTR("Kasia           "));
				beep(20);
			}

			if(command==teletext_green )
			{
				//lcd_cls();
				lcd_locate(1,1);
				lcd_str_P(PSTR("           Mirek"));
				beep(30);
			}

			if(command==menu )
			{
				beep(5);
				LED_TOG;
				LCD_TOG;
			}

			if(command==1)
			{
				lcd_cls();
				lcd_str(TOSTRING(MCU));
				lcd_locate(2,1);
				lcd_str(TOSTRING(F_CPU));
				ledy();
				_delay_ms(2000);
				lcd_cls();
			}

			command=0xff;
			address=0xff;
			Ir_key_press_flag=0;
		}

	}

}


// konwersja liczby dziesiêtnej na BCD
uint8_t dec2bcd(uint8_t dec) {
return ((dec / 10)<<4) | (dec % 10);
}

// konwersja liczby BCD na dziesiêtn¹
uint8_t bcd2dec(uint8_t bcd) {
    return ((((bcd) >> 4) & 0x0F) * 10) + ((bcd) & 0x0F);
}


ISR(TIMER2_COMP_vect)
{
	Timer++;			/* Performance counter for this module */
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
}

ISR(INT0_vect)
{
	in0_flag=1;

}
