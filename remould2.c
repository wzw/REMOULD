// *****************************************************************************
// Filename: wrk16f726.c
// Compiler: Hi-Tech PIC-C v.9.60
// Author: 
// Date: 
// *****************************************************************************

//#include	<pic.h>
#include    <htc.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include    "remould_def.h"
#include 	"usart.h"
#include	"dsensor.h"
#include 	"remould2.h"

//---------------------------------------------------------------
//Function: isr(void)
void interrupt isr(void)
{
	//Timer0 interrupt
	CLRWDT();
	if(T0IE && T0IF) {
		T0IF = 0;						                    //clear T0IF every time
    	TMR0 = TMR0_VALUE;			                        //Timer 0 should be 160 cycles interrupt, (256/2000)*78 ms=10 ms
#ifdef __DEBUG
    DEBUG1 = !DEBUG1;
#endif
		t0tick++;						                    // counter ++
		if(t0tick > 49) t0tick = 0;
        if(!t0tick){
            Flags.t500ms = 1;
            t0tick1++;
#ifdef __DEBUG
            LED2_EN = !LED2_EN;
#endif
            if(!(t0tick1 & 0x01)) {
                Flags.t1s = 1;
            }
            if(t0tick1 > 119){
                Flags.t1m = 1;
                minute++;
                if(minute > 59){minute = 0; hours++;}
                t0tick1 = 0;
            }
            switch(state){
                case S_START:
                    LED1_EN = OFF;
                    if(t0tick1 & 0x02)LED2_EN = ON;
                    else LED2_EN = OFF;
                    if(t0tick1 > 10) state = S_INIT;
                    break;
                case S_INIT:
                    LED1_EN = OFF;
                    if(t0tick1 & 0x04)LED2_EN = ON;
                    else LED2_EN = OFF;
                    break;
                case S_HEATING:
                    LED1_EN = ON;
#ifndef __DEBUG
                    LED2_EN = OFF;
#endif
                    break;
                case S_HEATING_WAIT:
#ifndef __DEBUG
                    LED2_EN = OFF;
#endif
                    if(t0tick1 & 0x02)LED1_EN = ON;
                    else LED1_EN = OFF;
                    break;
                case S_REHEATING:
                    LED1_EN = ON;
#ifndef __DEBUG
                    LED2_EN = OFF;
#endif
                    break;
                case S_COOLING:
#ifndef __DEBUG
                    LED2_EN = OFF;
#endif
                    if(t0tick1 & 0x02)LED1_EN = ON;
                    else LED1_EN = OFF;
                    break;
                case S_COOLING_WAIT:
                    LED1_EN = OFF;
                    if(t0tick1 & 0x02)LED2_EN = ON;
                    else LED2_EN = OFF;
                    break;
                case S_FINISH:
                    LED1_EN = OFF;
#ifndef __DEBUG
                    LED2_EN = ON;
#endif
                    break;
//                case S_INIT:
//                    break;
                case E_NO_DS18B20:
                case E_DS18B20_CRC:
                case E_DS18B20_NO_RESPONSE:
                case E_DS18B20_TIMEOUT:
                case E_HEATING_TIMEOUT:          // Heating timeout
                case E_HEATING_WAIT_TIMEOUT:     // Heating wait timeout
                case E_REHEATING_TIMEOUT:        // ReHeating timeout
                case E_COOLING_TIMEOUT:          // Cooling timeout
                case E_COOLING_WAIT_TIMEOUT:     // Cooling wait timeout
                    if(t0tick1 & 0x01){
                        LED1_EN = ON;
                        LED2_EN = OFF;
                    }
                    else {
                        LED2_EN = ON;
                        LED1_EN = OFF;
                    }
                    break;
//                case E_INIT:
//                    break;
                default:
                    break;
            }
        }
    }//end if(T0IE && T0IF)
}

//---------------------------------------------------------------
//Function: main(void)
//
void main(void)
{
    Init();                                     //system initialization
#ifdef __DEBUG
    debug_count = 0;
#endif

	//serial port init
    init_comms();

    DIAG(SPEN, put_string("\r\nRemould version \0"));
    DIAG(SPEN, put_ascii(VERSION_));
    DIAG(SPEN, put_string(".\0"));
    DIAG(SPEN, put_ascii(VERSION_minor));
    DIAG(SPEN, put_string("\r\n\0"));

#ifdef __DEBUG
    while(1){
        DIAG(SPEN, put_string("\r\nRemould loop \0"));
        DIAG(SPEN, put_ascii(debug_count));
        DIAG(SPEN, put_string(" \r\n\0"));        
#endif
        while((state == S_START));
        main_process();

#ifdef __DEBUG
        state = S_START;
        minute = 0;
        debug_count++;
    }
#endif
    while(1);
}

//---------------------------------------------------------------
//Function: main_process(void)
//
void main_process(void)
{
    state = S_INIT;
    if(sensor_init()) goto End_Program;
    DIAG(SPEN, put_string("\tSensor init OK! Start heating.\0"));

// heating phase 1
    state = S_HEATING;
    if(Remould_Phrase())goto End_Program;
    DIAG(SPEN, put_string("\tHeating OK! Start heating wait.\0"));
// heating phase 2
    state = S_HEATING_WAIT;
    if(Remould_Phrase())goto End_Program;
    DIAG(SPEN, put_string("\tHeating wait OK! Start heating again.\0"));
// heating phase 3
    state = S_REHEATING;
    if(Remould_Phrase())goto End_Program;
    DIAG(SPEN, put_string("\tHenting again OK! Start cooling.\0"));
// cooling phase 1
    state = S_COOLING;
    if(Remould_Phrase())goto End_Program;
    DIAG(SPEN, put_string("\tCooling OK! Start cooling wait.\0"));
// cooling phase 2
    state = S_COOLING_WAIT;
    if(Remould_Phrase())goto End_Program;
    DIAG(SPEN, put_string("\tCooling wait OK! Remould Finished.\0"));
// Remould finished
    state = S_FINISH;
    Remould_Phrase();

End_Program:
#ifdef __DEBUG
    idle_minute = minute;
#endif
    while(1){
        Remould_Phrase();
        if(tempreture0[1] < tlevel[3]) {
            fan_off();
#ifdef __DEBUG
            if(minute - idle_minute) break;
#endif
        } else {
            fan_on();
        }
    }
    return;
}

//---------------------------------------------------------------
//Function: fan_off(void)
//
void fan_off(void)
{
    FAN_SWITCH = 0;
	RX_PIN = 1;	
	TX_PIN = 1;	
    SPEN = 1;
    return;
}

//---------------------------------------------------------------
//Function: fan_on(void)
//
void fan_on(void)
{
	RX_PIN = 0;
	TX_PIN = 0;
    SPEN = 0;
    TXD = 1;
    FAN_SWITCH = 1;
    return;
}

//---------------------------------------------------------------
//Function: Remould_Phrase(void)
//
UCHAR Remould_Phrase(void)
{
    phase_timer = minute;
    
    while(1){
        if(Flags.t1s){
            Flags.t1s = 0;
            if(Read_Tempreture(tempreture0)) goto Remould_Error;
            DIAG(SPEN, put_string("\r\n\0"));
#ifdef __DEBUG
            DIAG(SPEN, put_ascii(debug_count));
            DIAG(SPEN, put_string("\t\0"));
#endif            
            DIAG(SPEN, put_ascii(state));
            DIAG(SPEN, put_string(" \t\0"));

            DIAG(SPEN, put_ascii(minute - phase_timer));
            DIAG(SPEN, put_string(" \t\0"));

            DIAG(SPEN, put_ascii(hours));
            DIAG(SPEN, put_string(":\0"));

            DIAG(SPEN, put_ascii(minute));
            DIAG(SPEN, put_string(":\0"));

            DIAG(SPEN, put_ascii(t0tick1>>1));
            DIAG(SPEN, put_string("\t\0"));

            itoa(strbuf, tempreture0[1], 10);
            DIAG(SPEN, put_string(strbuf));
            DIAG(SPEN, put_string(".\0"));
            
//            itoa(strbuf, (tempreture0[0]*625), 10);
//            DIAG(SPEN, put_string(strbuf));
//            DIAG(SPEN, put_string("\r\n\0"));
            if(tempreture0[0]) {
                DIAG(SPEN, put_string("50 \0"));
            } else {
                DIAG(SPEN, put_string("00 \0"));
            }
            
//            DIAG(SPEN, put_string("%x\t%d\t%02d:%02d:%02d\t%d.%d\r\n", state, (minute - phase_timer), hours, minute, (t0tick1>>1), tempreture0[1], (tempreture0[0]*625)));
            __delay_ms(50);
            switch(state){
                case S_HEATING:
                    AC_SWITCH = 1;
                    if(tempreture0[1] >= tlevel[0]) return(0);
                    if(minute > (phase_timer+T1TIMELIMIT)) {
                        state = E_HEATING_TIMEOUT;
                    }
                    break;

                case S_HEATING_WAIT:
                    AC_SWITCH = 0;
                    if(tempreture0[1] <= tlevel[1]) return(0);
                    if(minute > (phase_timer+T2TIMELIMIT)) {
                        state = E_HEATING_WAIT_TIMEOUT;
                    }
                    break;

                case S_REHEATING:
                    AC_SWITCH = 1;
                    if(tempreture0[1] >= tlevel[0]) return(0);
                    if(minute > (phase_timer+T3TIMELIMIT)) {
                        state = E_REHEATING_TIMEOUT;
                    }
                    break;

                case S_COOLING:
                    AC_SWITCH = 0;
                    fan_on();
                    if(tempreture0[1] <= tlevel[2]) {
                        fan_off();
                        return(0);
                    }
                    if(minute > (phase_timer+T4TIMELIMIT)) {
                        state = E_COOLING_TIMEOUT;
                    }
                    if(Flags.t1m){
                        Flags.t1m = 0;
                        fan_off();
                    }
                    break;

                case S_COOLING_WAIT:
                    AC_SWITCH = 0;
                    fan_on();
                    if(tempreture0[1] <= tlevel[3]) {
                        fan_off();
                        return(0);
                    }
                    if(minute > (phase_timer+T5TIMELIMIT)) {
                        state = E_COOLING_WAIT_TIMEOUT;
                    }
                    if(Flags.t1m){
                        Flags.t1m = 0;
                        fan_off();
                    }
                    break;

                case S_FINISH:
                    fan_off();
                    AC_SWITCH = 0;
                    return(0);

                default:
                    goto Remould_Error;

            }
        }
    }
Remould_Error:
    AC_SWITCH = 0;
    return(1);
}

//---------------------------------------------------------------
//Function: Read_Tempreture(UCHAR *)
//
UCHAR Read_Tempreture(UCHAR * tdata)
{
    int t;
    UCHAR i;
    UCHAR temp;

    temp = state;
    for(i=0; i<15; i++){
        __delay_ms(50);
        if(Ds18b20_reset()) continue;
        Ds18b20_Skip_Rom();
        if(Ds18b20_Convert_T()) continue;
        if(Ds18b20_reset()) continue; 
        Ds18b20_Skip_Rom();
        if(Ds18b20_Read_Scratchpad(tdata)) continue;
//        DIAG(SPEN, put_string("Read back tempreture: %02X:%02X\r\n", tdata[1], tdata[0]));
        t = tdata[1]<<8 | tdata[0];
        if(t>=0){
//            DIAG(SPEN, put_string("%3d.%04d\r\n", t>>4, (t & 0x0f)*625));
        }else{
            t = ~t + 1;
//            DIAG(SPEN, put_string("-%3d.%04d\r\n", t>>4, (t & 0x0f)*625));
        }
        if(Ds18b20_reset()) continue; 
        tdata[1] = (unsigned char) ( t>>4 );
        tdata[0] = (unsigned char) (t & 0x0f);
        state = temp;
        return(0);
    }
    return(1);
}

//---------------------------------------------------------------
//Function: sensor_init(void)
//
UCHAR sensor_init(void)
{
    UCHAR i;
    UCHAR j;

// Config of DS18B20
    scratchpad[0]=0x00;
    scratchpad[1]=0x00;
    scratchpad[2]=0x1F;
    for(i=0; i<15; i++){
        __delay_ms(50);
//detect device
        if(Ds18b20_reset())continue;
        if(Ds18b20_Read_Rom(rom0))continue;
        DIAG(SPEN, put_string("\r\nDS18B20 Number: \0"));
        for(j=0; j<7; j++){
            DIAG(SPEN, put_ascii(rom0[j]));
            DIAG(SPEN, put_string(" \0"));     
        }
        DIAG(SPEN, put_string(" \r\n\0"));     
        
        if(Ds18b20_reset())continue;
        Ds18b20_Skip_Rom();
        Ds18b20_Write_Scratchpad(scratchpad);
        if(Ds18b20_reset())continue;
        Ds18b20_Skip_Rom();
        if(Ds18b20_Read_Scratchpad(tempreture0))continue;
        if(Ds18b20_reset())continue;
        Ds18b20_Skip_Rom();
        Ds18b20_Copy_Scratchpad();
        if(Ds18b20_reset())continue;
        return(0);
    }
    return(1);
}


//---------------------------------------------------------------
//Function: DelayMs(unsigned char msec)
//
//input: msec, 0-255
void DelayMs(unsigned char msec)
{
    unsigned char i;
    do {
       for(i=0; i<8; i++)
        DelayUs(125);
    }while(--msec);
}

//---------------------------------------------------------------
//Function: DelaySec(unsigned char Dsec)
//
//input: Dsec, 0-255
void DelaySec(unsigned char Dsec)
{
    UCHAR i;
    do {
       for(i=0; i<10; i++)
        DelayMs(100);
    }while(--Dsec);
}

//---------------------------------------------------------------
//Function: Init(void)
// Purpose: Initialize PORTS, Capacitive Sensing Module and Selected Time Base
void Init(void) 
{
    OSCCON  = 0x20;                 //internal clock, 8M
	OPTION = OPTION_VALUE;          // OPTION_REG: RBPU=dis,INTEDG=falling,T0CS=FOSC/4,T0SE=0,PSA=timer0,PS=16

	INTCON = 0x00;      	        //disable interrupts ,GIE--PEIE--T0IE--INTE--RBIE--T0IF--INTF--RBIF(bit7---bit0)
    CLRWDT();                       //clear watchdog
	OSCTUNE = 0b00111111;
// PORT setup
// Pin usage for this sample application
//--------------------------------------------------------------------------
//			7		6		5		4		3		2		1		0      |
// PORTA	.       .       CPS7    CPS6    .       .       LED     LED    |    
// PORTB    .       .       CPS5    CPS4    CPS3    CPS2    CPS1    CPS0   |
// PORTC	RX      TX      .       .       .       .       .       .      | 
// PORTD	CPS15   CPS14   CPS13   CPS12   CPS11   CPS10   CPS9    CPS8   |
// PORTE	X		X		X		X		.		LED		.		LED    |
//--------------------------------------------------------------------------
    TRISC = 0b00110001;             //RX set PORTC input/output mode,1=input,0=output

    TRISA = 0b11111111;             //ping configured as 1=input,0=output mode
    ANSELA = 0b00000000;            // o=digital I/O, 1=analog input

	WPUB = 0x00;			        //disable portB weak pull_up
	TRISB = 0b00111111;             //
	ANSELB = 0b00000000;            //set as digital port
//clear PORT
    PORTA = 0xFF;
    PORTB = 0xFF;

    FAN_SWITCH = 0;
    AC_SWITCH = 0;
    
    PORTC = 0xFF;
    
    Init_var();

    timerInit();

    INTE = 0;                   //disable RB0/INT interrupt
    GIE = 1;                    //enable global interrupt
#ifdef __DEBUG
    DEBUG1 = 0;
    DEBUG2 = 1;
#endif
}

//---------------------------------------------------------------
//Function: timerInit(void)
// Purpose: Initialize Time Base Modules
void timerInit(void)
{
	TMR0 = TMR0_VALUE;			//Timer 0 should be 160 cycles interrupt, (256/2000)*78 ms=10 ms
	t0tick = 0;
    t0tick1 = 0;
    hours = 0;
    minute =0;
	T0IE = 1;                   //Enable timer0 interrupt
}

//---------------------------------------------------------------
//Function: Init_var(void)
// Purpose: Initialize the var
void Init_var(void)
{
    Flags.t500ms = 0;
    Flags.t1s = 0;
    state = S_START;
    if(SWITCH){
        tlevel[0] = T1HIGH;
        tlevel[1] = T2HIGH;
        tlevel[2] = T3HIGH;
        tlevel[3] = T4HIGH;
    }else{
        tlevel[0] = T1LOW;
        tlevel[1] = T2LOW;
        tlevel[2] = T3LOW;
        tlevel[3] = T4LOW;
    }
}

//---------------------------------------------------------------
//end file
