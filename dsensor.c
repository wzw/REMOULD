#include	<htc.h>
#include	<stdio.h>
#include    "remould_def.h"
#include	"usart.h"
#include	"dsensor.h"

// GLOBAL Defines
/*==================================================================================
**
** The 16F684 should be running at 8MHz using the internal oscillator
** Not sure at this stage how long it takes to run or what each count is
**
** =================================================================================
*/


/*==================================================================================
**
** Each communication cycle must begin with a reset from the 16F684
** To do this the port needs to be pulled low for 480uS then released
** The DS18B20 will respond by pulling the port low to indicate it's present
**
** 0 indicates device present, 1 indicates no device present
**
** DQ is on RC0
**
** Time Slot (Min 60uS - Max 120uS)
** Recovery Time - 1uS
** Reset Time (Min 480uS - Max 960uS)
** Present Detect 60uS
**
** One cycle appears to be 12uS
**
** =================================================================================
*/

/*==================================================================================
**
** DS18B20_reset - this should ideally reset the DS18B20 and recieve a Presence Pulse
**
** =================================================================================
*/

unsigned char Ds18b20_reset(void)
{
	unsigned char present;
    
	GIE = 0;			                        // disable all interrupt
	DQ_TRIS = 0;
	DQ = 0;				                        // Pull DQ line Low
	__delay_us(480);
	DQ_TRIS = 1;
    __delay_us(62);
	DQ = 1; 			                        // Allow line to return high
	present = DQ; 		                        // Get present signal
	GIE = 1;			                        // Enable interrupt	
	__delay_us(400);
    if(present) state = E_NO_DS18B20;

	return (present); 	                        // Present signal returned
}

/*==================================================================================
**
** READ_BIT - the time_delay required for a read bit is 15uS
**
** =================================================================================
*/

unsigned char read_bit(void)
{
    static bit resultB;

    DQ_LL;
    __delay_us(1);
    DQ_HIZ;
    __delay_us(14);
    resultB = DQ;
    return (resultB);
}

/*==================================================================================
**
** WRITE_BIT - writes a bit to the one-wire bus
**
** =================================================================================
*/

void write_bit (char b)
{
    DQ_LL;
    __delay_us(1);
    if(b)
       DQ_HIZ;
    __delay_us(60);
    DQ_HIZ;
}

/*==================================================================================
**
** READ_BYTE - reads a byte from the one-wire bus
**
** =================================================================================
*/

unsigned char read_byte (void)
{
	unsigned char i;
	unsigned char value = 0;
    
	CLRWDT();
   	for (i=0; i<8; i++) 	                    // reads in 8 bits of data
	{
        value = value >> 1;
		if (read_bit ()) 
//            value |= 0x01 << i;                 // reads data in and shifts it left
        value |= 0x80;
		__delay_us(150);
	}
	return (value); 		                    // return value
}

/*==================================================================================
**
** WRITE_BYTE - writes a byte to the one-wire bus
**
** =================================================================================
*/

void write_byte (char val)
{

	unsigned char i;
	unsigned char temp;

	CLRWDT();
	for (i=0; i<8; i++) 	                    // writes byte one bit at a time
	{
//		temp = val >> i;	                    // shifts val right i spaces
        temp = val;
        val = val >> 1;
		temp &= 0x01;		                    // copy that bit to temp
		write_bit (temp);	                    // write bit
	}
	__delay_us(100);
}
/*==================================================================================
**
** Ds18b20_Read_Rom - 
**
This command can only be used when there is one slave on the bus. 
It allows the bus master to read the slave¡¯s 64-bit ROM code without using the Search ROM procedure. 
If this command is used when there is more than one slave present on the bus, a data collision will 
occur when all the slaves attempt to respond at the same time.
** =================================================================================
*/
UCHAR Ds18b20_Check_CRC(UCHAR length, UCHAR * data)
{
    UCHAR i, j;
    UCHAR in, crc, temp;

    crc = 0;
    for(j=0; j<length; j++){
        in = data[j];
        for (i=0; i<8; i++){
            temp = crc & 0x01;
            temp ^= in & 0x01;
            if(temp){
                temp = 0b10001100;
            }else{
                temp = 0;
            }
            crc >>= 1;
            crc ^= temp; 
            in >>= 1;
//            DIAG(SPEN, put_string(" %02X", crc));
        }
//        DIAG(SPEN, put_string("\r\n\0"));
    }
    return(crc);
}

/*==================================================================================
**
** Ds18b20_Read_Rom - 
**
This command can only be used when there is one slave on the bus. 
It allows the bus master to read the slave¡¯s 64-bit ROM code without using the Search ROM procedure. 
If this command is used when there is more than one slave present on the bus, a data collision will 
occur when all the slaves attempt to respond at the same time.
** =================================================================================
*/
UCHAR Ds18b20_Read_Rom(UCHAR * r)
{
    UCHAR i;

	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_READROM);

    for (i=0; i<8; i++){
        r[i] = read_byte();
    }

    if(Ds18b20_Check_CRC(8, r)){
        DIAG(SPEN, put_string("\tDs18b20_Read_Rom CRC error!\r\n\0")); 
        i = 1;
    }else{
        i = 0;
    }
	GIE = 1;			                        // Enable interrupt	
    return(i);
}

/*==================================================================================
**
** Ds18b20_Match_Rom
**
The match ROM command followed by a 64-bit ROM code sequence allows the bus master to 
address a specific slave device on a multidrop or single-drop bus. Only the slave that 
exactly matches the 64-bit ROM code sequence will respond to the function command 
issued by the master; all other slaves on the bus will wait for a reset pulse.
** =================================================================================
*/
void Ds18b20_Match_Rom(UCHAR * r)
{
    UCHAR i;

	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_MATCHROM);
    for (i=0; i<8; i++){
        write_byte(r[i]);
    }

	GIE = 1;			                        // Enable interrupt	
    return;
}

/*==================================================================================
**
** Ds18b20_Search_Rom
**
When a system is initially powered up, the master must identify the ROM codes of all 
slave devices on the bus, which allows the master to determine the number of slaves 
and their device types. The master learns the ROM codes through a process of elimination 
that requires the master to perform a Search ROM cycle (i.e., Search ROM command followed 
by data exchange) as many times as necessary to identify all of the slave devices. If 
there is only one slave on the bus, the simpler Read ROM command (see below) can be 
used in place of the Search ROM process. For a detailed explanation of the Search ROM 
procedure, refer to the iButton® Book of Standards at www.maxim-ic.com/ibuttonbook. 
After every Search ROM cycle, the bus master must return to Step 1 (Initialization) 
in the transaction sequence.
** =================================================================================
*/
UCHAR Ds18b20_Search_Rom(void)
{
	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_SEARCHROM);

    read_bit();
    read_bit();
    write_bit(0);
    
	GIE = 1;			                        // Enable interrupt	
    return(0);
}

/*==================================================================================
**
** Ds18b20_Alarm_Search
**
The operation of this command is identical to the operation of the Search ROM command 
except that only slaves with a set alarm flag will respond. This command allows the 
master device to determine if any DS18B20s experienced an alarm condition during the 
most recent temperature conversion. After every Alarm Search cycle (i.e., Alarm Search 
command followed by data exchange), the bus master must return to Step 1 (Initialization) 
in the transaction sequence. See the Operation¡ªAlarm Signaling section for an explanation 
of alarm flag operation.
** =================================================================================
*/
UCHAR Ds18b20_Alarm_Search(void)
{
	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_ALARMSEARCH);
	GIE = 1;			                        // Enable interrupt	
    return(0);
}

/*==================================================================================
**
** Ds18b20_Skip_Rom
**
The master can use this command to address all devices on the bus simultaneously without 
sending out any ROM code information. For example, the master can make all DS18B20s on 
the bus perform simultaneous temperature conversions by issuing a Skip ROM command followed 
by a Convert T [44h] command.
Note that the Read Scratchpad [BEh] command can follow the Skip ROM command only if there 
is a single slave device on the bus. In this case, time is saved by allowing the master 
to read from the slave without sending the device¡¯s 64-bit ROM code. A Skip ROM command 
followed by a Read Scratchpad command will cause a data collision on the bus if there is 
more than one slave since multiple devices will attempt to transmit data simultaneously.
** =================================================================================
*/
void Ds18b20_Skip_Rom(void)
{
	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_SKIPROM);
	GIE = 1;			                        // Enable interrupt	
    return;
}

/*==================================================================================
**
** Ds18b20_Convert_T
**
This command initiates a single temperature conversion. Following the conversion, the 
resulting thermal data is stored in the 2-byte temperature register in the scratchpad 
memory and the DS18B20 returns to its low-power idle state. If the device is being 
used in parasite power mode, within 10¦Ìs (max) after this command is issued the master 
must enable a strong pullup on the 1-Wire bus for the duration of the conversion (tCONV) 
as described in the Powering the DS18B20 section. If the DS18B20 is powered by an external 
supply, the master can issue read time slots after the Convert T command and the DS18B20 
will respond by transmitting a 0 while the temperature conversion is in progress and a 1 
when the conversion is done. In parasite power mode this notification technique cannot be 
used since the bus is pulled high by the strong pullup during the conversion.
** =================================================================================
*/
UCHAR Ds18b20_Convert_T(void)
{
    UCHAR i;

	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_CONVERTT);
    
	CLRWDT();
    i = 0;                                      // Max time out 1 second
    while(read_bit()){
        i++;
        if(i > 100) {
            state = E_DS18B20_NO_RESPONSE;
            DIAG(SPEN, put_string("\tDs18b20_Convert_T No response.\0"));
            goto Error_Convert_T;
        }
        __delay_ms(8);
    	CLRWDT();
    }

    for (i=0; i<100; i++){                       // Max time out 1 second
    	CLRWDT();
        if(read_bit()){
            i = 0;
            goto Exit_Convert_T;
        }
    	__delay_ms(8);
    }
    state = E_DS18B20_TIMEOUT;
    DIAG(SPEN, put_string("\tDs18b20_Convert_T Time Out!\0"));
Error_Convert_T:
    i = 1;
Exit_Convert_T:
	GIE = 1;			                        // Enable interrupt	
    return(i);
}

/*==================================================================================
**
** Ds18b20_Copy_Scratchpad
**
This command copies the contents of the scratchpad TH, TL and configuration registers 
(bytes 2, 3 and 4) to EEPROM. If the device is being used in parasite power mode, within 
10¦Ìs (max) after this command is issued the master must enable a strong pullup on the 
1-Wire bus for at least 10ms as described in the Powering the DS18B20 section.
** =================================================================================
*/
void Ds18b20_Copy_Scratchpad(void)
{
	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_COPYSCRATCHPAD);
    __delay_ms(10);
	GIE = 1;			                        // Enable interrupt	
    return;
}

/*==================================================================================
**
** Ds18b20_Read_Power_Supply
**
The master device issues this command followed by a read time slot to determine if 
any DS18B20s on the bus are using parasite power. During the read time slot, parasite 
powered DS18B20s will pull the bus low, and externally powered DS18B20s will let the 
bus remain high. See the Powering the DS18B20 section for usage information for this 
command.
** =================================================================================
*/
void Ds18b20_Read_Power_Supply(void)
{
	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_READPOWERSUPPLY);
	GIE = 1;			                        // Enable interrupt	
    return;
}

/*==================================================================================
**
** Ds18b20_Recall_E2
**
This command recalls the alarm trigger values (TH and TL) and configuration data from 
EEPROM and places the data in bytes 2, 3, and 4, respectively, in the scratchpad memory. 
The master device can issue read time slots following the Recall E2 command and the 
DS18B20 will indicate the status of the recall by transmitting 0 while the recall is 
in progress and 1 when the recall is done. The recall operation happens automatically 
at power-up, so valid data is available in the scratchpad as soon as power is applied 
to the device.
** =================================================================================
*/
void Ds18b20_Recall_E2(void)
{
	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_RECALLE2);
	GIE = 1;			                        // Enable interrupt	
    return;
}

/*==================================================================================
**
** Ds18b20_Read_Scratchpad
**
This command allows the master to read the contents of the scratchpad. The data transfer 
starts with the least significant bit of byte 0 and continues through the scratchpad 
until the 9th byte (byte 8 ¨C CRC) is read. The master may issue a reset to terminate 
reading at any time if only part of the scratchpad data is needed.
** =================================================================================
*/
UCHAR Ds18b20_Read_Scratchpad(UCHAR * t)
{
	UCHAR get[10];
	UCHAR k;
    
	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_READSCRATCHPAD);

	for (k=0; k<9; k++)		                    // receive & read 9 data bytes
	{
    	get[k]=read_byte();
	}
	GIE = 1;			                        // Enable interrupt	

    DIAG(SPEN, put_string("\t\0"));
	for (k=0; k<9; k++)		                // receive & read 9 data bytes
	{
        DIAG(SPEN, put_ascii(get[k]));
        DIAG(SPEN, put_string(" \0"));
	}

    if(Ds18b20_Check_CRC(9, get)){
        DIAG(SPEN, put_string("\tDs18b20_Read_Scratchpad CRC error!\t\0")); 
        state = E_DS18B20_CRC;
        return(1);
    }
    
    t[0] = get[0];
    t[1] = get[1];
    return(0);
}

/*==================================================================================
**
** Ds18b20_Write_Scratchpad
**
This command allows the master to write 3 bytes of data to the DS18B20¡¯s scratchpad. 
The first data byte is written into the TH register (byte 2 of the scratchpad), the 
second byte is written into the TL register (byte 3), and the third byte is written 
into the configuration register (byte 4). Data must be transmitted least significant 
bit first. All three bytes MUST be written before the master issues a reset, or the 
data may be corrupted.
** =================================================================================
*/
void Ds18b20_Write_Scratchpad(UCHAR * data)
{
	GIE = 0;			                        // disable all interrupt
    write_byte(CMD_WRITESCRATCHPAD);
    write_byte(data[0]);
    write_byte(data[1]);
    write_byte(data[2]);
	GIE = 1;			                        // Enable interrupt	
//    DIAG(SPEN, put_string("Ds18b20_Write_Scratchpad OK!\r\n\0")); 
	GIE = 1;			                        // Enable interrupt	
	return;
}
