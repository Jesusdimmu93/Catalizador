#include "CanNm.h"

/*
  CAN Network manager
  1) Additional function to execute the CAN messages
*/

uint8_t CAN_MSG_Counter = 0;

void CanNm_Tx(void)
{
  if(CAN_MSG_Counter >= CAN_WAIT)
  {
    CAN_MSG_Counter = 0;
    CanIf_Transmit(ControllerId_1 ,CAN_MSGID_0);               /* Send CAN tx message */
  }
  else
  {
    CAN_MSG_Counter++;
  }
}

uint8_t Can_SetSignal(uint8_t Signal, uint8_t SignalValue)
{
  CanMsgObj.CanIfMessageConfig[0].CanPdu.CanSdu[Signal] = SignalValue;  /* Update SDU */
  return 0; /* OK = 0 */
}