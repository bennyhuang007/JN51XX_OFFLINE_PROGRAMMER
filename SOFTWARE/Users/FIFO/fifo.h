#ifndef _FIFO_H_
#define _FIFO_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "stdint.h"
#include "ErrCode.h"

typedef struct
{
    uint8_t *pBuffer;
    uint32_t Size;
    uint32_t Start;
    uint32_t End;
    uint8_t Flag_Over;
}FIFO;

extern void FIFO_Config(FIFO *pFIFO,uint8_t *pBuffer,uint32_t iLenght);
extern void FIFO_Reset(FIFO *pFIFO);
extern ERR_CODE FIFO_WriteByte(FIFO *pFIFO,uint8_t Data);
extern ERR_CODE FIFO_WriteBuffer(FIFO *pFIFO,uint8_t *pData,uint32_t iLenght);
extern ERR_CODE FIFO_ReadByte(FIFO *pFIFO,uint8_t *pData);
extern uint32_t FIFO_ReadBuffer(FIFO *pFIFO,uint8_t *pData,uint32_t iLenght);
extern uint32_t FIFO_GetFree(FIFO *pFIFO);
extern uint32_t FIFO_GetExist(FIFO *pFIFO);
extern uint32_t FIFO_GetPacket(FIFO *pFIFO,uint8_t *pData,uint32_t iBufferSize);


#ifdef __cplusplus
}
#endif
  
#endif