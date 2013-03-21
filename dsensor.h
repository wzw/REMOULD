#ifndef __DSENSOR_H_
#define	__DSENSOR_H_

//typedef     unsigned char       UCHAR;
//typedef     char                CHAR;

#ifndef _XTAL_FREQ
 // Unless specified elsewhere, 8MHz system frequency is assumed
 #define _XTAL_FREQ 8000000
#endif

#define TRUE	0x01
#define FALSE	0x00

//#define DQ	RC0
#define	DQ	RA6				// DS18B20 1-wire bus
#define	DQ_TRIS	TRISA6

#define DQ_LL   RA6=0,TRISA6=0
#define DQ_HIZ  TRISA6=1

// ROM FUNCTION COMMANDS
#define CMD_READROM         0x33
#define CMD_MATCHROM        0x55
#define CMD_SEARCHROM       0xF0
#define CMD_ALARMSEARCH     0xEC
#define CMD_SKIPROM         0xCC

// DS18B20 FUNCTION COMMANDS
#define CMD_CONVERTT        0x44
#define CMD_COPYSCRATCHPAD  0x48
#define CMD_READPOWERSUPPLY 0xB4
#define CMD_RECALLE2        0xB8
#define CMD_READSCRATCHPAD  0xBE
#define CMD_WRITESCRATCHPAD 0x4E

// Static Var

extern UCHAR state;

extern void time_delay (int uSeconds);
extern unsigned char Ds18b20_reset(void);
extern unsigned char read_bit (void);
extern void write_bit (char b);
extern unsigned char read_byte (void);
extern void write_byte (char val);

extern UCHAR Ds18b20_Check_CRC(UCHAR , UCHAR *);
extern UCHAR Ds18b20_Read_Rom(UCHAR *);
extern void Ds18b20_Match_Rom(UCHAR *);
extern UCHAR Ds18b20_Search_Rom(void);
extern UCHAR Ds18b20_Alarm_Search(void);
extern void Ds18b20_Skip_Rom(void);

extern UCHAR Ds18b20_Convert_T(void);
extern void Ds18b20_Copy_Scratchpad(void);
extern void Ds18b20_Read_Power_Supply(void);
extern void Ds18b20_Recall_E2(void);
extern UCHAR Ds18b20_Read_Scratchpad(UCHAR * );
extern void Ds18b20_Write_Scratchpad(UCHAR * );

#endif
