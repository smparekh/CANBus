#include <plib.h>
#include "UART.h"
#include "HardwareProfile.h"


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
        WriteString("Mike\0");

    }

    // We don't care about TX interrupt
    if (INTGetFlag(INT_SOURCE_UART_TX(UART1))) {
        INTClearFlag(INT_SOURCE_UART_TX(UART1));
    }
}