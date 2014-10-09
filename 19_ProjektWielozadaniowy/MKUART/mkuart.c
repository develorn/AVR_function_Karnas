/*
 * mkuart.c
 *
 *  Created on: 2010-09-04
 *       Autor: Miros≈Çaw Karda≈õ
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


#include "mkuart.h"


// definiujemy w koÒcu nasz bufor UART_RxBuf
volatile char UART_RxBuf[UART_RX_BUF_SIZE];
// definiujemy indeksy okreúlajπce iloúÊ danych w buforze
volatile uint8_t UART_RxHead; // indeks oznaczajπcy Ñg≥owÍ wÍøaî
volatile uint8_t UART_RxTail; // indeks oznaczajπcy Ñogon wÍøaî



// definiujemy w koÒcu nasz bufor UART_RxBuf
volatile char UART_TxBuf[UART_TX_BUF_SIZE];
// definiujemy indeksy okreúlajπce iloúÊ danych w buforze
volatile uint8_t UART_TxHead; // indeks oznaczajπcy Ñg≥owÍ wÍøaî
volatile uint8_t UART_TxTail; // indeks oznaczajπcy Ñogon wÍøaî






void USART_Init( uint16_t baud ) {
	/* Ustawienie prÍdkoúci */
	UBRRH = (uint8_t)(baud>>8);
	UBRRL = (uint8_t)baud;
	/* Za≥πczenie nadajnika I odbiornika */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Ustawienie format ramki: 8bitÛw danych, 1 bit stopu */
	UCSRC = (1<<URSEL)|(3<<UCSZ0);

	// jeúli korzystamy z interefejsu RS485
	#ifdef UART_DE_PORT
		// inicjalizujemy liniÍ sterujπcπ nadajnikiem
		UART_DE_DIR |= UART_DE_BIT;
		UART_DE_ODBIERANIE;
	#endif

	// jeúli korzystamy z interefejsu RS485
	#ifdef UART_DE_PORT
		// jeúli korzystamy z interefejsu RS485 za≥πczamy dodatkowe przerwanie TXCIE
		UCSRB |= (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
	#else
		// jeúli nie  korzystamy z interefejsu RS485
		UCSRB |= (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	#endif
}

// procedura obs≥ugi przerwania Tx Complete, gdy zostanie opÛøniony UDR
// kompilacja gdy uøywamy RS485
#ifdef UART_DE_PORT
ISR( USART_TXC_vect ) {
  UART_DE_PORT &= ~UART_DE_BIT;	// zablokuj nadajnik RS485
}
#endif


// definiujemy funkcjÍ dodajπcπ jeden bajtdoz bufora cyklicznego
void uart_putc( char data ) {
	uint8_t tmp_head;

    tmp_head  = (UART_TxHead + 1) & UART_TX_BUF_MASK;

          // pÍtla oczekuje jeøeli brak miejsca w buforze cyklicznym na kolejne znaki
    while ( tmp_head == UART_TxTail ){}

    UART_TxBuf[tmp_head] = data;
    UART_TxHead = tmp_head;

    // inicjalizujemy przerwanie wystÍpujπce, gdy bufor jest pusty, dziÍki
    // czemu w dalszej czÍúci wysy≥aniem danych zajmie siÍ juø procedura
    // obs≥ugi przerwania
    UCSRB |= (1<<UDRIE);
}


void uart_puts(char *s)		// wysy≥a ≥aÒcuch z pamiÍci RAM na UART
{
  register char c;
  while ((c = *s++)) uart_putc(c);			// dopÛki nie napotkasz 0 wysy≥aj znak
}

void uart_putint(int value, int radix)	// wysy≥a na port szeregowy tekst
{
	char string[17];			// bufor na wynik funkcji itoa
	itoa(value, string, radix);		// konwersja value na ASCII
	uart_puts(string);			// wyúlij string na port szeregowy
}


// definiujemy procedurÍ obs≥ugi przerwania nadawczego, pobierajπcπ dane z bufora cyklicznego
ISR( USART_UDRE_vect) {
    // sprawdzamy czy indeksy sπ rÛøne
    if ( UART_TxHead != UART_TxTail ) {
    	// obliczamy i zapamiÍtujemy nowy indeks ogona wÍøa (moøe siÍ zrÛwnaÊ z g≥owπ)
    	UART_TxTail = (UART_TxTail + 1) & UART_TX_BUF_MASK;
    	// zwracamy bajt pobrany z bufora  jako rezultat funkcji
    	UDR = UART_TxBuf[UART_TxTail];
    } else {
	// zerujemy flagÍ przerwania wystÍpujπcego gdy bufor pusty
	UCSRB &= ~(1<<UDRIE);
    }
}


// definiujemy funkcjÍ pobierajπcπ jeden bajt z bufora cyklicznego
char uart_getc(void) {
    // sprawdzamy czy indeksy sπ rÛwne
    if ( UART_RxHead == UART_RxTail ) return 0;

    // obliczamy i zapamiÍtujemy nowy indeks Ñogona wÍøaî (moøe siÍ zrÛwnaÊ z g≥owπ)
    UART_RxTail = (UART_RxTail + 1) & UART_RX_BUF_MASK;
    // zwracamy bajt pobrany z bufora  jako rezultat funkcji
    return UART_RxBuf[UART_RxTail];
}

// definiujemy procedurÍ obs≥ugi przerwania odbiorczego, zapisujπcπ dane do bufora cyklicznego
ISR( USART_RXC_vect ) {
    uint8_t tmp_head;
    char data;

    data = UDR; //pobieramy natychmiast bajt danych z bufora sprzÍtowego

    // obliczamy nowy indeks Ñg≥owy wÍøaî
    tmp_head = ( UART_RxHead + 1) & UART_RX_BUF_MASK;

    // sprawdzamy, czy wπø nie zacznie zjadaÊ w≥asnego ogona
    if ( tmp_head == UART_RxTail ) {
    	// tutaj moøemy w jakiú wygodny dla nas sposÛb obs≥uøyÊ  b≥πd spowodowany
    	// prÛbπ nadpisania danych w buforze, mog≥oby dojúÊ do sytuacji gdzie
    	// nasz wπø zaczπ≥by zjadaÊ w≥asny ogon
    } else {
	UART_RxHead = tmp_head; 		// zapamiÍtujemy nowy indeks
	UART_RxBuf[tmp_head] = data; 	// wpisujemy odebrany bajt do bufora
    }
}

