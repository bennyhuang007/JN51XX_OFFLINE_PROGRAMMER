#ifndef _APP_ADC_H_
#define _APP_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "stdint.h"
  
  //!!!Caution!!!         MARK(A)
  // if you modify the list below,please modify the MARK(A) in "app_adc.c" at the same time!
#define ADC_CHANNAL_CHIP1_HALF          0
#define ADC_CHANNAL_CHIP2_HALF          1
#define ADC_CHANNAL_CHIP3_HALF          2
#define ADC_CHANNAL_CHIP4_HALF          3
#define ADC_CHANNAL_VCC_HALF            4
#define ADC_CHANNAL_VREFIINT            5
  
#define ADC_RESULT_MV(a)                (2400 * a / ADC_Value[ADC_CHANNAL_VREFIINT])
#define VOL_VCC_MV                      (2400 * ADC_Value[ADC_CHANNAL_VCC_HALF]/ADC_Value[ADC_CHANNAL_VREFIINT])
  //!!!Caution!!!
  
  extern uint32_t ADC_Value[6];
  extern void ADC_Config(void);
  
#ifdef __cplusplus
}
#endif

#endif