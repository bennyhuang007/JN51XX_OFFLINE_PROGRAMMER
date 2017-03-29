#ifndef _USART_DRIVER_H_
#define _USART_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "stdint.h"
#include "FIFO.h"
#include "ucos_ii.h"

#define USART_MODE_QUERY                0
#define USART_MODE_DMA                  1
#define USART_MODE_INT                  2

#define USART_CFG_USART2_ENABLE         1
#define USART_CFG_USART2_BAUD           38400
#define USART_CFG_USART2_DATABIT        USART_WordLength_8b     // USART_WordLength_8b/USART_WordLength_9b
#define USART_CFG_USART2_PARITY         USART_Parity_No         // USART_Parity_No/USART_Parity_Even/USART_Parity_Odd
#define USART_CFG_USART2_STOPBIT        USART_StopBits_1        // USART_StopBits_1/USART_StopBits_0_5/USART_StopBits_2/USART_StopBits_1_5
#define USART_CFG_USART2_MODE           USART_MODE_INT
#define USART_CFG_USART2_FIFO           512

extern FIFO FIFO_USART2_TX;
extern FIFO FIFO_USART2_RX;

extern void USARTx_Config(void);        // Config All USART

extern void USART2_SendByte(uint8_t Data);
extern void USART2_SendBuff(uint8_t *pBuff,uint8_t iLength);
extern void USART2_RxBuffReset(void);
extern ERR_CODE USART2_ReadByte(uint8_t *pData);
extern uint32_t USART2_ReadBuff(uint8_t *pData,uint32_t iLenght);

#ifdef __cplusplus
}
#endif
  
#endif
