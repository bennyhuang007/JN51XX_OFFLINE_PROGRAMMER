#ifndef _APP_TASKUI_H_
#define _APP_TASKUI_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "stdint.h"
  
#define LED_NUM         5
#define UI_LED_CHIP1    0x01
#define UI_LED_CHIP2    0x02
#define UI_LED_CHIP3    0x04
#define UI_LED_CHIP4    0x08
#define UI_LED_USB      0x10
#define UI_LED_ALL      0x1F
  
  typedef enum
  {
    UI_LED_STATUS_OFF = 0,
    UI_LED_STATUS_ON,
    UI_LED_STATUS_TWINK,
  }UI_LED_STATUS;
  
  typedef struct
  {
    uint8_t eCurRStatus:4;
    uint8_t eCurGStatus:4;
  }UI_LED_PARAM;
  
  extern void App_TaskUI(void *p_arg);
  extern void UI_LED_Switch(uint8_t eUILed,UI_LED_STATUS eRStatus,UI_LED_STATUS eGStatus);
  
#ifdef __cplusplus
}
#endif

#endif