typedef     unsigned char       UCHAR;
typedef     char                CHAR;

#define     S_START                 0x00    // Program start state
#define     S_INIT                  0x01    // Normal start state
#define     S_HEATING               0x02    // Heating state
#define     S_HEATING_WAIT          0x03    // Heating wait state
#define     S_REHEATING             0x04    // ReHeating state
#define     S_COOLING               0x05    // Cooling state
#define     S_COOLING_WAIT          0x06    // Cooling wait state
#define     S_FINISH                0x07    // Finished state

#define     E_NO_DS18B20            0x80    // Error with no DS18B20 found
#define     E_DS18B20_CRC           0x81    // Error with DS18B20 read CRC Error
#define     E_DS18B20_NO_RESPONSE   0x82    // Error with no DS18B20 found
#define     E_DS18B20_TIMEOUT       0x83    // Error with DS18B20 timeout
#define     E_HEATING_TIMEOUT       0x84    // Heating timeout
#define     E_HEATING_WAIT_TIMEOUT  0x85    // Heating wait timeout
#define     E_REHEATING_TIMEOUT     0x86    // ReHeating timeout
#define     E_COOLING_TIMEOUT       0x87    // Cooling timeout
#define     E_COOLING_WAIT_TIMEOUT  0x88    // Cooling wait timeout

#define	DIAG(a,b) if(a)b
