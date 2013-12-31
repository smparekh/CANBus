#include <plib.h>
#include "UART.h"
#include "HardwareProfile.h"

void SendZigbeeMessage(UINT8* message, UINT8 length)
{
 Zigbee msg;

 //Should not change......
 msg.Delimiter = 0x7E;
 ///////////////////////

 int lth = 5 + length;

 msg.OPTIONS = 0x00;
 msg.LENGTH_MSB = ((lth & 0x0FF00) >> 8);
 msg.LENGTH_LSB = (lth & 0x0FF);
 msg.API_ID = 0x01;
 msg.FRAMEID = 0x01;
 msg.DEST_MSB = 0x50;
 msg.DEST_LSB = 0x01;
 UINT8 * payload[length];
 int i = 0;
 for(i = 0; i < length; i++)
 {
     msg.DATA[i] = message[i];
 }
 msg.CHECKSUM = CalculateChecksum(&msg,length);
 UINT8 serialMessage[length + 9];SerializeMessage(&msg,length,serialMessage);

 for(i = 0; i < (length + 9); i++)
 {
     PutCharacter((char)serialMessage[i]);
 }


}
UINT8 CalculateChecksum(Zigbee* msg, int payloadLength)
{
    UINT8 Check = 0x00;

    Check += msg->API_ID;
    Check += msg->FRAMEID;
    Check += msg->DEST_MSB;
    Check += msg->DEST_LSB;
    Check += msg->OPTIONS;
    int i = 0;
    for(i = 0; i < (payloadLength); i++)
    {
        Check += msg->DATA[i];
    }

   return (0xFF - (Check & 0x0FF));

}
void SerializeMessage(Zigbee* msg,UINT8 length, UINT8 * result)
{

    int i = 0;
    result[i++] = msg->Delimiter;
    result[i++] = msg->LENGTH_MSB;
    result[i++] = msg->LENGTH_LSB;
    result[i++] = msg->API_ID;
    result[i++] = msg->FRAMEID;
    result[i++] = msg->DEST_MSB;
    result[i++] = msg->DEST_LSB;
    result[i++] = msg->OPTIONS;
    int x = 0;
    for(x = 0; x < (length); x++)
    {
        result[i++] = msg->DATA[x];
    }
    result[i++] = msg->CHECKSUM;


}

void initUART()
{
    UARTConfigure( UART1, UART_ENABLE_PINS_TX_RX_ONLY );
    UARTSetFifoMode( UART1, UART_INTERRUPT_ON_RX_NOT_EMPTY );
    UARTSetLineControl( UART1, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1 );
    UARTSetDataRate ( UART1 , GetPeripheralClock(), UART_BAUD_RATE );
    UARTEnable( UART1, UART_ENABLE_FLAGS( UART_PERIPHERAL | UART_RX | UART_TX ) );

    INTEnable(INT_SOURCE_UART_RX(UART1), INT_ENABLED);
    INTSetVectorPriority(INT_VECTOR_UART(UART1), INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART1), INT_SUB_PRIORITY_LEVEL_0);
}
void WriteString(const char *string)
{
    while(*string != '\0')
    {
        while(!UARTTransmitterIsReady(UART1))
            ;

        UARTSendDataByte(UART1, *string);

        string++;

        while(!UARTTransmissionHasCompleted(UART1))
            ;
    }
}
void PutCharacter(const char character)
{
        while(!UARTTransmitterIsReady(UART1));

        UARTSendDataByte(UART1, character);


        while(!UARTTransmissionHasCompleted(UART1));
        mPORTGToggleBits(BIT_12);
}
void __ISR(_UART1_VECTOR, ipl2) UART_int_handler(void) {
    // Is this an RX interrupt?
    if (INTGetFlag(INT_SOURCE_UART_RX(UART1))) {
        // Clear the RX interrupt Flag
        INTClearFlag(INT_SOURCE_UART_RX(UART1));

        // Do Something with the data
        // Toggle LED to indicate UART activity
        mPORTGToggleBits(BIT_15);
        

    }

    // We don't care about TX interrupt
    if (INTGetFlag(INT_SOURCE_UART_TX(UART1))) {
        INTClearFlag(INT_SOURCE_UART_TX(UART1));
    }
}