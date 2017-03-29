#include "app_taskchipx.h"
#include "includes.h"
#include "app_taskmem.h"
#include "usart_driver.h"
#include "app_taskui.h"
#include "app_adc.h"
#include "bl_statemachine.hpp"

#define RAM_CODE_OFFSET     0x04000400
#define FLASH_CODE_OFFSET     0x00080000

OS_FLAG_GRP *FlagChipxStatus;

void App_TaskChip1(void *p_arg)
{
  HARDWARE_PARAM InitParam = 
  {
    .GPIOx_RST = GPIO_CHIP1_RST_PORT,
    .GPIOx_SIG = GPIO_CHIP1_SIG_PORT,
    .GPIOx_PWR = GPIO_CHIP1_PWR_PORT,
    .GPIO_RST_Pin = GPIO_CHIP1_RST_PIN,
    .GPIO_SIG_Pin = GPIO_CHIP1_SIG_PIN,
    .GPIO_PWR_Pin = GPIO_CHIP1_PWR_PIN,
    .USARTx = USART1,
    .FLAG_CHIPx_START_MASK = FLAG_CHIP1_START_MASK,
    .FLAG_CHIPx_RUNNING_MASK = FLAG_CHIP1_RUNNING_MASK,
    .eUILed = UI_LED_CHIP1,
    .cChipNum = '1',
    .pu32ADCValue = ADC_Value+ADC_CHANNAL_CHIP1_HALF,
  };
  
  BL_STATUSMACHINE SM_Chip1(&InitParam);
  
  SM_Chip1.StatusMachine();
}

void App_TaskChip2(void *p_arg)
{
  HARDWARE_PARAM InitParam = 
  {
    .GPIOx_RST = GPIO_CHIP2_RST_PORT,
    .GPIOx_SIG = GPIO_CHIP2_SIG_PORT,
    .GPIOx_PWR = GPIO_CHIP2_PWR_PORT,
    .GPIO_RST_Pin = GPIO_CHIP2_RST_PIN,
    .GPIO_SIG_Pin = GPIO_CHIP2_SIG_PIN,
    .GPIO_PWR_Pin = GPIO_CHIP2_PWR_PIN,
    .USARTx = USART2,
    .FLAG_CHIPx_START_MASK = FLAG_CHIP2_START_MASK,
    .FLAG_CHIPx_RUNNING_MASK = FLAG_CHIP2_RUNNING_MASK,
    .eUILed = UI_LED_CHIP2,
    .cChipNum = '2',
    .pu32ADCValue = ADC_Value+ADC_CHANNAL_CHIP2_HALF,
  };
  
  BL_STATUSMACHINE SM_Chip2(&InitParam);
  
  SM_Chip2.StatusMachine();
}

void App_TaskChip3(void *p_arg)
{
  HARDWARE_PARAM InitParam = 
  {
    .GPIOx_RST = GPIO_CHIP3_RST_PORT,
    .GPIOx_SIG = GPIO_CHIP3_SIG_PORT,
    .GPIOx_PWR = GPIO_CHIP3_PWR_PORT,
    .GPIO_RST_Pin = GPIO_CHIP3_RST_PIN,
    .GPIO_SIG_Pin = GPIO_CHIP3_SIG_PIN,
    .GPIO_PWR_Pin = GPIO_CHIP3_PWR_PIN,
    .USARTx = USART3,
    .FLAG_CHIPx_START_MASK = FLAG_CHIP3_START_MASK,
    .FLAG_CHIPx_RUNNING_MASK = FLAG_CHIP3_RUNNING_MASK,
    .eUILed = UI_LED_CHIP3,
    .cChipNum = '3',
    .pu32ADCValue = ADC_Value+ADC_CHANNAL_CHIP3_HALF,
  };
  
  BL_STATUSMACHINE SM_Chip3(&InitParam);
  
  SM_Chip3.StatusMachine();
}

void App_TaskChip4(void *p_arg)
{
  HARDWARE_PARAM InitParam = 
  {
    .GPIOx_RST = GPIO_CHIP4_RST_PORT,
    .GPIOx_SIG = GPIO_CHIP4_SIG_PORT,
    .GPIOx_PWR = GPIO_CHIP4_PWR_PORT,
    .GPIO_RST_Pin = GPIO_CHIP4_RST_PIN,
    .GPIO_SIG_Pin = GPIO_CHIP4_SIG_PIN,
    .GPIO_PWR_Pin = GPIO_CHIP4_PWR_PIN,
    .USARTx = USART3,
    .FLAG_CHIPx_START_MASK = FLAG_CHIP4_START_MASK,
    .FLAG_CHIPx_RUNNING_MASK = FLAG_CHIP4_RUNNING_MASK,
    .eUILed = UI_LED_CHIP4,
    .cChipNum = '4',
    .pu32ADCValue = ADC_Value+ADC_CHANNAL_CHIP4_HALF,
  };
  
  BL_STATUSMACHINE SM_Chip4(&InitParam);
  
  SM_Chip4.StatusMachine();
}