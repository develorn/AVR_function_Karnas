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


int main(
    void )
    __attribute__ (( noreturn ));
int main(
    void )

{

	DDRB = (1<<PB7);
	PORTB = (1<<PB7);

	lcd_init();

	//ADMUX = 0xEB;
	//ADMUX |= VREF_256;
	//ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	//ADCSRA |= (1<<ADSC)|(1<<ADATE);

	ADCSR = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

	static uint32_t sr=0, srv=0, v_dif=0;

	uint16_t czas=70;

	uint8_t kfil=50;

    uint16_t v1, v2;

    uint32_t value, valuev;

    div_t divmod;

	while(1)
	{

		ADMUX = 0b11000111;
		_delay_us(250);
		ADCSR |= (1<<ADSC);
		while( !(ADCSR & (1<<ADIF)) );
		valuev = ADCW;
		if(valuev < 5 || valuev > 1023) valuev = 0;

		srv=kfil*srv;
		srv=srv+valuev*25*1340;
		srv=srv/(kfil+1);


		ADMUX = 0b11001011;
		_delay_us(250);
		ADCSRA |= (1<<ADSC);
		while( !(ADCSRA & (1<<ADIF)) );
		value = ADCW;
		if(value < 25 || value > 511) value = 0;

		sr=kfil*sr;
		sr=sr+value*VREF_256_MUL*1082;
		sr=sr/(kfil+1);


		v_dif = (sr*2)/18;

		if(!czas) {

			lcd_locate(0,2);
			lcd_str("U = ");

			v1 = srv/1000000;
			divmod = div(srv/10000, 100);
			v2 = divmod.rem;

			if(v1>0 || (!v1 && !v2)) {
				lcd_str(int_to_str(v1, liczba, 2, '0'));
				lcd_char('.');
			} else lcd_char(' ');


			if(v1>0 || (!v1 && !v2)) {
				lcd_str(int_to_str(v2, liczba, 2, '0'));
				lcd_str(" V");
			} else {
				divmod = div(srv/1000, 1000);
				v2 = divmod.rem;
				lcd_str(int_to_str(v2, liczba, 3, '0'));
				lcd_str(" mV");
			}

			//--------------

			lcd_locate(1,2);
			lcd_str("I = ");
			v1 = v_dif/1000000;
			divmod = div(v_dif/1000, 1000);
			v2 = divmod.rem;

			if(v1>0 || (!v1 && !v2)) {
				lcd_str(int_to_str(v1, liczba, 1, '0'));
				lcd_char('.');
			} else lcd_char(' ');


			lcd_str(int_to_str(v2, liczba, 3, '0'));

			if(v1>0 || (!v1 && !v2)) lcd_str(" A");
			else lcd_str(" mA");


			czas=220;
		} else czas--;

		_delay_ms(1);

	}
}


