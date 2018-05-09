#ifndef _CAN_NTW_
#define _CAN_NTW_

#include "CanIf.h"

/* How many times shall the CanNm_Tx be called to send the CAN MSG */
#define CAN_WAIT    9

void CanNm_Tx(void);
uint8_t Can_SetSignal(uint8_t Signal, uint8_t SignalValue);

#endif /* _CAN_NTW_ */