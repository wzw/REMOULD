#ifndef _SERIAL_H_
#define _SERIAL_H_

#define BAUD 9600      
#define FOSC 8000000L
#define NINE 0     /* Use 9bit communication? FALSE=8bit */

#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1))
#define HIGH_SPEED 1

#if NINE == 1
#define NINE_BITS 0x40
#else
#define NINE_BITS 0
#endif

#if HIGH_SPEED == 1
#define SPEED 0x4
#else
#define SPEED 0
#endif

#define RX_PIN TRISC7
#define TX_PIN TRISC6

/* Serial initialization */
#define init_comms()\
	RX_PIN = 1;	\
	TX_PIN = 1;		  \
	SPBRG = DIVIDER;     	\
	RCSTA = (NINE_BITS|0x90);	\
	TXSTA = (SPEED|NINE_BITS|0x20)

extern void putch(unsigned char);
extern unsigned char getch(void);
extern unsigned char getche(void);
extern unsigned char getch0(void);
extern unsigned char getche0(void);
extern void	put_string(const char *);
extern void put_ascii(unsigned char );

#endif
