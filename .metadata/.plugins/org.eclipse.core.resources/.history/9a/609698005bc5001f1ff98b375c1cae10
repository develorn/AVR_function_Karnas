;---------------------------------------------------------------------------;
; USI control functions
;---------------------------------------------------------------------------;

.nolist
#include <avr/io.h>	// Include device specific definitions.
.list


;---------------------------------------------------------------------------;
; Receive a byte (28 clks)
;
; BYTE rcv_spi (void);

.global rcv_spi
.func rcv_spi
rcv_spi:
	ldi	r24, 0xFF
	; Link next function
.endfunc


;---------------------------------------------------------------------------;
; Transmit a byte (27 clks)
;
; void xmit_spi (BYTE);

.global xmit_spi
.func xmit_spi
xmit_spi:
	out	_SFR_IO_ADDR(USIDR), r24

	ldi	r24, 0b000100		; PB2(SCK)
	out	_SFR_IO_ADDR(PINB), r24	; Toggle SCK 16 times
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	out	_SFR_IO_ADDR(PINB), r24
	nop
	in	r24, _SFR_IO_ADDR(USIDR)
	reti
.endfunc


