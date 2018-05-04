#ifndef _CAN_IF_
#define _CAN_IF_

#include "CanIf_Cfg.h"
#include "CanIf_Types.h"

typedef struct
{
  uint32_t CanId;
  MCan_IdType CanIdType;
  MCan_DlcType CanDlc;
  uint8_t* CanSdu;
}CanIf_PduType;

typedef struct
{
   uint8_t CanMsgIdNumber;
   MCan_DirType MCanDir;
   CanIf_PduType CanPdu;
}CanIf_MessageConfigType;

typedef struct
{
   uint8_t CanNumberOfMsgs;
   CanIf_MessageConfigType *CanIfMessageConfig;
}CanIf_MsgObjType;

extern CanIf_MsgObjType CanMsgObj;

void CanIf_Init(uint8_t CanChannelId, CanIf_MsgObjType CanIfMsgConfig);
void CanIf_Transmit(uint8_t CanChannelId, uint8_t MsgId);

#endif /* _CAN_IF_ */