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
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM32-SK Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_PRIO            3

#define  APP_CFG_TASK_USB_PRIO              5
#define  APP_CFG_TASK_MEMCTRL_PRIO          7
#define  APP_CFG_CHIP1_PRIO                 11
#define  APP_CFG_CHIP2_PRIO                 12
#define  APP_CFG_CHIP3_PRIO                 13
#define  APP_CFG_CHIP4_PRIO                 14
#define  APP_CFG_UI_PRIO                    6
#define  APP_CFG_AUDIO_PRIO                 20

#define  OS_TASK_TMR_PRIO                   (OS_LOWEST_PRIO - 2)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE        128

#define  APP_CFG_TASK_USB_STK_SIZE          128
#define  APP_CFG_TASK_MEMCTRL_STK_SIZE      4096
#define  APP_CFG_TASK_CHIP1_STK_SIZE        512
#define  APP_CFG_TASK_CHIP2_STK_SIZE        512
#define  APP_CFG_TASK_CHIP3_STK_SIZE        512
#define  APP_CFG_TASK_CHIP4_STK_SIZE        512
#define  APP_CFG_TASK_UI_STK_SIZE           128
#define  APP_CFG_TASK_AUDIO_STK_SIZE        256
/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                    0
#define  TRACE_LEVEL_INFO                   1
#define  TRACE_LEVEL_DBG                    2

#define  APP_CFG_TRACE_LEVEL                TRACE_LEVEL_INFO
#define  APP_CFG_TRACE                      pritnf           

#define  BSP_CFG_TRACE_LEVEL                TRACE_LEVEL_INFO
#define  BSP_CFG_TRACE                      printf               

#define  APP_TRACE_INFO(x)                  ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                   ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)0)

#define  BSP_TRACE_INFO(x)                  ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO) ? (void)(BSP_CFG_TRACE x) : (void)0)
#define  BSP_TRACE_DBG(x)                   ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DBG)  ? (void)(BSP_CFG_TRACE x) : (void)0)

#endif
