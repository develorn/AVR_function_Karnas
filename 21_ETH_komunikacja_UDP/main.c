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
 * ---> OBS£UGA KLIENT/SERWER komunikacji UDP <-----
 * MODYFIKACJE: Miros³aw Kardaœ --- ATmega32
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
// ustalamy adres IP urz¹dzenia
static uint8_t myip[4] = {192,168,0,120};

// ustalamy porty UDP z jakich bêdziemy korzystaæ
// mo¿e ich byæ dowolna iloœæ
static uint16_t myport[] = {1200,22700};
// ustalamy adres IP bramy domyœlnej w sieci LAN
static uint8_t gwip[4] = {192,168,0,1};

// ustalamy wielkoœæ bufora dla ramek TCP/UDP
#define BUFFER_SIZE 650
static uint8_t buf[BUFFER_SIZE+1];

// wskaŸnik do funkcji zwrotnej w³asnego zdarzenia UDP_EVENT()
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
// indeksy adresów IP w postaci przyjaznych nazw
enum ip_names {ip_pc, ip_sterownik1};
// adresy IP sterowników z którymi bêdziemy siê komunikowaæ
// za pomoc¹ protoko³u UDP
static uint8_t farip[2][4] = { {192,168,0,10}, {192,168,0,180} };

// timer programowy na us³ugi funkcji SuperDebounce()
volatile uint8_t Timer1;	/* timer programowy 100Hz */


// separator tokenów we w³asnych ramkach przesy³anych przez UDP
char sep[] = "^";

// przydzielenie kodów we w³asnych ramkach przesy³anych przez UDP
#define ILOSC_KOMEND 3
enum commands {set_led, get_led, set_lcd};

// tablica wskaŸników do funkcji parsuj¹cych w pamiêci FLASH
void (*parse_fun[ILOSC_KOMEND])(char *buf) PROGMEM = {
		parse_set_led,
		parse_get_led,
		parse_set_lcd
};

//---------------------------------- deklaracje zmiennych globalnych -------------------------------









//======================= main() ================================================
int main(void){



/********** inicjalizacja pinów/portów *********************/
	// podœwietlenie wyœwietlacza LCD
	DDRA |= (1<<PA7);
	PORTA |= (1<<PA7);

	// po resecie piny klawiszy s¹ wejœciami
	// podci¹gniêcie wejœæ klawiszy do VCC
	KL_PORT |= KL1|KL2;

	// kierunki pinów diod LED jako wyjœcia
	LED1_DDR |= LED1;
	LED2_DDR |= LED2;
	// wy³¹czenie diod LED
	LED1_OFF;
	LED2_OFF;

/********** inicjalizacja modu³ów sprzêtowych procesora *********************/
	/* Timer2 – inicjalizacja przerwania co 10ms */
	TCCR2 	|= (1<<WGM21);			// tryb pracy CTC
	TCCR2 	|= (1<<CS22)|(1<<CS21)|(1<<CS20);	// preskaler = 1024
	OCR2 	= 108;					// przerwanie porównania co 10ms (100Hz)
	TIMSK 	= (1<<OCIE2);			// Odblokowanie przerwania CompareMatch

/********** inicjalizacja funkcji bibliotecznych *********************/
    lcd_init();
    lcd_str("Start..");


/********** inicjalizacja funkcji obs³ugi stosu TCP *********************/
    /*inicjalizacja uk³adu enc28j60*/
    enc28j60Init(mymac);

    // konfiguracja diod LED w gnieŸdzie RJ45
    // zielona (LEDA) 	- link
    // ¿ó³ta (LEDB) 	- rx/tx
    // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
    enc28j60PhyWrite(PHLCON,0x476);

    // inicjalizacja stosu TCP
    init_ip_arp_udp_tcp(mymac,myip,80);

    // inicjalizacja bramy domyœlnej
    // konieczne dla funkcji klienckich
    client_set_gwip(gwip);
/********** inicjalizacja funkcji obs³ugi stosu TCP *********************/

    // zarejestrowanie w³asnej procedury obs³ugi/reakcji na ping
    register_ping_rec_callback(&ping_callback);

    // zarejestrowanie w³asnej procedury obs³ugi/reakcji na
    // przychodz¹ce pakiety UDP
    register_udp_event_callback(udp_event_callback);

/********** w³¹czenie przerwañ globalnych *********************/
    sei();


/******************************************************/
/************                    **********************/
/********** g³ówna pêtla programu *********************/
/***********                     **********************/
/******************************************************/
    while(1){

    	// zdarzenie sprawdzaj¹ce czy s¹ jakieœ dane
    	// z sieci/uk³adu ENC28j60
    	UDP_EVENT(myport);

    	// przyk³adowa obs³uga klawiszy i wysy³anie
    	// w³asnych klienckich ramek UDP do wybranych urz¹dzeñ w sieci
    	SuperDebounce(&PINC, (1<<PC2), 100, 200, mk_send_udp, mk_send_udp);
    	SuperDebounce(&PINC, (1<<PC3), 20, 200, mk_send_udp, 0);

    }
}
//======================= main() koniec =========================================






/******* funkcja obs³uguj¹ca komendê set_led  ******/
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

/******* funkcja obs³uguj¹ca komendê get_led  ******/
void parse_get_led(char * s) {
	char str[20];
	sprintf(str,"%i^%i^%i^%i", get_led, (PINB & (1<<PB1))>>1, (PIND & (1<<PD7))>>7, (PINA & (1<<PA7))>>7 );
	make_udp_reply_from_request(buf,str,strlen(str), myport[0]);
}

/******* funkcja obs³uguj¹ca komendê set_lcd  ******/
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
/******* w³asna funkcja u¿ytkownika w której mo¿emy reagowaæ na ramkê UDP ***********/
/****/
void udp_event_callback(uint8_t *peer_ip, uint16_t port, uint16_t datapos, uint16_t len) {
	uint8_t i=0;
    char str[40];

    // wskaŸnik na potrzeby funkcji strtok_r
    char *wsk, *reszta;

    // wyœwietlamy na LCD z jakiego IP nadesz³a ramka
    // wyœwietlamy na jaki port nadesz³a ramka
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

    // jeœli ramka przysz³a na drugi numer portu z listy myport[]
    if(port == myport[1]) {
    	// odpowiadamy na zapytanie UDP po ten sam port
    	strcpy(str,"Ramka odpowiedzi UDP");
    	make_udp_reply_from_request(buf,str,strlen(str), port);

        // wysy³amy now¹ ramkê na dowolny inny port
    	strcpy((char*)&buf[datapos],"Nowa ramka UDP!");
        send_udp_prepare(buf, 1200, farip[ip_pc], 43500);
        send_udp_transmit(buf,strlen(str));
    }

    // jeœli ramka przysz³a na pierwszy numer portu z listy myport[]
    if(port == myport[0]) {
    	// sprawdzamy czy zawiera dane
    	wsk = strtok_r((char*)&buf[datapos], sep, &reszta);
    	if( wsk ) {
    		// jeœli zawiera dane to sprawdzamy jaka komenda
    		switch( atoi(wsk) ) {
    		// i w zale¿noœci od komendy wywo³ujemy odpowieni¹
    		// funkcjê parsuj¹c¹ z tabeli wskaŸników do funkcji parsuj¹cych
    		// przekazujemy jednoczeœnie wskaŸnik do pozosta³ej czêœci
    		// ³añcucha z danymi, który funkcje bêd¹ parsowaæ we w³asnym zakresie
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
    // obs³uga ni¿szych warstw stosu TCP jak ICMP, ARP itp
    // m.inn t¹ drog¹ obs³ugiwane s¹ zewnêtrzne PING'i
    // w tym miejscu sprawdzane jest od razu czy odczytana ramka
    // jest posiada typ jaki obs³ugiwany jest przez ten stos TCP
    // oraz czy jest ona zaadresowana do nas (w numeru IP)
    dat_p=packetloop_icmp_tcp(buf,plen);

    // jeœli d³ugoœæ odebranej ramki jest wiêksza ni¿ 0
    // to oznacza, ¿e mamy do czynienia z prawid³owo odebran¹
    // ramk¹ wraz z prawid³ow¹ sum¹ kontroln¹
    // sprawdzamy tak¿e, czy na pewno mamy do czynienia z ramk¹ UDP a nie np TCP
    if( plen && buf[IP_PROTO_P]==IP_PROTO_UDP_V ) {

    	// sprawdzamy do jakiego nr portu kierowana jest ta ramka UDP
    	dport = (buf[UDP_DST_PORT_H_P]<<8) | buf[UDP_DST_PORT_L_P];

    	// sprawdzamy w pêtli czy jest to ramka przeznaczona do
    	// portu nas³uchowego, które obs³uguje nasze urz¹dzenie
    	// listê dowolnej iloœci portów definiujemy w tablicy myport[]
    	// która przekazywana jest jako parametr do zdarzenia/funkcji UDP_EVENT()
    	do {
    		if( (is_my_port = (port[i++] == dport)) ) break;
    	} while (i<sizeof(port));

    	// sprawdzamy czy informacja przysz³a na obs³ugiwany przez nas port UDP
    	if ( is_my_port ){
			udp_data_len=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;

			// jeœli wszystko siê zgadza to sprawdzamy, czy zarejestrowana jest
			// funkcja zwrotna przez u¿ytkownika. Jeœli nie jest zarejestrowana
			// to nie wykonana siê ¿adna akcja poza powy¿ej obs³ug¹
			// ni¿szych warstw stosu TCP (np PING czy ARP)
			// Jeœli jest zarejestrowana to zostanie wywo³ana z odpowiednimi parametrami
			// 1. podany bêdzie adres IP sk¹d nadesz³a ramka
			// 2. podany bêdzie port na jaki zosta³a do nas skierowana ramka
			// 3. indeks do bufora ca³ej ramki TCP, wskazuj¹cy na pocz¹tek danych w ramce UDP
			// 4. iloœæ danych w bajtach przes³anych w ramce UDP
			if(mk_udp_event_callback) (*mk_udp_event_callback)(&(buf[IP_SRC_P]), dport, UDP_DATA_P, udp_data_len);
        }
    }
}


/******* funkcja wysy³aj¹ca w³asne dane przez UDP w zale¿noœci od klawisza  ******/
void mk_send_udp(uint8_t nr_klawisza) {
	char str[30];
	// przygotowanie w³asnej ramki (tekest) oraz np numer wciœniêtego klawisza
	sprintf(str,"SuperDebounce klawsz nr: %i", nr_klawisza);
	// przes³anie ramki do wybranego adresu IP pod wskazany (dowolny) port
	// w tym przypadku wysy³amy dane do komputera PC
    send_udp(buf, str, strlen(str), 1100, farip[ip_pc], 21000);
}


/******* funkcja callback ping *********/
// dziêki tej funkcji zwrotnej mo¿emy dowiedzieæ siê
// jaki adres IP PING'uje nasz uk³ad i reagowaæ na
// to w specyficzny sposób
void ping_callback(uint8_t *ip) {
	// za ka¿dym razem gdy przychodzi PING do naszego urz¹dzenia
	// zmieniamy stan diody LED na przeciwny
	LED1_TOG;
}




/************** funkcja SuperDebounce do obs³ugi pojedynczych klawiszy ***************
 * 							AUTOR: Miros³aw Kardaœ
 * ZALETY:
 * 		- nie wprowadza najmniejszego spowalnienia
 * 		- posiada funkcjê REPEAT (powtarzanie akcji dla d³u¿ej wciœniêtego przycisku)
 * 		- mo¿na przydzieliæ ró¿ne akcje dla trybu REPEAT i pojedynczego klikniêcia
 * 		- mo¿na przydzieliæ tylko jedn¹ akcjê wtedy w miejsce drugiej przekazujemy 0 (NULL)
 * 		- zabezpieczenie przed naciœniêciem dwóch klawiszy jednoczeœnie
 *
 * Wymagania:
 * 	Timer programowy utworzony w oparciu o Timer sprzêtowy (przerwanie 100Hz)
 *
 * 	Parametry wejœciowe - 6 :
 *  *KPIN - nazwa PINx portu na którym umieszczony jest klawisz, np: PINB
 *  key_mask - maska klawisza np: (1<<PB3)
 *  rep_time - czas powtarzania funkcji rep_proc w trybie REPEAT
 *  rep_wait - czas oczekiwania do przejœcia do trybu REPEAT
 *  push_proc - wskaŸnik do w³asnej funkcji wywo³ywanej raz po zwolenieniu przycisku
 *  rep_proc - wskaŸnik do w³asnej funkcji wykonywanej w trybie REPEAT
 **************************************************************************************/
void SuperDebounce(volatile uint8_t *KPIN,
		uint8_t key_mask, uint16_t rep_time, uint16_t rep_wait,
		void (*push_proc)(uint8_t), void (*rep_proc)(uint8_t) ) {

	enum KS {idle, debounce, go_rep, wait_rep, rep};

	static enum KS key_state;
	static uint8_t last_key;
	uint8_t key_press;

	// zabezpieczenie przed wykonywaniem tej samej funkcji dla
	// dwóch klawiszy wciskanych jednoczeœnie (zawsze bêdzie
	// wykonywana odpowiednia akcja dla tego, który zosta³
	// wciœniêty jako pierwszy
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



/**** obs³uga przerwania Timer2 CompareMatch *****/
// potrzebnego do realizacji timera programowego dla SuperDebounce()
// mo¿na je wykorzystaæ na utworzenie dodatkowych w³asnych timerów programowych
ISR(TIMER2_COMP_vect)
{
	uint8_t n;

	n = Timer1;		/* 100Hz Timer1 */
	if (n) Timer1 = --n;
}
