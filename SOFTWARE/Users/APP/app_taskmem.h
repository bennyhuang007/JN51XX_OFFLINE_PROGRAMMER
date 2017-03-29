#ifndef _APP_TASKMEM_H_
#define _APP_TASKMEM_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "ucos_ii.h"
  
#define MSG_Q_MAX       4       // 消息队列长度 channel0-channel3
  
  extern void *ArrayMemMsg[MSG_Q_MAX],*ArrayMacMsg[MSG_Q_MAX];
  extern OS_EVENT *QMemMsg,*QMacMsg;
  
  extern void App_TaskMemCtrl(void *p_arg);
  
#ifdef __cplusplus
}
#endif

#endif