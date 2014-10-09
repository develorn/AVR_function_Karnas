/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 *
 * Tuxgraphics AVR webserver/ethernet board
 *
 * http://tuxgraphics.org/electronics/
 * Chip type           : Atmega88/168/328 with ENC28J60
 *
 *
 * ---> OBS�UGA KLIENT/SERWER komunikacji UDP <-----
 * MODYFIKACJE: Miros�aw Karda� --- ATmega32
 *
 *********************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ip_arp_udp_tcp.h"
#include "enc28j60.h"
#include "util/delay.h"
#include "net.h"

#include "LCD/lcd44780.h"

//-------------------------- definicje preprocesora ---------------------------------
#define LED1_DDR	DDRB
#define LED1_PORT	PORTB
#define LED1		(1<<PB1)
#define LED1_ON		LED1_PORT &= ~LED1
#define LED1_OFF	LED1_PORT |= LED1
#define LED1_TOG	LED1_PORT ^= LED1

#define LED2_DDR	DDRD
#define LED2_PORT	PORTD
#define LED2		(1<<PD7)
#define LED2_ON		LED2_PORT &= ~LED2
#define LED2_OFF	LED2_PORT |= LED2
#define LED2_TOG	LED2_PORT ^= LED2

#define KL_PORT		PORTC
#define KL1			(1<<PC2)
#define KL2			(1<<PC3)
//-------------------------- definicje preprocesora ---------------------------------


//---------------------------------------------------------------------------------------------------------------
// ustalamy adres MAC
static uint8_t mymac[6] = {0x00,'M','I','R','E','K'};
// ustalamy adres IP urz�dzenia
static uint8_t myip[4] = {192,168,0,120};

// ustalamy porty UDP z jakich b�dziemy korzysta�
// mo�e ich by� dowolna ilo��
static uint16_t myport[] = {1200,22700};
// ustalamy adres IP bramy domy�lnej w sieci LAN
static uint8_t gwip[4] = {192,168,0,1};

// ustalamy wielko�� bufora dla ramek TCP/UDP
#define BUFFER_SIZE 650
static uint8_t buf[BUFFER_SIZE+1];

// wska�nik do funkcji zwrotnej w�asnego zdarzenia UDP_EVENT()
static void (*mk_udp_event_callback)(uint8_t *peer_ip, uint16_t port,
				uint16_t datapos, uint16_t len);

// funkcja do rejestracji funkcji zwrotnej w zdarzeniu UDP_EVENT()
void register_udp_event_callback(void (*callback)(uint8_t *peer_ip,
				uint16_t port, uint16_t datapos, uint16_t len))
{
	mk_udp_event_callback = callback;
}
//---------------------------------------------------------------------------------------------------------------


//---------------------------------- deklaracje funkcji -------------------------------
void ping_callback(uint8_t *ip);

void SuperDebounce( volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(uint8_t), void (*rep_proc)(uint8_t) );

void mk_send_udp(uint8_t nr_klawisza);

void udp_event_callback(uint8_t *peer_ip, uint16_t port,
		uint16_t datapos, uint16_t len);

void UDP_EVENT(uint16_t *port);

void parse_set_led(char * buf);
void parse_get_led(char * s);
void parse_set_lcd(char * buf);
//---------------------------------- deklaracje funkcji -------------------------------


//---------------------------------- deklaracje zmiennych globalnych -------------------------------
// indeksy adres�w IP w postaci przyjaznych nazw
enum ip_names {ip_pc, ip_sterownik1};
// adresy IP sterownik�w z kt�rymi b�dziemy si� komunikowa�
// za pomoc� protoko�u UDP
static uint8_t farip[2][4] = { {192,168,0,10}, {192,168,0,180} };

// timer programowy na us�ugi funkcji SuperDebounce()
volatile uint8_t Timer1;	/* timer programowy 100Hz */


// separator token�w we w�asnych ramkach przesy�anych przez UDP
char sep[] = "^";

// przydzielenie kod�w we w�asnych ramkach przesy�anych przez UDP
#define ILOSC_KOMEND 3
enum commands {set_led, get_led, set_lcd};

// tablica wska�nik�w do funkcji parsuj�cych w pami�ci FLASH
void (*parse_fun[ILOSC_KOMEND])(char *buf) PROGMEM = {
		parse_set_led,
		parse_get_led,
		parse_set_lcd
};

//---------------------------------- deklaracje zmiennych globalnych -------------------------------









//======================= main() ================================================
int main(void){



/********** inicjalizacja pin�w/port�w *********************/
	// pod�wietlenie wy�wietlacza LCD
	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7);

	// po resecie piny klawiszy s� wej�ciami
	// podci�gni�cie wej�� klawiszy do VCC
	KL_PORT |= KL1|KL2;

	// kierunki pin�w diod LED jako wyj�cia
	LED1_DDR |= LED1;
	LED2_DDR |= LED2;
	// wy��czenie diod LED
	LED1_OFF;
	LED2_OFF;

/********** inicjalizacja modu��w sprz�towych procesora *********************/
	/* Timer2 � inicjalizacja przerwania co 10ms */
	TCCR2 	|= (1<<WGM21);			// tryb pracy CTC
	TCCR2 	|= (1<<CS22)|(1<<CS21)|(1<<CS20);	// preskaler = 1024
	OCR2 	= 108;					// przerwanie por�wnania co 10ms (100Hz)
	TIMSK 	= (1<<OCIE2);			// Odblokowanie przerwania CompareMatch

/********** inicjalizacja funkcji bibliotecznych *********************/
    lcd_init();
    lcd_str("Start..");


/********** inicjalizacja funkcji obs�ugi stosu TCP *********************/
    /*inicjalizacja uk�adu enc28j60*/
    enc28j60Init(mymac);

    // konfiguracja diod LED w gnie�dzie RJ45
    // zielona (LEDA) 	- link
    // ��ta (LEDB) 	- rx/tx
    // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
    enc28j60PhyWrite(PHLCON,0x476);

    // inicjalizacja stosu TCP
    init_ip_arp_udp_tcp(mymac,myip,80);

    // inicjalizacja bramy domy�lnej
    // konieczne dla funkcji klienckich
    client_set_gwip(gwip);
/********** inicjalizacja funkcji obs�ugi stosu TCP *********************/

    // zarejestrowanie w�asnej procedury obs�ugi/reakcji na ping
    register_ping_rec_callback(&ping_callback);

    // zarejestrowanie w�asnej procedury obs�ugi/reakcji na
    // przychodz�ce pakiety UDP
    register_udp_event_callback(udp_event_callback);

/********** w��czenie przerwa� globalnych *********************/
    sei();


/******************************************************/
/************                    **********************/
/********** g��wna p�tla programu *********************/
/***********                     **********************/
/******************************************************/
    while(1){

    	// zdarzenie sprawdzaj�ce czy s� jakie� dane
    	// z sieci/uk�adu ENC28j60
    	UDP_EVENT(myport);

    	// przyk�adowa obs�uga klawiszy i wysy�anie
    	// w�asnych klienckich ramek UDP do wybranych urz�dze� w sieci
    	SuperDebounce(&PINC, (1<<PC2), 100, 200, mk_send_udp, mk_send_udp);
    	SuperDebounce(&PINC, (1<<PC3), 20, 200, mk_send_udp, 0);

    }
}
//======================= main() koniec =========================================






/******* funkcja obs�uguj�ca komend� set_led  ******/
void parse_set_led(char * buf) {
	char *wsk;
	uint8_t nr_led, led_on;

	wsk = strtok_r(NULL, sep, &buf);
	if( wsk ) {
		nr_led = atoi(wsk);
		wsk = strtok_r(NULL, sep, &buf);
		if( wsk ) {
			led_on = atoi(wsk);
			switch( nr_led ) {
			case 1: if(led_on) LED1_ON; else LED1_OFF; break;
			case 2: if(led_on) LED2_ON; else LED2_OFF; break;
			case 3: if(led_on) PORTA |= (1<<PA7); else PORTA &= ~(1<<PA7);
			}
		}
	}
}

/******* funkcja obs�uguj�ca komend� get_led  ******/
void parse_get_led(char * s) {
	char str[20];
	sprintf(str,"%i^%i^%i^%i", get_led, (PINB & (1<<PB1))>>1, (PIND & (1<<PD7))>>7, (PINA & (1<<PA7))>>7 );
	make_udp_reply_from_request(buf,str,strlen(str), myport[0]);
}

/******* funkcja obs�uguj�ca komend� set_lcd  ******/
void parse_set_lcd(char * buf) {
	char *wsk, *wsk1;

	wsk = strtok_r(NULL, sep, &buf);
	wsk1 = strtok_r(NULL, sep, &buf);
	if( wsk  ) {
		lcd_cls();
		lcd_str( wsk );
		lcd_locate(1,0);
		lcd_str( wsk1 );
	}
}

/****/
/******* w�asna funkcja u�ytkownika w kt�rej mo�emy reagowa� na ramk� UDP ***********/
/****/
void udp_event_callback(uint8_t *peer_ip, uint16_t port, uint16_t datapos, uint16_t len) {
	uint8_t i=0;
    char str[40];

    // wska�nik na potrzeby funkcji strtok_r
    char *wsk, *reszta;

    // wy�wietlamy na LCD z jakiego IP nadesz�a ramka
    // wy�wietlamy na jaki port nadesz�a ramka
	lcd_cls();
	lcd_str("UDP from: ");
	lcd_int(port);
	lcd_locate(1,0);
    while(i<4){
        lcd_int(peer_ip[i]);
        if(3==i) break;
        lcd_str(".");
        i++;
    }

    // je�li ramka przysz�a na drugi numer portu z listy myport[]
    if(port == myport[1]) {
    	// odpowiadamy na zapytanie UDP po ten sam port
    	strcpy(str,"Ramka odpowiedzi UDP");
    	make_udp_reply_from_request(buf,str,strlen(str), port);

        // wysy�amy now� ramk� na dowolny inny port
    	strcpy((char*)&buf[datapos],"Nowa ramka UDP!");
        send_udp_prepare(buf, 1200, farip[ip_pc], 43500);
        send_udp_transmit(buf,strlen(str));
    }

    // je�li ramka przysz�a na pierwszy numer portu z listy myport[]
    if(port == myport[0]) {
    	// sprawdzamy czy zawiera dane
    	wsk = strtok_r((char*)&buf[datapos], sep, &reszta);
    	if( wsk ) {
    		// je�li zawiera dane to sprawdzamy jaka komenda
    		switch( atoi(wsk) ) {
    		// i w zale�no�ci od komendy wywo�ujemy odpowieni�
    		// funkcj� parsuj�c� z tabeli wska�nik�w do funkcji parsuj�cych
    		// przekazujemy jednocze�nie wska�nik do pozosta�ej cz�ci
    		// �a�cucha z danymi, kt�ry funkcje b�d� parsowa� we w�asnym zakresie
    		case set_led: parse_fun[set_led](reszta); break;
    		case get_led: parse_fun[get_led](reszta); break;
    		case set_lcd: parse_fun[set_lcd](reszta);
    		}
    	}
    }
}


/********** zdarzenie UDP EVENT ***********************/
void UDP_EVENT(uint16_t *port) {
	// zmienne tymczasowe (automatyczne)
    uint16_t plen, dat_p;
    uint16_t dport;
    uint8_t is_my_port=0;
    uint8_t i=0;


    uint8_t udp_data_len=0;


    // sprawdzamy czy istnieje nowy odebrany pakiet
    plen = enc28j60PacketReceive(BUFFER_SIZE, buf);
    // obs�uga ni�szych warstw stosu TCP jak ICMP, ARP itp
    // m.inn t� drog� obs�ugiwane s� zewn�trzne PING'i
    // w tym miejscu sprawdzane jest od razu czy odczytana ramka
    // jest posiada typ jaki obs�ugiwany jest przez ten stos TCP
    // oraz czy jest ona zaadresowana do nas (w numeru IP)
    dat_p=packetloop_icmp_tcp(buf,plen);

    // je�li d�ugo�� odebranej ramki jest wi�ksza ni� 0
    // to oznacza, �e mamy do czynienia z prawid�owo odebran�
    // ramk� wraz z prawid�ow� sum� kontroln�
    // sprawdzamy tak�e, czy na pewno mamy do czynienia z ramk� UDP a nie np TCP
    if( plen && buf[IP_PROTO_P]==IP_PROTO_UDP_V ) {

    	// sprawdzamy do jakiego nr portu kierowana jest ta ramka UDP
    	dport = (buf[UDP_DST_PORT_H_P]<<8) | buf[UDP_DST_PORT_L_P];

    	// sprawdzamy w p�tli czy jest to ramka przeznaczona do
    	// portu nas�uchowego, kt�re obs�uguje nasze urz�dzenie
    	// list� dowolnej ilo�ci port�w definiujemy w tablicy myport[]
    	// kt�ra przekazywana jest jako parametr do zdarzenia/funkcji UDP_EVENT()
    	do {
    		if( (is_my_port = (port[i++] == dport)) ) break;
    	} while (i<sizeof(port));

    	// sprawdzamy czy informacja przysz�a na obs�ugiwany przez nas port UDP
    	if ( is_my_port ){
			udp_data_len=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;

			// je�li wszystko si� zgadza to sprawdzamy, czy zarejestrowana jest
			// funkcja zwrotna przez u�ytkownika. Je�li nie jest zarejestrowana
			// to nie wykonana si� �adna akcja poza powy�ej obs�ug�
			// ni�szych warstw stosu TCP (np PING czy ARP)
			// Je�li jest zarejestrowana to zostanie wywo�ana z odpowiednimi parametrami
			// 1. podany b�dzie adres IP sk�d nadesz�a ramka
			// 2. podany b�dzie port na jaki zosta�a do nas skierowana ramka
			// 3. indeks do bufora ca�ej ramki TCP, wskazuj�cy na pocz�tek danych w ramce UDP
			// 4. ilo�� danych w bajtach przes�anych w ramce UDP
			if(mk_udp_event_callback) (*mk_udp_event_callback)(&(buf[IP_SRC_P]), dport, UDP_DATA_P, udp_data_len);
        }
    }
}


/******* funkcja wysy�aj�ca w�asne dane przez UDP w zale�no�ci od klawisza  ******/
void mk_send_udp(uint8_t nr_klawisza) {
	char str[30];
	// przygotowanie w�asnej ramki (tekest) oraz np numer wci�ni�tego klawisza
	sprintf(str,"SuperDebounce klawsz nr: %i", nr_klawisza);
	// przes�anie ramki do wybranego adresu IP pod wskazany (dowolny) port
	// w tym przypadku wysy�amy dane do komputera PC
    send_udp(buf, str, strlen(str), 1100, farip[ip_pc], 21000);
}


/******* funkcja callback ping *********/
// dzi�ki tej funkcji zwrotnej mo�emy dowiedzie� si�
// jaki adres IP PING'uje nasz uk�ad i reagowa� na
// to w specyficzny spos�b
void ping_callback(uint8_t *ip) {
	// za ka�dym razem gdy przychodzi PING do naszego urz�dzenia
	// zmieniamy stan diody LED na przeciwny
	LED1_TOG;
}




/************** funkcja SuperDebounce do obs�ugi pojedynczych klawiszy ***************
 * 							AUTOR: Miros�aw Karda�
 * ZALETY:
 * 		- nie wprowadza najmniejszego spowalnienia
 * 		- posiada funkcj� REPEAT (powtarzanie akcji dla d�u�ej wci�ni�tego przycisku)
 * 		- mo�na przydzieli� r�ne akcje dla trybu REPEAT i pojedynczego klikni�cia
 * 		- mo�na przydzieli� tylko jedn� akcj� wtedy w miejsce drugiej przekazujemy 0 (NULL)
 * 		- zabezpieczenie przed naci�ni�ciem dw�ch klawiszy jednocze�nie
 *
 * Wymagania:
 * 	Timer programowy utworzony w oparciu o Timer sprz�towy (przerwanie 100Hz)
 *
 * 	Parametry wej�ciowe - 6 :
 *  *KPIN - nazwa PINx portu na kt�rym umieszczony jest klawisz, np: PINB
 *  key_mask - maska klawisza np: (1<<PB3)
 *  rep_time - czas powtarzania funkcji rep_proc w trybie REPEAT
 *  rep_wait - czas oczekiwania do przej�cia do trybu REPEAT
 *  push_proc - wska�nik do w�asnej funkcji wywo�ywanej raz po zwolenieniu przycisku
 *  rep_proc - wska�nik do w�asnej funkcji wykonywanej w trybie REPEAT
 **************************************************************************************/
void SuperDebounce(volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(uint8_t), void (*rep_proc)(uint8_t) ) {

	enum KS {idle, debounce, go_rep, wait_rep, rep};

	static enum KS key_state;
	static uint8_t last_key;
	uint8_t key_press;

	// zabezpieczenie przed wykonywaniem tej samej funkcji dla
	// dw�ch klawiszy wciskanych jednocze�nie (zawsze b�dzie
	// wykonywana odpowiednia akcja dla tego, kt�ry zosta�
	// wci�ni�ty jako pierwszy
	if( last_key && last_key != key_mask ) return;

	key_press = !(*KPIN & key_mask);

	if( key_press && !key_state ) {
		key_state = debounce;
		Timer1 = 5;
	} else
	if( key_state  ) {
		if( key_press && debounce==key_state && !Timer1 ) {
			key_state = go_rep;
			Timer1=3;
			last_key = key_mask;
		} else
		if( !key_press && key_state>debounce && key_state<rep ) {
			if(push_proc) push_proc(key_mask);						/* KEY_UP */
			key_state=idle;
			last_key = 0;
		} else
		if( key_press && go_rep==key_state && !Timer1 ) {
			if(!rep_time) rep_time=20;
			if(!rep_wait) rep_wait=150;
			key_state = wait_rep;
			Timer1=rep_wait;
		} else
		if( key_press && wait_rep==key_state && !Timer1 ) {
			key_state = rep;
		} else
		if( key_press && rep==key_state && !Timer1 ) {
			Timer1 = rep_time;
			if(rep_proc) rep_proc(key_mask);						/* KEY_REP */
		}
	}

	if( key_state>=wait_rep && !key_press ) {
		key_state = idle;
		last_key = 0;
	}
}



/**** obs�uga przerwania Timer2 CompareMatch *****/
// potrzebnego do realizacji timera programowego dla SuperDebounce()
// mo�na je wykorzysta� na utworzenie dodatkowych w�asnych timer�w programowych
ISR(TIMER2_COMP_vect)
{
	uint8_t n;

	n = Timer1;		/* 100Hz Timer1 */
	if (n) Timer1 = --n;
}
