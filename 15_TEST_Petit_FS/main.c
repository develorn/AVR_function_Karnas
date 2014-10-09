#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "PetitFS/diskio.h"
#include "PetitFS/pff.h"

#include "LCD/lcd44780.h"


char file_name[] = "test.txt";
char bufor[128];


/*-----------------------------------------------------------------------*/
/* Main                                                                  */
int main (void)
{
	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7); /* pod�wietlenie wy�wietlacza LCD */

	BYTE res;
	WORD s1;

	FATFS fs;			/* File system object */
	//DIR dir;			/* Directory object */
	//FILINFO fno;		/* File information */

#define SCK PB7
#define MOSI PB5
#define CS PB4
	DDRB |= (1<<CS)|(1<<MOSI)|(1<<SCK)|(1<<CS);
	PORTB |= (1<<CS);
	SPCR |= (1<<SPE)|(1<<MSTR);

	lcd_init();

	lcd_str("odczyt: test.txt");
	lcd_locate(1,0);

	_delay_ms(100);

	res = disk_initialize();

	if( res == FR_OK ) {
		res = pf_mount(&fs);
		if( res == FR_OK ) {
			res = pf_open(file_name);
			if( res == FR_OK ) {

				res = pf_read(bufor, sizeof(bufor), &s1);

				if( res == FR_OK ) {
					bufor[s1+1] = 0;
					lcd_str(bufor);

					_delay_ms(3000);

					lcd_locate(0,0);
					lcd_str("zapis-> test.txt");

					if(bufor[0]=='t') sprintf(bufor, "TEST ****");
					else sprintf(bufor, "test ----");

					s1=4;
					res = pf_write(bufor, sizeof(bufor), &s1);
					write_close();

					if( res != FR_OK ) {
						lcd_locate(0,0);
						lcd_str("write file error");
					} else {
						lcd_locate(1,0);
						lcd_str("OK,   zresetuj  ");
					}

					pf_mount(NULL);



				} else lcd_str("read error");

			} else lcd_str("opern file error");
		} else lcd_str("mount error");
	} else lcd_str("disk init error");

	while(1);

}


