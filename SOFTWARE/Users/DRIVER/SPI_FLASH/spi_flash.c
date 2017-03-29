#include "spi_flash.h"
#include "stm32f10x_spi.h"

uint8_t AudioBuff[256];
void SPI_FLASH_Config(void)
{
  // GPIO Init
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure = {
    .GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5,
    .GPIO_Mode = GPIO_Mode_AF_PP,
    .GPIO_Speed = GPIO_Speed_50MHz,
  };
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // SPI Init
  SPI_InitTypeDef SPI_InitStructure = {
    .SPI_Direction = SPI_Direction_2Lines_FullDuplex,
    .SPI_Mode   = SPI_Mode_Master,
    .SPI_DataSize  = SPI_DataSize_8b,
    .SPI_CPOL   = SPI_CPOL_Low,
    .SPI_CPHA   = SPI_CPHA_1Edge,
    .SPI_NSS   = SPI_NSS_Soft,
    .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4,
    .SPI_FirstBit    = SPI_FirstBit_MSB,
    .SPI_CRCPolynomial  = 7,
  };
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI2, ENABLE);
  // DMA Init
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);
  DMA_DeInit(DMA2_Channel1);
  DMA_InitTypeDef DMA_InitStructure = {
   .DMA_PeripheralBaseAddr = SPI3_BASE+0x0C,    // SPI3_DR
   .DMA_MemoryBaseAddr = (uint32_t)AudioBuff,
   .DMA_DIR = DMA_DIR_PeripheralSRC,
   .DMA_BufferSize = 256,
   .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
   .DMA_MemoryInc = DMA_MemoryInc_Enable,
   .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
   .DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte,
   .DMA_Mode = DMA_Mode_Normal,
   .DMA_Priority = DMA_Priority_High,
   .DMA_M2M = DMA_M2M_Disable,
  };
  DMA_Init(DMA2_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA2_Channel1, ENABLE);
}

uint8_t SPI_FLASH_TXRXByte(uint16_t TXByte,uint8_t RXByte)
{
  ;
}