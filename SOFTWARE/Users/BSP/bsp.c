/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2008; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the 
*                                 IAR STM32F103ZE-SK Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/
  
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include <bsp.h>

#include "usart_driver.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_RCC_TO_VAL                  0x00000FFF             /* Max Timeout for RCC register                             */

/*
*********************************************************************************************************
*                                           EXTERNAL FUNCTIONS
*********************************************************************************************************
*/

extern void SCP1000_IntrHook (void);
static void BSP_GPIO_StartupConfig(void);

/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void BSP_RCC_Init(void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
**********************************************************************************************************
**********************************************************************************************************
**                                         GLOBAL FUNCTIONS
**********************************************************************************************************
**************************************************************************************q********************
*/

/*
*********************************************************************************************************
*                                            BSP_Init()
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);       // <---
  
  BSP_IntInit();        /* Interrupt Vector Init */ 
  BSP_RCC_Init();       /* Initialize the Reset and Control (RCC) module */
  //
  USARTx_Config();
  //
  BSP_GPIO_StartupConfig();
}

/*
*********************************************************************************************************
*                                       BSP_CPU_ClkFreq()
*
* Description : This function reads CPU registers to determine the CPU clock frequency of the chip in KHz.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
  static  RCC_ClocksTypeDef  rcc_clocks;
  
  RCC_GetClocksFreq(&rcc_clocks);
  
  return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                         OS CORTEX-M3 FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         OS_CPU_SysTickClkFreq()
*
* Description : Get system tick clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : Clock frequency (of system tick).
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

INT32U  OS_CPU_SysTickClkFreq (void)
{
  INT32U  freq;
  
  
  freq = BSP_CPU_ClkFreq();
  return (freq);
}

/*
*********************************************************************************************************
*                                         BSP_RCC_Init()
*
* Description : Initializes the RCC module. Set the FLASH memmory timing and the system clock dividers
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_RCC_Init (void) 
{    
  CPU_INT32U  rcc_to;                                          /* RCC registers timeout                                    */
  
  
  RCC_DeInit();                                                /*  Reset the RCC clock config to the default reset state   */
  
  RCC_HSEConfig(RCC_HSE_ON);                                   /*  HSE Oscillator ON                                       */        
  RCC_LSEConfig(RCC_LSE_OFF);
  
  rcc_to = BSP_RCC_TO_VAL;
  
  while ((rcc_to > 0) && (RCC_WaitForHSEStartUp() != SUCCESS)) /* Wait until the oscilator is stable                       */
    rcc_to--;
  
  FLASH_SetLatency(FLASH_Latency_2);
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);         /* Fcpu = (PLL_src * PLL_MUL) = (8 Mhz / 1) * (9) = 72Mhz   */ 
  RCC_PLLCmd(ENABLE);
  
  rcc_to = BSP_RCC_TO_VAL;
  
  while ((rcc_to > 0) && (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET))
    rcc_to--;
  
  RCC_HCLKConfig(RCC_SYSCLK_Div1);                             /* Set system clock dividers                                */                            
  RCC_PCLK2Config(RCC_HCLK_Div1);    
  RCC_PCLK1Config(RCC_HCLK_Div2);    
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);
  
  FLASH_SetLatency(FLASH_Latency_2);                           /* Embedded Flash Configuration                             */
  FLASH_HalfCycleAccessCmd(FLASH_HalfCycleAccess_Disable);
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

static void BSP_GPIO_StartupConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure =  {.GPIO_Speed = GPIO_Speed_50MHz};
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
  // Remap IOs
  // Caution:if you use PC14 PC15 for GPIOs,please check if the LSE is OFF!
  // Disable JTAG,set PA15-SPI3_NSS PB3-SPI3_SCK PB4-SPI3_MISO as GPIOs
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  
  // MARK(A)
  //!!!Caution!!!
  //if modify the GPIO Pin blow,please modify the MARK(A) in bsp.h
  
  // RSTs & SIGs
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP1_RST_PIN;
  GPIO_Init(GPIO_CHIP1_RST_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP1_SIG_PIN;
  GPIO_Init(GPIO_CHIP1_SIG_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP2_RST_PIN;
  GPIO_Init(GPIO_CHIP2_RST_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP2_SIG_PIN;
  GPIO_Init(GPIO_CHIP2_SIG_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP3_RST_PIN;
  GPIO_Init(GPIO_CHIP3_RST_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP3_SIG_PIN;
  GPIO_Init(GPIO_CHIP3_SIG_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP4_RST_PIN;
  GPIO_Init(GPIO_CHIP4_RST_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP4_SIG_PIN;
  GPIO_Init(GPIO_CHIP4_SIG_PORT, &GPIO_InitStructure);
  
  GPIO_CHIP1_RST_HIGH();
  GPIO_CHIP1_SIG_HIGH();
  GPIO_CHIP2_RST_HIGH();
  GPIO_CHIP2_SIG_HIGH();
  GPIO_CHIP3_RST_HIGH();
  GPIO_CHIP3_SIG_HIGH();
  GPIO_CHIP4_RST_HIGH();
  GPIO_CHIP4_SIG_HIGH();
  // PWRs
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP1_PWR_PIN;
  GPIO_Init(GPIO_CHIP1_PWR_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP2_PWR_PIN;
  GPIO_Init(GPIO_CHIP2_PWR_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP3_PWR_PIN;
  GPIO_Init(GPIO_CHIP3_PWR_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP4_PWR_PIN;
  GPIO_Init(GPIO_CHIP4_PWR_PORT, &GPIO_InitStructure);
  GPIO_CHIP1_PWR_LOW();
  GPIO_CHIP2_PWR_LOW();
  GPIO_CHIP3_PWR_LOW();
  GPIO_CHIP4_PWR_LOW();
  // ADC
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP1_ADC_PIN;
  GPIO_Init(GPIO_CHIP1_ADC_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP2_ADC_PIN;
  GPIO_Init(GPIO_CHIP2_ADC_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP3_ADC_PIN;
  GPIO_Init(GPIO_CHIP3_ADC_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_CHIP4_ADC_PIN;
  GPIO_Init(GPIO_CHIP4_ADC_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_ADC_VCC_PIN;
  GPIO_Init(GPIO_ADC_VCC_PORT, &GPIO_InitStructure);
  // LEDs
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_LED_COM_PIN_MASK;
  GPIO_Init(GPIO_LED_COM_PORT, &GPIO_InitStructure);
  GPIO_SetBits(GPIO_LED_COM_PORT,GPIO_LED_COM_PIN_MASK);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_LED_R_PIN;
  GPIO_Init(GPIO_LED_R_PORT, &GPIO_InitStructure);
  GPIO_LED_R_HIGH();
  
  GPIO_InitStructure.GPIO_Pin = GPIO_LED_G_PIN;
  GPIO_Init(GPIO_LED_G_PORT, &GPIO_InitStructure);
  GPIO_LED_G_HIGH();
  // CLKOUT
  GPIO_InitStructure.GPIO_Pin = GPIO_CLKOUT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIO_CLKOUT_PORT, &GPIO_InitStructure);
  // USB
  GPIO_InitStructure.GPIO_Pin = GPIO_USB_PWRCHK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_USB_PWRCHK_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_USB_EN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_USB_EN_PORT, &GPIO_InitStructure);
  GPIO_USB_EN_HIGH();           // Disconnect
  
  // KEY START
  GPIO_InitStructure.GPIO_Pin = GPIO_USB_PWRCHK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_USB_PWRCHK_PORT, &GPIO_InitStructure);
  
  // SPI_NSS
  GPIO_InitStructure.GPIO_Pin = GPIO_SPI_NSS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_SPI_NSS_PORT, &GPIO_InitStructure);
  GPIO_SPI_NSS_HIGH();
  
  //!!!Caution!!!
}

void BSP_GPIO_ChangeRSTSIGMode(IO_MODE eIOMode)
{}
/*
void BSP_GPIO_ChangeRSTSIGMode(IO_MODE eIOMode)
{
  // MARK(B)
  // !!!Caution!!!
  // if modify the GPIO Pin blow,please modify the MARK(A) in bsp.h
#if GPIO_CHIP1_RST_SHIFT < 8
    GPIO_CHIP1_RST_PORT->CRL &= (0xFFFFFFF3ul << (4*GPIO_CHIP1_RST_SHIFT));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP1_RST_PORT->CRL |= (0x0ul << (4*GPIO_CHIP1_RST_SHIFT));
    else
      GPIO_CHIP1_RST_PORT->CRL |= (0x4ul << (4*GPIO_CHIP1_RST_SHIFT));
#else
    GPIO_CHIP1_RST_PORT->CRH &= (0xFFFFFFF3ul << (4*(GPIO_CHIP1_RST_SHIFT-8)));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP1_RST_PORT->CRL |= (0x0ul << (4*(GPIO_CHIP1_RST_SHIFT-8)));
    else
      GPIO_CHIP1_RST_PORT->CRL |= (0x4ul << (4*(GPIO_CHIP1_RST_SHIFT-8)));
#endif
    
#if GPIO_CHIP2_RST_SHIFT < 8
    GPIO_CHIP2_RST_PORT->CRL &= (0xFFFFFFF3ul << (4*GPIO_CHIP2_RST_SHIFT));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP2_RST_PORT->CRL |= (0x0ul << (4*GPIO_CHIP2_RST_SHIFT));
    else
      GPIO_CHIP2_RST_PORT->CRL |= (0x4ul << (4*GPIO_CHIP2_RST_SHIFT));
#else
    GPIO_CHIP2_RST_PORT->CRH &= (0xFFFFFFF3ul << (4*(GPIO_CHIP2_RST_SHIFT-8)));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP2_RST_PORT->CRL |= (0x0ul << (4*(GPIO_CHIP2_RST_SHIFT-8)));
    else
      GPIO_CHIP2_RST_PORT->CRL |= (0x4ul << (4*(GPIO_CHIP2_RST_SHIFT-8)));
#endif
    
#if GPIO_CHIP3_RST_SHIFT < 8
    GPIO_CHIP3_RST_PORT->CRL &= (0xFFFFFFF3ul << (4*GPIO_CHIP3_RST_SHIFT));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP3_RST_PORT->CRL |= (0x0ul << (4*GPIO_CHIP3_RST_SHIFT));
    else
      GPIO_CHIP3_RST_PORT->CRL |= (0x4ul << (4*GPIO_CHIP3_RST_SHIFT));
#else
    GPIO_CHIP3_RST_PORT->CRH &= (0xFFFFFFF3ul << (4*(GPIO_CHIP3_RST_SHIFT-8)));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP3_RST_PORT->CRL |= (0x0ul << (4*(GPIO_CHIP3_RST_SHIFT-8)));
    else
      GPIO_CHIP3_RST_PORT->CRL |= (0x4ul << (4*(GPIO_CHIP3_RST_SHIFT-8)));
#endif
    
#if GPIO_CHIP4_RST_SHIFT < 8
    GPIO_CHIP4_RST_PORT->CRL &= (0xFFFFFFF3ul << (4*GPIO_CHIP4_RST_SHIFT));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP4_RST_PORT->CRL |= (0x0ul << (4*GPIO_CHIP4_RST_SHIFT));
    else
      GPIO_CHIP4_RST_PORT->CRL |= (0x4ul << (4*GPIO_CHIP4_RST_SHIFT));
#else
    GPIO_CHIP4_RST_PORT->CRH &= (0xFFFFFFF3ul << (4*(GPIO_CHIP4_RST_SHIFT-8)));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP4_RST_PORT->CRL |= (0x0ul << (4*(GPIO_CHIP4_RST_SHIFT-8)));
    else
      GPIO_CHIP4_RST_PORT->CRL |= (0x4ul << (4*(GPIO_CHIP4_RST_SHIFT-8)));
#endif
    
#if GPIO_CHIP1_SIG_SHIFT < 8
    GPIO_CHIP1_SIG_PORT->CRL &= (0xFFFFFFF3ul << (4*GPIO_CHIP1_SIG_SHIFT));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP1_SIG_PORT->CRL |= (0x0ul << (4*GPIO_CHIP1_SIG_SHIFT));
    else
      GPIO_CHIP1_SIG_PORT->CRL |= (0x4ul << (4*GPIO_CHIP1_SIG_SHIFT));
#else
    GPIO_CHIP1_SIG_PORT->CRH &= (0xFFFFFFF3ul << (4*(GPIO_CHIP1_SIG_SHIFT-8)));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP1_SIG_PORT->CRL |= (0x0ul << (4*(GPIO_CHIP1_SIG_SHIFT-8)));
    else
      GPIO_CHIP1_SIG_PORT->CRL |= (0x4ul << (4*(GPIO_CHIP1_SIG_SHIFT-8)));
#endif
    
#if GPIO_CHIP2_SIG_SHIFT < 8
    GPIO_CHIP2_SIG_PORT->CRL &= (0xFFFFFFF3ul << (4*GPIO_CHIP2_SIG_SHIFT));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP2_SIG_PORT->CRL |= (0x0ul << (4*GPIO_CHIP2_SIG_SHIFT));
    else
      GPIO_CHIP2_SIG_PORT->CRL |= (0x4ul << (4*GPIO_CHIP2_SIG_SHIFT));
#else
    GPIO_CHIP2_SIG_PORT->CRH &= (0xFFFFFFF3ul << (4*(GPIO_CHIP2_SIG_SHIFT-8)));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP2_SIG_PORT->CRL |= (0x0ul << (4*(GPIO_CHIP2_SIG_SHIFT-8)));
    else
      GPIO_CHIP2_SIG_PORT->CRL |= (0x4ul << (4*(GPIO_CHIP2_SIG_SHIFT-8)));
#endif
    
#if GPIO_CHIP3_SIG_SHIFT < 8
    GPIO_CHIP3_SIG_PORT->CRL &= (0xFFFFFFF3ul << (4*GPIO_CHIP3_SIG_SHIFT));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP3_SIG_PORT->CRL |= (0x0ul << (4*GPIO_CHIP3_SIG_SHIFT));
    else
      GPIO_CHIP3_SIG_PORT->CRL |= (0x4ul << (4*GPIO_CHIP3_SIG_SHIFT));
#else
    GPIO_CHIP3_SIG_PORT->CRH &= (0xFFFFFFF3ul << (4*(GPIO_CHIP3_SIG_SHIFT-8)));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP3_SIG_PORT->CRL |= (0x0ul << (4*(GPIO_CHIP3_SIG_SHIFT-8)));
    else
      GPIO_CHIP3_SIG_PORT->CRL |= (0x4ul << (4*(GPIO_CHIP3_SIG_SHIFT-8)));
#endif
    
#if GPIO_CHIP4_SIG_SHIFT < 8
    GPIO_CHIP4_SIG_PORT->CRL &= (0xFFFFFFF3ul << (4*GPIO_CHIP4_SIG_SHIFT));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP4_SIG_PORT->CRL |= (0x0ul << (4*GPIO_CHIP4_SIG_SHIFT));
    else
      GPIO_CHIP4_SIG_PORT->CRL |= (0x4ul << (4*GPIO_CHIP4_SIG_SHIFT));
#else
    GPIO_CHIP4_SIG_PORT->CRH &= (0xFFFFFFF3ul << (4*(GPIO_CHIP4_SIG_SHIFT-8)));
    if(eIOMode == IO_MODE_PP)
      GPIO_CHIP4_SIG_PORT->CRL |= (0x0ul << (4*(GPIO_CHIP4_SIG_SHIFT-8)));
    else
      GPIO_CHIP4_SIG_PORT->CRL |= (0x4ul << (4*(GPIO_CHIP4_SIG_SHIFT-8)));
#endif
  // !!!Caution!!!
}*/

void BSP_GPIO_CLKOUT_Config(bool bEnable)
{
  if(bEnable)
    RCC_MCOConfig(RCC_MCO_HSE); // 8MHz
  else
    RCC_MCOConfig(RCC_MCO_NoClock);
}