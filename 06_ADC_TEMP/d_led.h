/*
 * d_led.h
 *
 *  Created on: 2010-03-30
 *       Autor: Miros�aw Karda�
 */

#ifndef _d_led_h		// dyrektywy zabezpieczaj�ce przed wielokrotnym do��czaniem
#define _d_led_h		// tego samego pliku nag��wkowego je�li b�dzie do��czany
				// w wielu r�nych plikach programu

// definicje port�w i pin�w wy�wietlacza u�ywanych dla u�atwienia w programie jako sta�e preprocesora
#define 	LED_DATA 	PORTC	// port z pod��czonymi segmentami
#define		LED_DATA_DIR DDRC	// rejestr kierunku portu katod wy�wietlaczy
#define 	LED_ANODY 	PORTD	// port z pod��czonymi anodami- 4 bity najm�odsze
#define 	ANODY_DIR 	DDRD	// rejestr kierunku portu anod wy�wietlaczy


#define 	CA1 	(1<<PD0)		// CA1 oznacza bit nr.0 portu D
#define 	CA2 	(1<<PD1)		// CA2 oznacza bit nr.1 portu D
#define 	CA3 	(1<<PD2)		// CA3 oznacza bit nr.2 portu D
#define 	CA4 	(1<<PD3)		// CA4 oznacza bit nr.3 portu D

// definicje bit�w dla poszczeg�lnych segment�w LED
#define SEG_A (1<<0)
#define SEG_B (1<<1)
#define SEG_C (1<<2)
#define SEG_D (1<<3)
#define SEG_E (1<<4)
#define SEG_F (1<<5)
#define SEG_G (1<<6)
#define SEG_DP (1<<7)

#define STOPIEN 16
#define NIC 17
#define MINUS 18

// same DEKLARACJE zmiennych globalnych na potrzeby wykorzystania ich w innych plikach projektu
// przydomek externpowoduje, i� te zmienne b�d� dost�pne we wszystkich modu�ach, kt�re do��cz� plik
// d_led.h za pomoc� dyrektywy #include
extern volatile uint8_t cy1;
extern volatile uint8_t cy2;
extern volatile uint8_t cy3;
extern volatile uint8_t cy4;



void d_led_init(void);

#endif	// koniec _d_led_h

