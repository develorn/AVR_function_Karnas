/*
 * main.c
 *
 *  Created on: 2010-09-13
 *       Autor: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/*
 *
 *	ODKOMENTUJ poni¿sz¹ liniê aby u¿yæ programowego SPI
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


void InitSpi(void);					/* inicjalizacja sprzêtowego SPI */
void InitSoftSpi(void);				/* inicjalizacja programowego SPI */

void SendSpi( uint8_t bajt );		/* wys³anie bajtu - sprzêtowe SPI */
void SendSoftSpi( uint8_t bajt );	/* wys³anie bajtu - programowe SPI */


/* definicja  zmiennej pomocniczej – licznik kroków/pozycja
 * zapalonej diody LED */
uint8_t cnt;

//**************************** g³ówna funkcja programu
int main(void) {
	/* inicjalizacja SPI */
	#ifndef USE_SOFT_SPI
		InitSpi();
	#else
		InitSoftSpi();
	#endif

	/* wysy³aj¹c zero dokonujemy programowego resetu rejestru */
	#ifndef USE_SOFT_SPI
		SendSpi( 0 );
	#else
		SendSoftSpi( 0 );
	#endif


	/* pêtla nieskoñczona */
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


	} /* koniec pêtli nieskoñczonej */
}

#ifndef USE_SOFT_SPI

void InitSpi(void) {
	/* ustawienie kierunku wyjœciowego dla linii MOSI, SCK i CS */
	DDRB |= (1<<MOSI)|(1<<SCK)|(1<<CS);
	/* aktywacja  SPI, tryb pracy Master, prêdkoœæ zegara Fosc/64 */
	SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR1);
}

/* definicja  funkcji wysy³aj¹cej bajt */
void SendSpi( uint8_t bajt ) {
	/* wysy³amy bajt do uk³adu Slave */
	SPDR = bajt;
	/* czekamy a¿ zostanie wys³any ostatni bit */
	while( !(SPSR & (1<<SPIF)) );

	/* zbocze narastaj¹ce sygna³u „Latch Clock” powoduje
	 * przepisanie wartoœci rejestru do wyjœæ Qa - Qh */
	PORTB |= (1<<CS);
	/* odczekanie 1 us – czas trwania impulsu LE */
	_delay_us(1);
	/* przywracamy stan niski na linii LE dziêki czemu
	 * podczas wysy³ania nastêpnego bajtu nie bêd¹ widoczne zmiany
	 * na wyjœciach Qa-Qh podczas przesuwania siê rejestru do czasu
	 * ponownego zatrzaœniêcia ca³ego bajtu */
	PORTB &= ~(1<<CS);
}
#else

void InitSoftSpi(void) {
	/* ustawienie kierunku wyjœciowego dla linii MOSI, SCK i CS */
	DDRB |= (1<<SMOSI)|(1<<SSCK)|(1<<SCS);
}

/* definicja  funkcji wysy³aj¹cej bajt */
void SendSoftSpi( uint8_t bajt ) {
	uint8_t cnt=0x80;

	SCK_0;
	/* wysy³amy bajt do uk³adu Slave */
	while( cnt ) {
		if( bajt & cnt ) MOSI_1;
		else MOSI_0;
		SCK_1;
		SCK_0;
		cnt>>=1;
	}
	/* czekamy a¿ zostanie wys³any ostatni bit */

	/* zbocze narastaj¹ce sygna³u „Latch Clock” powoduje
	 * przepisanie wartoœci rejestru do wyjœæ Qa - Qh */
	PORTB |= (1<<SCS);
	/* odczekanie 1 us – czas trwania impulsu LE */
	_delay_us(1);
	/* przywracamy stan niski na linii LE dziêki czemu
	 * podczas wysy³ania nastêpnego bajtu nie bêd¹ widoczne zmiany
	 * na wyjœciach Qa-Qh podczas przesuwania siê rejestru do czasu
	 * ponownego zatrzaœniêcia ca³ego bajtu */
	PORTB &= ~(1<<SCS);
}

#endif






