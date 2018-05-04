#ifndef _CANIF_TYPES_
#define _CANIF_TYPES_

#include "board.h"

typedef enum
{
  CAN_TX = 0,
  CAN_RX = 1,
}MCan_DirType;

typedef enum
{
  CAN_MSGID_0 = 0,
  CAN_MSGID_1 = 1,
  CAN_MSGID_2 = 2,
  CAN_MSGID_3 = 3
}MCan_MsgID;

typedef enum
{
  ControllerId_0 = 0,
  ControllerId_1 = 1,
}MCan_LogCanInt;

#endif /* _CANIF_TYPES_ */