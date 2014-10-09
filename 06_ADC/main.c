/*
 * main.c
 *
 *  Created on: 2010-04-21
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "LCD/lcd44780.h"


#define VREF_VCC (1<<REFS0)
#define VREF_256 (1<<REFS1)|(1<<REFS0)

#define VREF_VCC_MUL 49 //(uint16_t)((4.83*1000000)/1024)
#define VREF_256_MUL 25 //(uint16_t)((2.56*10000)/1024)


volatile uint32_t value;

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
   } while ( (fw>0));

   if(subzero) *strp++ = '-';			// jeœli by³a to liczba ujemna, wstaw znak minus
   *strp = 0;                     		// zakoñcz ³añcuch zerem

   // w zwi¹zku z tym, ¿e w ³añcuchu jest odwrócona kolejnoœæ cyfr
   // wykonaj ich zamianê
   strrev(str);

   // zwróæ wskaŸnik do pocz¹tku ³añucha z liczb¹
   return str;
}



int main(void)
{
	lcd_init();

	DDRD |= (1<<PD3)|(1<<PD4);
	PORTD |= (1<<PD3);

	PORTD &= ~(1<<PD4);

	ADMUX = 7;
	ADMUX |= VREF_256;
	//ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	//ADCSRA |= (1<<ADSC)|(1<<ADATE);

	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);



	sei();

	lcd_cls();
	lcd_str("start...");
	_delay_ms(1000);


	uint32_t v, sr=0, m=0;

	uint32_t licz=0;

	uint8_t czas=5;

	static uint8_t idx=0;


	while(1)
	{

		uint8_t kfil=4;

	    uint16_t v1, v2, v3, v4;

	    long int v2a;


		ADCSRA |= (1<<ADSC);
		while( !(ADCSRA & (1<<ADSC)) );
		value = ADCW;

		sr=kfil*sr;
		sr=sr+value*VREF_256_MUL*1194;
		sr=sr/(kfil+1);

		v=sr;


		if(!czas) {
			lcd_locate(0,0);
			if(v1 || v2)	lcd_str("+");
			else lcd_str(" ");
			v1 = v/1000000;
			lcd_int( v1 );
			lcd_str(".");

			div_t divmod = div(v/1000, 1000);

			v2 = divmod.rem;


			lcd_str(int_to_str(v2, liczba, 3, '0'));

	//		v2 = (v/100000)%10UL;
	//		lcd_int( v2 );
	//		v3 = (v/10000UL)%10UL;
	//		lcd_int( v3 );
	//		v4 = (v/1000UL)%10UL;
	//		lcd_int( v4 );

			lcd_str(" V   ");

			lcd_locate(1,0);
			//lcd_str(long_int_to_str(v, liczba, 9, ' '));
			czas=7;
		} else czas--;

		//lcd_locate(1,0);
		//lcd_int( value );
		//lcd_str("   ");
		_delay_ms(100);


		PORTD |= (1<<PD4);
		_delay_ms(1);
		PORTD &= ~(1<<PD4);



	}
}

ISR(ADC_vect)
{
	//value = ADCW;

}
