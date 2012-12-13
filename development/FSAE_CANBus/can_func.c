#include "can_func.h"

static volatile BOOL isCAN1MsgReceived = FALSE;

char buffer[20];

void CAN1Init(void)
{
    CAN_BIT_CONFIG canBitConfig;
    UINT baudPrescalar;
    CANEnableModule(CAN1, TRUE);
    CANSetOperatingMode(CAN1, CAN_CONFIGURATION);
    while(CANGetOperatingMode(CAN1) != CAN_CONFIGURATION);

    // Standard Values
    canBitConfig.phaseSeg2Tq = CAN_BIT_3TQ;
    canBitConfig.phaseSeg1Tq = CAN_BIT_5TQ;
    canBitConfig.propagationSegTq = CAN_BIT_1TQ;
    canBitConfig.phaseSeg2TimeSelect = TRUE;
    canBitConfig.sample3Time = TRUE;
    canBitConfig.syncJumpWidth = CAN_BIT_1TQ;

    // Set speed to 1Mbit/s
    CANSetSpeed(CAN1, &canBitConfig, SYSTEM_FREQ, CAN_BUS_SPEED);

    // 256 bytes of storage space
    CANAssignMemoryBuffer(CAN1, CAN1MessageFifoArea, (2*8*16));

    CANConfigureChannelForTx(CAN1, CAN_CHANNEL0, 8, CAN_TX_RTR_DISABLED,
            CAN_LOW_MEDIUM_PRIORITY);

    CANConfigureChannelForRx(CAN1, CAN_CHANNEL1, 8, CAN_RX_FULL_RECEIVE);

    CANConfigureFilter(CAN1, CAN_FILTER0, 0x5F1, CAN_SID);

    CANConfigureFilterMask(CAN1, CAN_FILTER_MASK0, 0x7FF, CAN_SID, CAN_FILTER_MASK_IDE_TYPE);

    CANLinkFilterToChannel(CAN1, CAN_FILTER0, CAN_FILTER_MASK0, CAN_CHANNEL1);

    CANEnableFilter(CAN1, CAN_FILTER0, TRUE);

    CANEnableChannelEvent(CAN1, CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY, TRUE);

    CANEnableModuleEvent(CAN1, CAN_RX_EVENT, TRUE);

    INTSetVectorPriority(INT_CAN_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_CAN_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_CAN1, INT_ENABLED);

    CANSetOperatingMode(CAN1, CAN_NORMAL_OPERATION);
    while(CANGetOperatingMode(CAN1) != CAN_NORMAL_OPERATION);

    
}

void CAN1RxMsgProcess(void)
{
    CANRxMessageBuffer *message;

    if(isCAN1MsgReceived == FALSE)
    {
        return;
    }

    isCAN1MsgReceived = FALSE;

    message = CANGetRxMessage(CAN1, CAN_CHANNEL1);
    lcdInstruction("0;0H");
    unsigned int sid = message->msgSID.SID;

    unsigned int data = message->data[0] << 8;
    unsigned int data2 = message->data[1];
    float temp = (data + data2) * 0.1;

    sprintf(buffer, "%u", sid);
    lcdString(buffer);

    sprintf(buffer, "%2.2fF", temp);

    lcdInstruction("1;0H");
    lcdString("ET: ");
    lcdString(buffer);

    CANUpdateChannel(CAN1, CAN_CHANNEL1);
    CANEnableChannelEvent(CAN1, CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY, TRUE);

}

void __attribute__((vector(46), interrupt(ipl4), nomips16))
CAN1InterruptHandler(void)
{
    if ((CANGetModuleEvent(CAN1) & CAN_RX_EVENT) != 0)
    {
        if(CANGetPendingEventCode(CAN1) == CAN_CHANNEL1_EVENT)
        {
            CANEnableChannelEvent(CAN1, CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY, FALSE);
            isCAN1MsgReceived = TRUE;
        }
    }
    INTClearFlag(INT_CAN1);
}