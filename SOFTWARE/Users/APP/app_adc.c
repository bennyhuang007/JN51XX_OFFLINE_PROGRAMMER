#include "app_adc.h"
#include "includes.h"
#include "stdint.h"

// ADC_CH10-PC0-CHIP1;ADC_CH11-PC1-CHIP4;ADC_CH12-PC2-CHIP2;ADC_CH13-PC3-CHIP3;ADC_CH14-PC4-Vcc;ADC_CH17-Vrefint
uint32_t ADC_Value[6] = {2048,2048,2048,2048,2048,1489}; //<---

void ADC_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 6;
  ADC_Init(ADC1,&ADC_InitStructure);
  //!!!Caution!!!         MARK(A)
  // if you modify the list below,please modify the MARK(A) in "app_adc.h" at the same time!
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10,1,ADC_SampleTime_239Cycles5);   // CHIP1
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12,2,ADC_SampleTime_239Cycles5);   // CHIP2
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13,3,ADC_SampleTime_239Cycles5);   // CHIP3
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11,4,ADC_SampleTime_239Cycles5);   // CHIP4
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14,5,ADC_SampleTime_239Cycles5);   // Vcc/2
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17,6,ADC_SampleTime_239Cycles5);   // Vrefint
  //!!!Caution!!!
  ADC_Cmd(ADC1,ENABLE);
  
  //复位校准寄存器
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  //启动自动校准
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
  //DMA控制
  ADC_DMACmd(ADC1, ENABLE);
  //开始转换
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}