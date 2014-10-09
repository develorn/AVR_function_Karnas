/*------------------------------------------------*/
/* UART functions                                 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

//#define	SYSCLK		9216000
#define	BAUD		115200


typedef struct _fifo {
	uint8_t	idx_w;
	uint8_t	idx_r;
	uint8_t	count;
	uint8_t buff[64];
} FIFO;


static volatile
FIFO txfifo, rxfifo;



/* Initialize UART */

void uart_init()
{
	rxfifo.idx_r = 0;
	rxfifo.idx_w = 0;
	rxfifo.count = 0;
	txfifo.idx_r = 0;
	txfifo.idx_w = 0;
	txfifo.count = 0;

	UBRRL = F_CPU/BAUD/16-1;
	UCSRB = _BV(RXEN)|_BV(RXCIE)|_BV(TXEN);
}


/* Get a received character */

uint8_t uart_test ()
{
	return rxfifo.count;
}


uint8_t uart_get ()
{
	uint8_t d, i;


	i = rxfifo.idx_r;
	while(rxfifo.count == 0);
	d = rxfifo.buff[i++];
	cli();
	rxfifo.count--;
	sei();
	if(i >= sizeof(rxfifo.buff))
		i = 0;
	rxfifo.idx_r = i;

	return d;
}


/* Put a character to transmit */

void uart_put (uint8_t d)
{
	uint8_t i;


	i = txfifo.idx_w;
	while(txfifo.count >= sizeof(txfifo.buff));
	txfifo.buff[i++] = d;
	cli();
	txfifo.count++;
	UCSRB = _BV(RXEN)|_BV(RXCIE)|_BV(TXEN)|_BV(UDRIE);
	sei();
	if(i >= sizeof(txfifo.buff))
		i = 0;
	txfifo.idx_w = i;
}


/* UART RXC interrupt */

SIGNAL(SIG_UART_RECV)
{
	uint8_t d, n, i;


	d = UDR;
	n = rxfifo.count;
	if(n < sizeof(rxfifo.buff)) {
		rxfifo.count = ++n;
		i = rxfifo.idx_w;
		rxfifo.buff[i++] = d;
		if(i >= sizeof(rxfifo.buff))
			i = 0;
		rxfifo.idx_w = i;
	}
}


/* UART UDRE interrupt */

SIGNAL(SIG_UART_DATA)
{
	uint8_t n, i;


	n = txfifo.count;
	if(n) {
		txfifo.count = --n;
		i = txfifo.idx_r;
		UDR = txfifo.buff[i++];
		if(i >= sizeof(txfifo.buff))
			i = 0;
		txfifo.idx_r = i;
	}
	if(n == 0)
		UCSRB = _BV(RXEN)|_BV(RXCIE)|_BV(TXEN);
}

