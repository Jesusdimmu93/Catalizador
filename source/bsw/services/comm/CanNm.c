#include "CanNm.h"

/*
  CAN Network manager
  1) Additional function to test the CAN IF layer
*/


void CanNm_Tx(void)
{
  CanMsgObj.CanIfMessageConfig[0].CanPdu.CanSdu[0] = 0x00;  /* Update SDU */
  CanMsgObj.CanIfMessageConfig[0].CanPdu.CanSdu[1] = 0x11;  
  CanIf_Transmit(ControllerId_1,CAN_MSGID_0);               /* Send CAN tx message */  	
}
