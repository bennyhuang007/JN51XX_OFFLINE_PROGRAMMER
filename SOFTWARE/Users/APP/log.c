#include "log.h"
#include "stdio.h"
#include "stdarg.h"
#include "common.h"
#include "fifo.h"
#include "ucos_ii.h"

uint8_t u8DataBuff[512];
FIFO FIFO_LOG = 
{
  .pBuffer = u8DataBuff,
  .Size = sizeof(u8DataBuff),
  .Start = 0,
  .End = 0,
  .Flag_Over = 0,
};

void log(LOG_LEVEL eLogLevel,const char *pStr,...)
{
  char cStrBuff[128];
  uint32_t u32ExistLen = 0;
  uint32_t u32FreeSize = min(FIFO_GetFree(&FIFO_LOG),sizeof(cStrBuff));
  if(eLogLevel <= SystemInfo.Log_Level)
  {
    va_list pArgs;
    va_start(pArgs,pStr);
    u32ExistLen = snprintf(cStrBuff,u32FreeSize,"[%08d][%s]",OSTime,LOG_LEVEL_MARK_STR[eLogLevel]);
    u32ExistLen += vsnprintf(cStrBuff + u32ExistLen,u32FreeSize - u32ExistLen,pStr,pArgs);
    u32ExistLen += snprintf(cStrBuff + u32ExistLen,u32FreeSize - u32ExistLen,"\r\n");
    FIFO_WriteBuffer(&FIFO_LOG,(uint8_t*)cStrBuff,u32ExistLen);
    va_end(pArgs);
  }
}

uint32_t log_getdata(uint8_t *pData,uint32_t iLenght)
{
  return FIFO_ReadBuffer(&FIFO_LOG,pData,iLenght);
}