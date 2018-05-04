#include "CanIf.h"

CanIf_MessageConfigType *ptrCanIfMessageConfig;
uint8_t CanNumberOfMsgs;
CanIf_PduType CanIf_Pdu;

const MCan_ConfigType *LogChannelId[] =
{
  &mcan0Config,
  &mcan1Config
};

/*
  CAN Initialization:
  1) This function shall initialize the Can channel according to CanChannelId parameter.
  2) Additionally, the function shall configure the CAN TX Messages 
              as per the message configuration structure shown below.
*/
void CanIf_Init(uint8_t CanChannelId, CanIf_MsgObjType CanIfMsgConfig)
{  
  CanNumberOfMsgs = CanIfMsgConfig.CanNumberOfMsgs;
  ptrCanIfMessageConfig =  CanIfMsgConfig.CanIfMessageConfig;
  const MCan_ConfigType *lcConfig; 
  uint8_t i = 0u;
  
  lcConfig = LogChannelId[CanChannelId];  /* Gets the CAN configuration of the CAN channel to be used */

  MCAN_Init(lcConfig);        /* Initializes the MCAN hardware */
  MCAN_Enable(lcConfig);
  
  for( i = 0; i< CanNumberOfMsgs; i++ )   /* Get and set the RAM address for SDU's */
  {
    CanIf_PduType *CanPdu = &ptrCanIfMessageConfig[i].CanPdu;     /* Gets the address of every CAN PDU */                                                       
    CanPdu->CanSdu = MCAN_ConfigTxDedBuffer(lcConfig,i, CanPdu->CanId, CanPdu->CanIdType, CanPdu->CanDlc );   /* Gets the RAM address for every CAN Tx SDU */
  }
  //MCAN_ConfigRxBufferFilter(lcConfig, RX_BUFFER_0, FILTER_0, MSG_ID_0,CAN_STD_ID);
  //MCAN_ConfigRxClassicFilter(lcConfig, CAN_FIFO_0, FILTER_1, MSG_ID_2,CAN_STD_ID, MSG_ID_2_MASK);
}

/*
  CAN Transmission:

   1) This function shall transmit the Can Message according to MsgId 
              on the CAN Bus as per the CanChannelId parameter.
*/
void CanIf_Transmit(uint8_t CanChannelId, uint8_t MsgId)
{
  CanIf_PduType *Pdu;
  const MCan_ConfigType *lcConfig;   
  uint8_t i = 0u;
    
  lcConfig = LogChannelId[CanChannelId];  /* Gets the CAN configuration of the CAN channel to be used */
    
  for(i = 0; i < CanNumberOfMsgs; i++)
  {
    if(MsgId ==  ptrCanIfMessageConfig[i].CanMsgIdNumber)
    {
      MCAN_SendTxDedBuffer(lcConfig,i);
    }
  }
}        

void CanIf_Receive(uint8_t CanChannelId, uint8_t MsgId)
{
  CanIf_PduType *Pdu;
  const MCan_ConfigType *lcConfig;   
  uint8_t i = 0u;
    
  lcConfig = LogChannelId[CanChannelId];  /* Gets the CAN configuration of the CAN channel to be used */
    
  /* Poll for new CAN messages in RX FIFO *
	for(i = 0; i < CanNumberOfMsgs; i++)
  {
		fifo_entries = MCAN_GetRxFifoBuffer(&mcan0Config, CAN_FIFO_0,(Mailbox64Type *) &rxMailbox2);
	  if (fifo_entries > 0)
      rxdCntr++;
  } while (fifo_entries > 1);
  */
}
