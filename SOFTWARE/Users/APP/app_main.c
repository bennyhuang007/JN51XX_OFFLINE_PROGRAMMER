/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
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
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the 
*                                 IAR STM32F103ZE-SK Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>

#include "bl_statemachine.hpp"
#include "common.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "flash_if.h"

#include "app_taskui.h"
#include "app_taskchipx.h"
#include "app_taskmem.h"
#include "app_adc.h"
#include "ram_code.h"
#include "spi_flash.h"
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/* ----------------- APPLICATION GLOBALS ------------------ */
/* -------- uC/OS-II APPLICATION TASK STACKS -------------- */
#pragma data_alignment = 8
static OS_STK App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
#pragma data_alignment = 8
static OS_STK App_TaskUSBStk[APP_CFG_TASK_USB_STK_SIZE];
#pragma data_alignment = 8
static OS_STK App_TaskMemCtrlStk[APP_CFG_TASK_MEMCTRL_STK_SIZE];
#pragma data_alignment = 8
static OS_STK App_TaskChip1Stk[APP_CFG_TASK_CHIP1_STK_SIZE];
#pragma data_alignment = 8
static OS_STK App_TaskChip2Stk[APP_CFG_TASK_CHIP2_STK_SIZE];
#pragma data_alignment = 8
static OS_STK App_TaskChip3Stk[APP_CFG_TASK_CHIP3_STK_SIZE];
#pragma data_alignment = 8
static OS_STK App_TaskChip4Stk[APP_CFG_TASK_CHIP4_STK_SIZE];
#pragma data_alignment = 8
static OS_STK App_TaskUIStk[APP_CFG_TASK_UI_STK_SIZE];
#pragma data_alignment = 8
static OS_STK App_TaskAudioStk[APP_CFG_TASK_AUDIO_STK_SIZE];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void App_TaskStart(void *p_arg);
static void App_TaskUSB(void *p_arg);
static void App_TaskAudio(void *p_arg);
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main(void)
{
#if (OS_TASK_NAME_SIZE > 5)    
  CPU_INT08U  err;
#endif
  
  memcpy(&SystemInfo,&SystemInfoDef,sizeof(SYSTEM_INFO));       //Init SystemInfo
  
  BSP_IntDisAll();                                            /* Disable all interrupts until we are ready to accept them */
  
  OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */
  
  OSTaskCreateExt((void (*)(void *)) App_TaskStart,           /* Create the start task                                    */
                  (void           *) 0,
                  (OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_TASK_START_PRIO,
                  (INT16U          ) APP_CFG_TASK_START_PRIO,
                  (OS_STK         *)&App_TaskStartStk[0],
                  (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  
#if (OS_TASK_NAME_SIZE > 5)
  OSTaskNameSet(APP_CFG_TASK_START_PRIO, (INT8U*)"Start", &err);
#endif
  
  // 多个线程使用到Flash组件,放在外面
  FLASH_Init();
  
  OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}

/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
  CPU_INT08U  err;
  (void)p_arg;
  
  BSP_Init();                   // Initialize BSP functions
  ADC_Config();
  OS_CPU_SysTickInit();         // Initialize the SysTick.
  
#if (OS_TASK_STAT_EN > 0)
  OSStatInit();                 // Determine CPU capacity
#endif
  
  QMemMsg = OSQCreate(ArrayMemMsg,MSG_Q_MAX);
  QMacMsg = OSQCreate(ArrayMacMsg,MSG_Q_MAX);
  FlagSelfCheck = OSFlagCreate(0x00,&err);
  FlagChipxStatus = OSFlagCreate(0x00,&err);
  
  log(LOG_LEVEL_ERROR,"System Reboot...");
  // UI
  OSTaskCreateExt((void (*)(void *)) App_TaskUI,
                  (void           *) 0,
                  (OS_STK         *)&App_TaskUIStk[APP_CFG_TASK_UI_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_UI_PRIO,
                  (INT16U          ) APP_CFG_UI_PRIO,
                  (OS_STK         *)&App_TaskUIStk[0],
                  (INT32U          ) APP_CFG_TASK_UI_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  
#if (OS_TASK_NAME_SIZE > 5)
  OSTaskNameSet(APP_CFG_UI_PRIO, (INT8U*)"UI", &err);
#endif
  // USB
  if(GPIO_USB_PWRCHK_VAL())
  {
    UI_LED_Switch(UI_LED_USB,UI_LED_STATUS_ON,UI_LED_STATUS_OFF);       // UI - USB Mode
    
    OSTaskCreateExt((void (*)(void *)) App_TaskUSB,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskUSBStk[APP_CFG_TASK_USB_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_USB_PRIO,
                    (INT16U          ) APP_CFG_TASK_USB_PRIO,
                    (OS_STK         *)&App_TaskUSBStk[0],
                    (INT32U          ) APP_CFG_TASK_USB_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  }else
  {
    UI_LED_Switch(UI_LED_ALL,UI_LED_STATUS_TWINK,UI_LED_STATUS_TWINK);  // UI - Self Check Mode
    
    OSTaskCreateExt((void (*)(void *)) App_TaskMemCtrl,
                    (void           *) 0,
                    (OS_STK         *)&App_TaskMemCtrlStk[APP_CFG_TASK_MEMCTRL_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_MEMCTRL_PRIO,
                    (INT16U          ) APP_CFG_TASK_MEMCTRL_PRIO,
                    (OS_STK         *)&App_TaskMemCtrlStk[0],
                    (INT32U          ) APP_CFG_TASK_MEMCTRL_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    
    OSFlagPend(FlagSelfCheck,SC_FILESYSTEM_MASK|SC_CONFIGFILE_MASK|SC_TARGETFILE_MASK,\
      OS_FLAG_WAIT_SET_ALL,0,&err);                             // Check FileSysterm
    
    while((VOL_VCC_MV <= 3000)||(VOL_VCC_MV >= 3600))         // Check Power
      OSTimeDly(100);
    
    UI_LED_Switch(UI_LED_ALL,UI_LED_STATUS_OFF,UI_LED_STATUS_OFF);      // UI - Self Check OK
    UI_LED_Switch(UI_LED_USB,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
    
    RAM_CODE_CRC32 = CalculateBlockCRC32(0x0,RAM_CODE,sizeof(RAM_CODE));
    
    if(SystemInfo.ChannelMask & 0x01)
    {
      OSTaskCreateExt((void (*)(void *)) App_TaskChip1,
                      (void           *) 0,
                      (OS_STK         *)&App_TaskChip1Stk[APP_CFG_TASK_CHIP1_STK_SIZE - 1],
                      (INT8U           ) APP_CFG_CHIP1_PRIO,
                      (INT16U          ) APP_CFG_CHIP1_PRIO,
                      (OS_STK         *)&App_TaskChip1Stk[0],
                      (INT32U          ) APP_CFG_TASK_CHIP1_STK_SIZE,
                      (void           *) 0,
                      (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
      UI_LED_Switch(UI_LED_CHIP1,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
    }
    
    if(SystemInfo.ChannelMask & 0x02)
    {
      OSTaskCreateExt((void (*)(void *)) App_TaskChip2,
                      (void           *) 0,
                      (OS_STK         *)&App_TaskChip2Stk[APP_CFG_TASK_CHIP2_STK_SIZE - 1],
                      (INT8U           ) APP_CFG_CHIP2_PRIO,
                      (INT16U          ) APP_CFG_CHIP2_PRIO,
                      (OS_STK         *)&App_TaskChip2Stk[0],
                      (INT32U          ) APP_CFG_TASK_CHIP2_STK_SIZE,
                      (void           *) 0,
                      (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
      UI_LED_Switch(UI_LED_CHIP2,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
    }
    
    if(SystemInfo.ChannelMask & 0x04)
    {
      OSTaskCreateExt((void (*)(void *)) App_TaskChip3,
                      (void           *) 0,
                      (OS_STK         *)&App_TaskChip3Stk[APP_CFG_TASK_CHIP3_STK_SIZE - 1],
                      (INT8U           ) APP_CFG_CHIP3_PRIO,
                      (INT16U          ) APP_CFG_CHIP3_PRIO,
                      (OS_STK         *)&App_TaskChip3Stk[0],
                      (INT32U          ) APP_CFG_TASK_CHIP3_STK_SIZE,
                      (void           *) 0,
                      (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
      UI_LED_Switch(UI_LED_CHIP3,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
    }
    
    if(SystemInfo.ChannelMask & 0x08)
    {
      OSTaskCreateExt((void (*)(void *)) App_TaskChip4,
                      (void           *) 0,
                      (OS_STK         *)&App_TaskChip4Stk[APP_CFG_TASK_CHIP4_STK_SIZE - 1],
                      (INT8U           ) APP_CFG_CHIP4_PRIO,
                      (INT16U          ) APP_CFG_CHIP4_PRIO,
                      (OS_STK         *)&App_TaskChip4Stk[0],
                      (INT32U          ) APP_CFG_TASK_CHIP4_STK_SIZE,
                      (void           *) 0,
                      (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
      UI_LED_Switch(UI_LED_CHIP4,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
    }
  }
  
  OSTaskCreateExt((void (*)(void *)) App_TaskAudio,
                  (void           *) 0,
                  (OS_STK         *)&App_TaskAudioStk[APP_CFG_TASK_AUDIO_STK_SIZE - 1],
                  (INT8U           ) APP_CFG_AUDIO_PRIO,
                  (INT16U          ) APP_CFG_AUDIO_PRIO,
                  (OS_STK         *)&App_TaskAudioStk[0],
                  (INT32U          ) APP_CFG_TASK_AUDIO_STK_SIZE,
                  (void           *) 0,
                  (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
  
  while(1)
    OSTimeDlyHMSM(1,0,0,0);
}

//------------------------------------------------------------------------------
static void App_TaskUSB(void *p_arg)
{
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  
  while(1)
    OSTimeDlyHMSM(1,0,0,0);
}

static void App_TaskAudio(void *p_arg)
{
  SPI_FLASH_Config();
  while(1)
    OSTimeDlyHMSM(1,0,0,0);
}
