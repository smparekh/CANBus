/****************************************************
*       This file was created by                    *
*       Microchip Application Maestro               *
*       and holds the set-up parameters             *
*       specified when the code was generated.      *
*       The user is advised not to make any         *
*       changes to this file                        *
****************************************************/
#define BAUD_RATE_PRESC 4
#define SJW_TIME 1
#define SAMPLES 1
#define PROP_TIME 1
#define PH_SEG_1 1
#define PH_SEG_2 1
#define CAN_CONFIG_1 BAUD_RATE_PRESC-1|(SJW_TIME-1<<6)
#if SAMPLES == 1
#define CAN_CONFIG_2 0x80|(PH_SEG_1-1<<3)|(PROP_TIME-1)
#elif SAMPLES == 3
#define CAN_CONFIG_2 0xC0|(PH_SEG_1-1<<3)|(PROP_TIME-1)
#else
#error "Number of samples is out of range"
#endif
#define CAN_CONFIG_3 PH_SEG_2-1
#define RXMASK0 0
#define RXMASK1 0
#define RXFILT0 0
#define ST_FILTER_0
#define RXFILT1 0
#define ST_FILTER_1
#define RXFILT2 0
#define ST_FILTER_2
#define RXFILT3 0
#define ST_FILTER_3
#define RXFILT4 0
#define ST_FILTER_4
#define RXFILT5 0
#define ST_FILTER_5
#define LPBACK
#define MY_IDENT 0
#define STD_IDENT
#define RX_BUFFER 4
#define TX_BUFFER 4
//#define CAN_INT_LOW
//#define CAN_ERROR_HANDLER_ENABLE
