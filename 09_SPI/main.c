/*
 * main.c
 *
 *  Created on: 2010-09-13
 *       Autor: Miros�aw Karda�
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/*
 *
 *	ODKOMENTUJ poni�sz� lini� aby u�y� programowego SPI
 *
 */
//#define USE_SOFT_SPI


/* hardware SPI */
#define MOSI PB5	//   <---- A (SER IN)
#define SCK PB7		//   <---- SHIFT CLOCK (SC)
#define CS PB4		//	 <---- LATCH CLOCK (LC)

/* software SPI */
#define SMOSI PB0 	//   <---- A (SER IN)
#define SSCK PB1	//   <---- SHIFT CLOCK (SC)
#define SCS PB2		//	 <---- LATCH CLOCK (LC)
#define SCK_0 PORTB &= ~(1<<SSCK)
#define SCK_1 PORTB |= (1<<SSCK)
#define MOSI_0 PORTB &= ~(1<<SMOSI)
#define MOSI_1 PORTB |= (1<<SMOSI)


void InitSpi(void);					/* inicjalizacja sprz�towego SPI */
void InitSoftSpi(void);				/* inicjalizacja programowego SPI */

void SendSpi( uint8_t bajt );		/* wys�anie bajtu - sprz�towe SPI */
void SendSoftSpi( uint8_t bajt );	/* wys�anie bajtu - programowe SPI */


/* definicja  zmiennej pomocniczej � licznik krok�w/pozycja
 * zapalonej diody LED */
uint8_t cnt;

//**************************** g��wna funkcja programu
int main(void) {
	/* inicjalizacja SPI */
	#ifndef USE_SOFT_SPI
		InitSpi();
	#else
		InitSoftSpi();
	#endif

	/* wysy�aj�c zero dokonujemy programowego resetu rejestru */
	#ifndef USE_SOFT_SPI
		SendSpi( 0 );
	#else
		SendSoftSpi( 0 );
	#endif


	/* p�tla niesko�czona */
	while(1) {
		cnt=1;
		while(cnt) {
			#ifndef USE_SOFT_SPI
				SendSpi( cnt );
			#else
				SendSoftSpi( cnt );
			#endif
			_delay_ms(100);
			cnt <<= 1;
		}

		cnt=0x80;
		while(cnt) {
			#ifndef USE_SOFT_SPI
				SendSpi( cnt );
			#else
				SendSoftSpi( cnt );
			#endif
			_delay_ms(100);
			cnt >>= 1;
		}


	} /* koniec p�tli niesko�czonej */
}

#ifndef USE_SOFT_SPI

void InitSpi(void) {
	/* ustawienie kierunku wyj�ciowego dla linii MOSI, SCK i CS */
	DDRB |= (1<<MOSI)|(1<<SCK)|(1<<CS);
	/* aktywacja  SPI, tryb pracy Master, pr�dko�� zegara Fosc/64 */
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1);
}

/* definicja  funkcji wysy�aj�cej bajt */
void SendSpi( uint8_t bajt ) {
	/* wysy�amy bajt do uk�adu Slave */
	SPDR = bajt;
	/* czekamy a� zostanie wys�any ostatni bit */
	while( !(SPSR & (1<<SPIF)) );

	/* zbocze narastaj�ce sygna�u �Latch Clock� powoduje
	 * przepisanie warto�ci rejestru do wyj�� Qa - Qh */
	PORTB |= (1<<CS);
	/* odczekanie 1 us � czas trwania impulsu LE */
	_delay_us(1);
	/* przywracamy stan niski na linii LE dzi�ki czemu
	 * podczas wysy�ania nast�pnego bajtu nie b�d� widoczne zmiany
	 * na wyj�ciach Qa-Qh podczas przesuwania si� rejestru do czasu
	 * ponownego zatrza�ni�cia ca�ego bajtu */
	PORTB &= ~(1<<CS);
}
#else

void InitSoftSpi(void) {
	/* ustawienie kierunku wyj�ciowego dla linii MOSI, SCK i CS */
	DDRB |= (1<<SMOSI)|(1<<SSCK)|(1<<SCS);
}

/* definicja  funkcji wysy�aj�cej bajt */
void SendSoftSpi( uint8_t bajt ) {
	uint8_t cnt=0x80;

	SCK_0;
	/* wysy�amy bajt do uk�adu Slave */
	while( cnt ) {
		if( bajt & cnt ) MOSI_1;
		else MOSI_0;
		SCK_1;
		SCK_0;
		cnt>>=1;
	}
	/* czekamy a� zostanie wys�any ostatni bit */

	/* zbocze narastaj�ce sygna�u �Latch Clock� powoduje
	 * przepisanie warto�ci rejestru do wyj�� Qa - Qh */
	PORTB |= (1<<SCS);
	/* odczekanie 1 us � czas trwania impulsu LE */
	_delay_us(1);
	/* przywracamy stan niski na linii LE dzi�ki czemu
	 * podczas wysy�ania nast�pnego bajtu nie b�d� widoczne zmiany
	 * na wyj�ciach Qa-Qh podczas przesuwania si� rejestru do czasu
	 * ponownego zatrza�ni�cia ca�ego bajtu */
	PORTB &= ~(1<<SCS);
}

#endif






