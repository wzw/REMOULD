#include <htc.h>
#include <stdio.h>
#include "usart.h"

void 
putch(unsigned char byte) 
{
	/* output one byte */
	while(!TXIF) CLRWDT();                      /* set when register is empty */
	TXREG = byte;
}

unsigned char 
getch() {
	/* retrieve one byte */
	while(!RCIF)	                            /* set when register is not empty */
		CLRWDT();
	return RCREG;	
}

unsigned char
getche(void)
{
	unsigned char c;
	putch(c = getch());
	return c;
}

unsigned char 
getch0() {
	/* retrieve one byte */
	if(RCIF) return RCREG;
    return(0);
}

unsigned char
getche0(void)
{
	unsigned char c;

    c = getch0();
	if(c) putch(c);
	return c;
}

void put_string(const char * s)
{
	while(*s){
		putch(*s++);
        CLRWDT();
    }
}

void put_ascii(unsigned char c)
{
    unsigned char temp;

    temp = ((c>>4) & 0xF) | 0x30;
    if(temp > 0x39)temp += 0x07;
    putch(temp);
    temp = (c & 0xF) | 0x30;
    if(temp > 0x39)temp += 0x07;
    putch(temp);
}
