/*
 * main.c
 *
 *  Created on: 2010-04-21
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "LCD/lcd44780.h"
#include "d_led.h"

#include "1wire/onewire.h"
#include "1wire/ds18x20.h"
#include "temperatura.h"
#include "temp_inline.h"


#define VREF_VCC (1<<REFS0)
#define VREF_256 (1<<REFS1)|(1<<REFS0)

#define VREF_VCC_MUL 49 //(uint16_t)((4.83*1000000)/1024)
#define VREF_256_MULv 25 //(uint16_t)((2.56*10000)/1024)

#define VREF_256_MUL 4 //(uint16_t)((2.56*10000)/512)




//#define buf_cnt 15
//volatile uint32_t buf[buf_cnt];

uint8_t init=0;

char liczba[10];


char *int_to_str(int val, char *str, int8_t fw, char znak_wiodacy) {
	char *strp = str;
	uint8_t subzero = 0;

	if(val<0) {			// jeœli liczba jest ujemna
		val = ~val+1;	// zaneguj i koryguj
		subzero=1;		// ustaw znacznik na 1
		fw--;
	}

   do{
      div_t divmod = div(val, 10);   	// opracja dzielenia oraz modulo - wynik do struktury ldiv_t
      //*strp++ = divmod.rem + '0';   	// wstawianie cyfr od najmniej znacz¹cej do ³añcucha

      if((val == 0) && (strp != str)) {
         *strp++ = znak_wiodacy;
      } else {
         *strp++ = divmod.rem + '0';
      }


      val = divmod.quot;            	// wartoœæ zmniejsza siê o jednostki, dziesi¹tki, setki itd
      fw--;                     		// zmniejszenie licznika szerokoœci formatowanego pola
      // wykonuj pêtlê do momentu sprawdzenia ostatniej cyfry znacz¹cej lub zajêtoœci ca³ego pola
   } while ( (fw>0)  );

   if(subzero) *strp++ = '-';			// jeœli by³a to liczba ujemna, wstaw znak minus
   *strp = 0;                     		// zakoñcz ³añcuch zerem

   // w zwi¹zku z tym, ¿e w ³añcuchu jest odwrócona kolejnoœæ cyfr
   // wykonaj ich zamianê
   strrev(str);

   // zwróæ wskaŸnik do pocz¹tku ³añucha z liczb¹
   return str;
}

//char * long_int_to_str(long int val, char *str, int8_t fw, char znak_wiodacy) {
//	char *strp = str;
//	uint8_t subzero = 0;
//
//	if(val<0) {			// jeœli liczba jest ujemna
//		val = ~val+1;	// zaneguj i koryguj
//		subzero=1;		// ustaw znacznik na 1
//		fw--;
//	}
//
//   do{
//      ldiv_t divmod = ldiv(val, 10);   	// opracja dzielenia oraz modulo - wynik do struktury ldiv_t
//      //*strp++ = divmod.rem + '0';   	// wstawianie cyfr od najmniej znacz¹cej do ³añcucha
//
//      if((val == 0) && (strp != str)) {
//         *strp++ = znak_wiodacy;
//      } else {
//         *strp++ = divmod.rem + '0';
//      }
//
//
//      val = divmod.quot;            	// wartoœæ zmniejsza siê o jednostki, dziesi¹tki, setki itd
//      fw--;                     		// zmniejszenie licznika szerokoœci formatowanego pola
//      // wykonuj pêtlê do momentu sprawdzenia ostatniej cyfry znacz¹cej lub zajêtoœci ca³ego pola
//   } while ( (fw>0) );
//
//   if(subzero) *strp++ = '-';			// jeœli by³a to liczba ujemna, wstaw znak minus
//   *strp = 0;                     		// zakoñcz ³añcuch zerem
//
//   // w zwi¹zku z tym, ¿e w ³añcuchu jest odwrócona kolejnoœæ cyfr
//   // wykonaj ich zamianê
//   strrev(str);
//
//   // zwróæ wskaŸnik do pocz¹tku ³añucha z liczb¹
//   return str;
//}

#ifndef ADCSR
#define ADCSR ADCSRA
#endif

uint8_t lit_e[] = {32,32,14,17,31,16,14,1};
uint8_t lit_a[] = {32,32,14,1,15,17,14,1};

int main(
    void )
    __attribute__ (( noreturn ));
int main(
    void )

{

	DDRB |= (1<<PB7);

	PORTB |= (1<<PB7);


	lcd_init();
	d_led_init();
	init_t_sensor();
	pomiar_t();

	cy1=MINUS;
	cy2=MINUS;
	cy3=MINUS;
	cy4=MINUS;

	sei();

//while(1);

	//ADMUX = 0xEB;
	//ADMUX |= VREF_256;
	//ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	//ADCSRA |= (1<<ADSC)|(1<<ADATE);

	ADCSR = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

	static uint32_t sr=0, srv=0, v=0, v_dif=0;

	uint16_t czas=70;

	uint8_t kfil=90;

    uint16_t v1, v2;

    uint32_t value, valuev;

    div_t divmod;

    uint8_t tl=0;
    uint16_t tcnt=0;

    sei();

    lcd_defchar(0x80, lit_a);
    lcd_defchar(0x81, lit_e);

    lcd_cls();
    lcd_str("Mirenik napi""\x81""cia,");
	lcd_locate(1,0);
	lcd_str("pr""\x80""du + DS18B20");
	_delay_ms(100);

	odczyt_t();
	cy1=get_temp()/100;
	cy2=(get_temp()/10)%10;
	cy3=STOPIEN;
	cy4=12;
	_delay_ms(100);

    lcd_cls();
	lcd_str("Napi""\x81""cie   0-32V");
	lcd_locate(1,0);
	lcd_str("Pr""\x80""d     10ma-5A");
	_delay_ms(100);



	_delay_ms(100);
	lcd_cls();

	uint8_t vt1=0, vt2=0;
	uint32_t ala=0, ola=0;

	while(1)
	{

		/*   P O M I A R    N A P I Ê C I A     0 - 32v
		 *   schemat dzielnika rezystorowego dla ATmega32
				   ____		         ____
		GND |-----|____|----+-------|____|------O pomiar napiêcia
					 10K	|		 120K
							|
				ADCx O-------

		*/
		ADMUX = 0b11000111;
		_delay_us(450);
		ADCSR |= 7;
		ADCSR |= (1<<ADSC);
		while( (ADCSR & (1<<ADSC)) );
		valuev = ADCW;
		ala+=ADCW;
		//if(valuev < 5 || valuev > 1023) valuev = 0;

		srv=kfil*srv;
		srv=srv+valuev*25*1300;//*3225;//*1340;
		srv=srv/(kfil+1);

		v=srv;
		if(valuev) {
			if(vt1<50) vt1++;
		} else
			if(vt1) vt1--;

		if(!vt1) v=0;

		if(32 == vt1) {
			vt1=0;
			ala>>=5;
			v=ala;
		}

		vt1++;


		/*   P O M I A R    P R ¥ D U    0 - 5A
		 *   schemat dzielnika rezystorowego dla ATmega32
					 ____		     ____
		GND |-------|____|--+-------|____|------O pomiar na 0R1
					 22K	|		  22K
							|
		ADC1	Vpos O-------

		ADC0	Vneg O------|
							|
							|
						   ---
						   GND
		*/
		ADMUX = 0b11001001;		// -----> wzmocnienie x10
		//ADMUX = 0b11000001;
		_delay_us(450);
		ADCSRA |= (1<<ADSC);
		while( (ADCSRA & (1<<ADSC)) );
		value = ADCW;
		ola+=ADCW;


//		lcd_locate(0,0);
//		lcd_str(int_to_str(value, liczba, 3, '0'));
//		_delay_ms(100);



		//if(value<20 && value ) value-=5;

		//if(value<10 && value ) value+=1;

		//if(value<1 || value > 511) value=0;

//		if(value>100) value-=15;
//		else
//		if(value>3) value-=3;

		//if(value<3) value=0;


		sr=kfil*sr;
		//sr=sr+((value-((250-value)/20))*115*170);
		//sr=sr+((value)*25*5500);
		//sr=sr+((value+(value/2))*25*1500);
		//sr=sr+(value*25*200);
		//sr=sr+ ( value * 25 * 1800  );

		sr=sr+ ( ((256*value*100)/(512))*200 );

		sr=sr/(kfil+1);
		v_dif=sr;



		if(64 == vt2) {
			vt2=0;
			ola/=64;
			v_dif=ola;
		}

		vt2++;



//		if(value>5) {
//			if(vt2<10) vt2++;
//		} else
//			if(vt2) vt2--;
//
//		if(!vt2) v_dif=0;

		if(!czas) {

			lcd_locate(0,2);
			lcd_str("U = ");

			v1 = v/1000000;
			divmod = div(v/10000, 100);
			v2 = divmod.rem;

			lcd_str(int_to_str(v1, liczba, 2, '0'));
			lcd_char('.');
			lcd_str(int_to_str(v2, liczba, 2, '0'));
			lcd_str(" V");

			//--------------

			lcd_locate(1,2);
			lcd_str("I = ");
			v1 = v_dif/1000000;
			divmod = div(v_dif/1000, 1000);
			v2 = divmod.rem;

			lcd_str(int_to_str(v1, liczba, 1, '0'));
			lcd_char('.');
			lcd_str(int_to_str(v2, liczba, 3, '0'));
			lcd_str(" A");

			czas=220;
		} else czas--;

		_delay_ms(1);

		tcnt++;
		if(tcnt>550)
		{
			if(!tl) {
				tl=1;
				pomiar_t();
				lcd_locate(1,15);
				lcd_char('.');
			} else {
				tl=0;
				odczyt_t();
				lcd_locate(1,15);
				lcd_char(' ');
				if(get_temp()>=0) {
					if( (get_temp()/100) > 0 ) cy1=get_temp()/100;
					else cy1=NIC;
					cy2=(get_temp()/10)%10;
					cy3=STOPIEN;
					cy4=12;
				} else {
					if( (get_temp()) <-99 ) {
						cy1=MINUS;
						cy2=(get_temp()*-1)/100;
						cy3=((get_temp()*-1)/10)%10;
						cy4=STOPIEN;
					} else {
						cy1=MINUS;
						cy2=( (get_temp()*-1)/10)%10;
						cy3=STOPIEN;
						cy4=12;
					}

				}
			}
			tcnt=0;

			//PORTD ^= (1<<PD7);
		}

	}
}


