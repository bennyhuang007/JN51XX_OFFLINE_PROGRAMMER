#include "usart_driver.h"
#include "includes.h"

void USART2_ISR(void);

#ifdef USART_CFG_USART2_ENABLE

uint8_t BUFF_USART2_TX[USART_CFG_USART2_FIFO];
uint8_t BUFF_USART2_RX[USART_CFG_USART2_FIFO];

FIFO FIFO_USART2_TX = {BUFF_USART2_TX,USART_CFG_USART2_FIFO,0,0,0};
FIFO FIFO_USART2_RX = {BUFF_USART2_RX,USART_CFG_USART2_FIFO,0,0,0};
#endif

void USARTx_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure; 
  //-----------------------------------------------------
#if (USART_CFG_USART2_ENABLE)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = USART_CFG_USART2_BAUD;           // 波特率
  USART_InitStructure.USART_WordLength = USART_CFG_USART2_DATABIT;	// 数据位
  USART_InitStructure.USART_StopBits = USART_CFG_USART2_STOPBIT;        // 停止位
  USART_InitStructure.USART_Parity = USART_CFG_USART2_PARITY;		// 校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART2,&USART_InitStructure);
  
#if USART_CFG_USART2_MODE == USART_MODE_QUERY
  asm("nop");
#elif USART_CFG_USART2_MODE == USART_MODE_DMA
  asm("nop");
#elif USART_CFG_USART2_MODE == USART_MODE_INT
  USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
  USART_ITConfig(USART2,USART_IT_TXE|USART_IT_TC,DISABLE);

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  BSP_IntVectSet(BSP_INT_ID_USART2,USART2_ISR);
  NVIC_Init(&NVIC_InitStructure);
#else
#error "USART_CFG_USART2_MODE is wrong defined!"
#endif
  USART_Cmd(USART2,ENABLE);
#endif
}

#ifdef USART_CFG_USART2_ENABLE
void USART2_SendByte(uint8_t Data)
{
  USART_SendData(USART2,Data);
  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
  while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);
  USART_ClearFlag(USART2,USART_FLAG_TC);
}

void USART2_SendBuff(uint8_t *pBuff,uint8_t iLength)
{
  for(uint8_t i = 0;i < iLength;i++)
  {
    USART_SendData(USART2,*(pBuff+i));
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
    while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);
    USART_ClearFlag(USART2,USART_FLAG_TC);
  }
}

void USART2_RxBuffReset(void)
{
  FIFO_Reset(&FIFO_USART2_RX);
}

ERR_CODE USART2_ReadByte(uint8_t *pData)
{
  return FIFO_ReadByte(&FIFO_USART2_RX,pData);
}

uint32_t USART2_ReadBuff(uint8_t *pData,uint32_t iLenght)
{
  return FIFO_ReadBuffer(&FIFO_USART2_RX,pData,iLenght);
}

void USART2_ISR(void)
{
  uint8_t Temp;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    FIFO_WriteByte(&FIFO_USART2_RX,USART_ReceiveData(USART2));
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  }
  // 发送中断
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {
    USART_ClearITPendingBit(USART2,USART_IT_TXE);
    FIFO_ReadByte(&FIFO_USART2_TX,&Temp);
    USART_SendData(USART2,Temp);
  }
  // 发送完成
  if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
    USART_ClearITPendingBit(USART2,USART_IT_TC);
}
#endif
