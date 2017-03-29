#ifndef _APP_TASKCHIPX_H_
#define _APP_TASKCHIPX_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "ucos_ii.h"
  
#define TIME_OUT_MS     1000
  
  // define the bits in "FlagChipxStatus"
#define FLAG_CHIP1_START_MASK           0x01
#define FLAG_CHIP1_RUNNING_MASK         0x02
#define FLAG_CHIP2_START_MASK           0x04
#define FLAG_CHIP2_RUNNING_MASK         0x08
#define FLAG_CHIP3_START_MASK           0x10
#define FLAG_CHIP3_RUNNING_MASK         0x20
#define FLAG_CHIP4_START_MASK           0x40
#define FLAG_CHIP4_RUNNING_MASK         0x80
  
  extern OS_FLAG_GRP *FlagChipxStatus;
  
  extern void App_TaskChip1(void *p_arg);
  extern void App_TaskChip2(void *p_arg);
  extern void App_TaskChip3(void *p_arg);
  extern void App_TaskChip4(void *p_arg);
  
#ifdef __cplusplus
}
#endif

#endif