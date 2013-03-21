// *****************************************************************************
// Filename:  wrk16f726.h
// Author: wkf 
// Date: 
// Purpose: Defines and Vars for pic16f726.c module demo
// *****************************************************************************

#ifndef REMOULD2_H
#define REMOULD2_H

// Program config. word 1
__CONFIG(DEBUGDIS & PLLEN & BORV19 & BORDIS & UNPROTECT & MCLREN & PWRTEN & WDTEN & INTIO & 0x3FFF); 
// Program config. word 2
__CONFIG(VCAPDIS);

// REGISTER 8-1: CONFIG1: CONFIGURATION WORD REGISTER 1
//  bit 13 DEBUG: In-Circuit Debugger Mode bit
//*     1 = In-Circuit Debugger disabled, RB6/ICSPCLK and RB7/ICSPDAT are general purpose I/O pins
//      0 = In-Circuit Debugger enabled, RB6/ICSPCLK and RB7/ICSPDAT are dedicated to the debugger
//  bit 12 PLLEN: INTOSC PLL Enable bit
//      0 = INTOSC Frequency is 500 kHz
//*     1 = INTOSC Frequency is 16 MHz (32x)
//* bit 11 Unimplemented: Read as
//  bit 10 BORV: Brown-out Reset Voltage selection bit
//      0 = Brown-out Reset Voltage (VBOR) set to 2.5 V nominal
//*     1 = Brown-out Reset Voltage (VBOR) set to 1.9 V nominal
//  bit 9-8 BOREN<1:0>: Brown-out Reset Selection bits(1)
//*     0x = BOR disabled (Preconditioned State)
//      10 = BOR enabled during operation and disabled in Sleep
//      11 = BOR enabled
//* bit 7 Unimplemented: Read as
//  bit 6 CP: Code Protection bit(2)
//*     1 = Program memory code protection is disabled
//      0 = Program memory code protection is enabled
//  bit 5 MCLRE: RE3/MCLR pin function select bit(3)
//      1 = RE3/MCLR pin function is MCLR
//*     0 = RE3/MCLR pin function is digital input, MCLR internally tied to VDD
//  bit 4 PWRTE: Power-up Timer Enable bit
//      1 = PWRT disabled
//*     0 = PWRT enabled
//  bit 3 WDTE: Watchdog Timer Enable bit
//      1 = WDT enabled
//*     0 = WDT disabled
//  Note 1: Enabling Brown-out Reset does not automatically enable Power-up Timer.
//       2: The entire program memory will be erased when the code protection is turned off.
//       3: When MCLR is asserted in INTOSC or RC mode, the internal clock oscillator is disabled.
//       4: MPLAB IDE masks unimplemented Configuration bits to.
//  bit 2-0 FOSC<2:0>: Oscillator Selection bits
//      111 = RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN
//      110 = RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN
//      101 = INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN
// *    100 = INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN
//      011 = EC: I/O function on RA6/OSC2/CLKOUT pin, CLKIN on RA7/OSC1/CLKIN
//      010 = HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN
//      001 = XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN
//      000 = LP oscillator: Low-power crystal on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN
// REGISTER 8-1: CONFIG1: CONFIGURATION WORD REGISTER 1 (CONTINUED)
//  Note 1: Enabling Brown-out Reset does not automatically enable Power-up Timer.
//       2: The entire program memory will be erased when the code protection is turned off.
//       3: When MCLR is asserted in INTOSC or RC mode, the internal clock oscillator is disabled.
//       4: MPLAB® IDE masks unimplemented Configuration bits to '0'.
//
// REGISTER 8-2: CONFIG2: CONFIGURATION WORD REGISTER 2
//  bit 13-6 Unimplemented: Read as
//  bit 5-4 VCAPEN<1:0>: Voltage Regulator Capacitor Enable bits
//  For the PIC16LF72X:
//      These bits are ignored. All VCAP pin functions are disabled.
//  For the PIC16F72X:
//      00 = VCAP functionality is enabled on RA0
//      01 = VCAP functionality is enabled on RA5
//      10 = VCAP functionality is enabled on RA6
//      11 = All VCAP functions are disabled (not recommended)
//  bit 3-0 Unimplemented: Read as
//  Note 1: MPLAB IDE masks unimplemented Configuration bits to '0'.
//
// Execute contained code ONE-TIME ONLY

//typedef     unsigned char       UCHAR;
//typedef     char                CHAR;

// Create FFlag variable type
// Used for application status bit flags
typedef struct {
	UCHAR   t500ms :    1;      // timer pulse 500 ms
	UCHAR   t1s :       1;      // timer pulse 1 second
    UCHAR   t1m :       1;      // timer pulse 1 minute
    UCHAR   bit3 :      1;
    UCHAR   bit4 :      1;
    UCHAR   bit5 :      1;
    UCHAR   bit6 :      1;
    UCHAR   bit7 :      1;
} FFlags;
/** Prototypes **/
//time delay
#define DelayUs(x)	{unsigned char dcnt; \
					dcnt = (x)/((12*1000L)/(8*1000L)) | 1; \
					while(--dcnt != 0) \
					continue; }

// Defines                      // Pinouts
#define     SWITCH      RA1
#define     AC_SWITCH   RC1
#define     LED1_EN     RC2     //LED1
#define     LED2_EN     RC3     //LED2
#define     TXD         RC6
#define     FAN_SWITCH  RC7
#ifdef __DEBUG
#define     DEBUG1      RB6
#define     DEBUG2      RB7
#endif

/** Constants **/
#define     ON          0       // active low leds
#define     OFF         1       //

#define     VERSION_            2
#define     VERSION_m           8
#ifdef __DEBUG
#define     VERSION_minor       VERSION_m-1
#warning PICC in DEBUG Mode.
#else
#define     VERSION_minor       VERSION_m
#endif

#define     OPTION_VALUE        0b10000111      // OPTION_REG: RBPU=dis,INTEDG=falling,T0CS=FOSC/4,T0SE=0,PSA=timer0,PS=16
                                            //bit 7 RBPU: PORTB Pull-up Enable bit
                                            //* 1 = PORTB pull-ups are disabled
                                            //  0 = PORTB pull-ups are enabled by individual port latch values
                                            //bit 6 INTEDG: Interrupt Edge Select bit
                                            //  1 = Interrupt on rising edge of INT pin
                                            //* 0 = Interrupt on falling edge of INT pin
                                            //bit 5 T0CS: TMR0 Clock Source Select bit
                                            //  1 = Transition on T0CKI pin or CPSOSC signal
                                            //* 0 = Internal instruction cycle clock (FOSC/4)(2MHz)
                                            //bit 4 T0SE: TMR0 Source Edge Select bit
                                            //  1 = Increment on high-to-low transition on T0CKI pin
                                            //* 0 = Increment on low-to-high transition on T0CKI pin
                                            //bit 3 PSA: Prescaler Assignment bit
                                            //  1 = Prescaler is assigned to the WDT
                                            //* 0 = Prescaler is assigned to the Timer0 module
                                            //bit 2-0 PS<2:0>: Prescaler Rate Select bits
                                            //  BIT VALUE    TMR0 RATE       WDT RATE
                                            //    000         1 : 2           1 : 1
                                            //    001         1 : 4           1 : 2
                                            //    010         1 : 8           1 : 4
                                            //    011         1 : 16          1 : 8
                                            //    100         1 : 32          1 : 16
                                            //    101         1 : 64          1 : 32
                                            //    110         1 : 128         1 : 64
                                            //*   111         1 : 256         1 : 128
#define     TMR0_VALUE              0xB2    // Timer 0 should be 160 cycles interrupt, (256/2000)*78 ms=10 ms

#ifdef __DEBUG
#define     T1HIGH                  105
#define     T2HIGH                  95
#define     T3HIGH                  45
#define     T4HIGH                  40

#define     T1LOW                   70
#define     T2LOW                   60
#define     T3LOW                   45
#define     T4LOW                   40
#else
#define     T1HIGH                  105
#define     T2HIGH                  95
#define     T3HIGH                  45
#define     T4HIGH                  40

#define     T1LOW                   70
#define     T2LOW                   60
#define     T3LOW                   45
#define     T4LOW                   40
#endif
#define     T1TIMELIMIT             20
#define     T2TIMELIMIT             20
#define     T3TIMELIMIT             20
#define     T4TIMELIMIT             20
#define     T5TIMELIMIT             20

/** Variables **/

FFlags  Flags;
UCHAR	t0tick;					//timer0ÖÐ¶Ï¼ÆÊýÆ÷
UCHAR   t0tick1;
UCHAR   hours;
UCHAR   minute;
UCHAR   phase_timer;
UCHAR   state;
UCHAR   tlevel[4];
UCHAR   str_in[4];

UCHAR   rom0[8];
UCHAR   rom1[8];
UCHAR   rom2[8];
UCHAR   rom3[8];
UCHAR   rom4[8];
UCHAR   rom5[8];
UCHAR   rom6[8];
UCHAR   rom7[8];
UCHAR   rom8[8];
UCHAR   rom9[8];

UCHAR   tempreture0[2];
UCHAR   tempreture1[2];
UCHAR   tempreture2[2];
UCHAR   tempreture3[2];
UCHAR   tempreture4[2];
UCHAR   tempreture5[2];
UCHAR   tempreture6[2];
UCHAR   tempreture7[2];
UCHAR   tempreture8[2];
UCHAR   tempreture9[2];

#ifdef __DEBUG
UCHAR   debug_count;
UCHAR   idle_minute;
#endif

UCHAR   scratchpad[3];
char strbuf[10];

void timerInit(void);
void main(void);
void main_process(void);
void fan_off(void);
void fan_on(void);
UCHAR Remould_Phrase(void);
UCHAR Read_Tempreture(UCHAR * );
UCHAR sensor_init(void);
void ErrorState(UCHAR );
void Init(void);
void Init_var(void);
void interrupt isr(void);
void DelayMs(unsigned char );
void DelaySec(unsigned char );
#endif /* REMOULD2_H */
