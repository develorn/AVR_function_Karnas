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

//#include "SW_RS_ELM_CHAN/suart.h"


#define VREF_VCC (1<<REFS0)
#define VREF_256 (1<<REFS1)|(1<<REFS0)

#define VREF_VCC_MUL 49 //(uint16_t)((4.83*1000000)/1024)
#define VREF_256_MULv 25 //(uint16_t)((2.56*10000)/1024)

#define VREF_256_MUL 45 //(uint16_t)((2.56*10000)/512)

//void put_str(char *s);

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




int main(
    void )
    __attribute__ (( noreturn ));
int main(
    void )

{

	DDRA=0;
	PORTA=0;

	_delay_ms(100);
	lcd_init();
	lcd_cls();
	lcd_str_P(PSTR("  Miernik  U,I  "));
	lcd_locate(1,0);
	lcd_str_P(PSTR("(C)2010 mirekk36"));
	_delay_ms(200);
	lcd_cls();

	//ADMUX = 0xEB;
	//ADMUX |= VREF_256;
	//ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	//ADCSRA |= (1<<ADSC)|(1<<ADATE);

	ADCSR = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);






	static uint32_t sr=0, srv=0, v=0, v_dif=0;



	uint16_t czas=2;


	uint8_t kfil=90;

    uint16_t v1, v2;

    uint8_t vt1=0, vt2=0;

    uint32_t value=0, valuev=0;

    div_t divmod;


	while(1)
	{

		/*   P O M I A R    N A P I Ê C I A     0 - 32v
		 *   schemat dzielnika rezystorowego dla ATTiny26
				   ____		         ____
		GND |-----|____|----+-------|____|------O pomiar napiêcia
					 10K	|		 120K
							|
				ADCx O-------

		*/
		ADMUX = 0b11000110;
		_delay_us(450);
		ADCSR |= (1<<ADSC);
		while( !(ADCSR & (1<<ADIF)) );
		valuev = ADCW;


		srv=kfil*srv;
		srv=srv+valuev*25*1400;
		srv=srv/(kfil+1);

		v=srv;
		if(valuev) {
			if(vt1<50) vt1++;
		} else
			if(vt1) vt1--;

		if(!vt1) v=0;



		/*   P O M I A R    P R ¥ D U    0 - 5A
		 *   schemat dzielnika rezystorowego dla ATtiny26
					 ____		     ____
		GND |-------|____|--+-------|____|------O pomiar na 0R1
					 22K	|		  68K
							|
		ADC0	Vpos O-------

		ADC1	Vneg O------|
							|
							|
						   ---
						   GND
		*/
		ADMUX = 0b11001011;		// -----> wzmocnienie x20
		_delay_us(450);
		ADCSR |= (1<<ADSC);
		while( (ADCSR & (1<<ADSC)) );
		value = ADCW;
		if(value>3) value-=3;

		//if(value<3) value=0;


		//if(value<4 && value ) value+=1;

		//if(value<10 && value ) value+=1;


		sr=kfil*sr;
		//sr=sr+((value-(value/150))*25*202);
		//sr=sr+((value*25*200));
		sr=sr+ ( ((256*value*100)/(2*1024))*450 );
		sr=sr/(kfil+1);

		v_dif=sr;

		if(value>4) {
			if(vt2<5) vt2++;
		} else
			if(vt2) vt2--;

		if(!vt2) v_dif=0;



		if(!czas) {

			lcd_locate(0,2);
			lcd_str("U = ");

			//put_str("Napiecie: ");

			v1 = v/1000000;
			divmod = div(v/10000, 100);
			v2 = divmod.rem;

			lcd_str(int_to_str(v1, liczba, 2, '0'));
			//put_str(liczba);
			//put_str(".");
			lcd_char('.');

			//lcd_str(int_to_str(v2, liczba, 2, '0'));
			lcd_str(int_to_str(value, liczba, 3, '0'));

			lcd_str(" V");
			//put_str(liczba);
			//put_str(" V          ");


			//--------------

			lcd_locate(1,2);
			lcd_str("I = ");
			v1 = v_dif/1000000;
			divmod = div(v_dif/1000, 1000);
			v2 = divmod.rem;

			//put_str("Prad: ");
			lcd_str(int_to_str(v1, liczba, 1, ' '));
			lcd_char('.');
			//put_str(liczba);
			//put_str(".");
			lcd_str(int_to_str(v2, liczba, 3, '0'));
			lcd_str(" A");
			//put_str(liczba);
			//put_str(" A\r\n");


			czas=320;
		} else czas--;


		_delay_ms(1);

	}
}

//void put_str(char *s) {
//	while( *s ) xmit(*s++);
//}



