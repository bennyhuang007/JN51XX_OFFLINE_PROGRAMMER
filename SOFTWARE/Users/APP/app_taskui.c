#include "app_taskui.h"
#include "ucos_ii.h"
#include "common.h"
#include "includes.h"
#include "app_taskchipx.h"

#define Cycle   9      // 2^9=512

UI_LED_PARAM UI_LED_Param[5];

void App_TaskUI(void *p_arg)
{
  uint8_t err;
  uint8_t u8Scan = 0x01;
  uint32_t u32Tick = 0;
  uint32_t u32KeyCount = 0;
  
  GPIO_LED_R_HIGH();
  GPIO_LED_G_HIGH();
  GPIO_LED_COM_LOW(GPIO_LED_COM_PIN_MASK);
  
  while(1)
  {
    //--------------------------------------------------------------------------
    // Key
    if(GPIO_KEY_START_VAL())
    {
      u32KeyCount++;
      if(u32KeyCount == 30)
      {
        if((SystemInfo.ChannelMask&0x01)&&OSFlagAccept(FlagChipxStatus,FLAG_CHIP1_RUNNING_MASK,OS_FLAG_WAIT_CLR_ANY,&err)&&(err == OS_NO_ERR))
          OSFlagPost(FlagChipxStatus,FLAG_CHIP1_START_MASK,OS_FLAG_SET,&err);
        if((SystemInfo.ChannelMask&0x02)&&OSFlagAccept(FlagChipxStatus,FLAG_CHIP2_RUNNING_MASK,OS_FLAG_WAIT_CLR_ANY,&err)&&(err == OS_NO_ERR))
          OSFlagPost(FlagChipxStatus,FLAG_CHIP2_START_MASK,OS_FLAG_SET,&err);
        if((SystemInfo.ChannelMask&0x04)&&OSFlagAccept(FlagChipxStatus,FLAG_CHIP3_RUNNING_MASK,OS_FLAG_WAIT_CLR_ANY,&err)&&(err == OS_NO_ERR))
          OSFlagPost(FlagChipxStatus,FLAG_CHIP3_START_MASK,OS_FLAG_SET,&err);
        if((SystemInfo.ChannelMask&0x08)&&OSFlagAccept(FlagChipxStatus,FLAG_CHIP4_RUNNING_MASK,OS_FLAG_WAIT_CLR_ANY,&err)&&(err == OS_NO_ERR))
          OSFlagPost(FlagChipxStatus,FLAG_CHIP4_START_MASK,OS_FLAG_SET,&err);
      }
    }else
      u32KeyCount = 0;
    //--------------------------------------------------------------------------
    // LED
    GPIO_LED_COM_LOW(GPIO_LED_COM_PIN_MASK);
    if(u8Scan&UI_LED_CHIP1)
    {
      if(UI_LED_Param[0].eCurRStatus == UI_LED_STATUS_OFF)
        GPIO_LED_R_HIGH();
      else if(UI_LED_Param[0].eCurRStatus == UI_LED_STATUS_ON)
        GPIO_LED_R_LOW();
      else if(UI_LED_Param[0].eCurRStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_R_LOW();
        else
          GPIO_LED_R_HIGH();
      }
      
      if(UI_LED_Param[0].eCurGStatus == UI_LED_STATUS_OFF)
        GPIO_LED_G_HIGH();
      else if(UI_LED_Param[0].eCurGStatus == UI_LED_STATUS_ON)
        GPIO_LED_G_LOW();
      else if(UI_LED_Param[0].eCurGStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_G_LOW();
        else
          GPIO_LED_G_HIGH();
      }
      GPIO_LED_COM_HIGH(GPIO_LED_CHIP1_PIN);
    }else if(u8Scan&UI_LED_CHIP2)
    {
      if(UI_LED_Param[1].eCurRStatus == UI_LED_STATUS_OFF)
        GPIO_LED_R_HIGH();
      else if(UI_LED_Param[1].eCurRStatus == UI_LED_STATUS_ON)
        GPIO_LED_R_LOW();
      else if(UI_LED_Param[1].eCurRStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_R_LOW();
        else
          GPIO_LED_R_HIGH();
      }
      
      if(UI_LED_Param[1].eCurGStatus == UI_LED_STATUS_OFF)
        GPIO_LED_G_HIGH();
      else if(UI_LED_Param[1].eCurGStatus == UI_LED_STATUS_ON)
        GPIO_LED_G_LOW();
      else if(UI_LED_Param[1].eCurGStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_G_LOW();
        else
          GPIO_LED_G_HIGH();
      }
      GPIO_LED_COM_HIGH(GPIO_LED_CHIP2_PIN);
    }else if(u8Scan&UI_LED_CHIP3)
    {
      if(UI_LED_Param[2].eCurRStatus == UI_LED_STATUS_OFF)
        GPIO_LED_R_HIGH();
      else if(UI_LED_Param[2].eCurRStatus == UI_LED_STATUS_ON)
        GPIO_LED_R_LOW();
      else if(UI_LED_Param[2].eCurRStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_R_LOW();
        else
          GPIO_LED_R_HIGH();
      }
      
      if(UI_LED_Param[2].eCurGStatus == UI_LED_STATUS_OFF)
        GPIO_LED_G_HIGH();
      else if(UI_LED_Param[2].eCurGStatus == UI_LED_STATUS_ON)
        GPIO_LED_G_LOW();
      else if(UI_LED_Param[2].eCurGStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_G_LOW();
        else
          GPIO_LED_G_HIGH();
      }
      GPIO_LED_COM_HIGH(GPIO_LED_CHIP3_PIN);
    }else if(u8Scan&UI_LED_CHIP4)
    {
      if(UI_LED_Param[3].eCurRStatus == UI_LED_STATUS_OFF)
        GPIO_LED_R_HIGH();
      else if(UI_LED_Param[3].eCurRStatus == UI_LED_STATUS_ON)
        GPIO_LED_R_LOW();
      else if(UI_LED_Param[3].eCurRStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_R_LOW();
        else
          GPIO_LED_R_HIGH();
      }
      
      if(UI_LED_Param[3].eCurGStatus == UI_LED_STATUS_OFF)
        GPIO_LED_G_HIGH();
      else if(UI_LED_Param[3].eCurGStatus == UI_LED_STATUS_ON)
        GPIO_LED_G_LOW();
      else if(UI_LED_Param[3].eCurGStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_G_LOW();
        else
          GPIO_LED_G_HIGH();
      }
      GPIO_LED_COM_HIGH(GPIO_LED_CHIP4_PIN);
    }else if(u8Scan&UI_LED_USB)
    {
      if(UI_LED_Param[4].eCurRStatus == UI_LED_STATUS_OFF)
        GPIO_LED_R_HIGH();
      else if(UI_LED_Param[4].eCurRStatus == UI_LED_STATUS_ON)
        GPIO_LED_R_LOW();
      else if(UI_LED_Param[4].eCurRStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_R_LOW();
        else
          GPIO_LED_R_HIGH();
      }
      
      if(UI_LED_Param[4].eCurGStatus == UI_LED_STATUS_OFF)
        GPIO_LED_G_HIGH();
      else if(UI_LED_Param[4].eCurGStatus == UI_LED_STATUS_ON)
        GPIO_LED_G_LOW();
      else if(UI_LED_Param[4].eCurGStatus == UI_LED_STATUS_TWINK)
      {
        if((u32Tick >> Cycle)&0x1)
          GPIO_LED_G_LOW();
        else
          GPIO_LED_G_HIGH();
      }
      GPIO_LED_COM_HIGH(GPIO_LED_USB_PIN);
    }
    
    u32Tick++;
    u8Scan = u8Scan << 1;
    if(u8Scan & (1<<LED_NUM))   
      u8Scan = 0x01;
    //--------------------------------------------------------------------------
    OSTimeDly(1);
  }
}

void UI_LED_Switch(uint8_t eUILed,UI_LED_STATUS eRStatus,UI_LED_STATUS eGStatus)
{
  if(eUILed & UI_LED_CHIP1)
  {
    UI_LED_Param[0].eCurGStatus = eGStatus;
    UI_LED_Param[0].eCurRStatus = eRStatus;
  }
  if(eUILed & UI_LED_CHIP2)
  {
    UI_LED_Param[1].eCurGStatus = eGStatus;
    UI_LED_Param[1].eCurRStatus = eRStatus;
  }
  if(eUILed & UI_LED_CHIP3)
  {
    UI_LED_Param[2].eCurGStatus = eGStatus;
    UI_LED_Param[2].eCurRStatus = eRStatus;
  }
  if(eUILed & UI_LED_CHIP4)
  {
    UI_LED_Param[3].eCurGStatus = eGStatus;
    UI_LED_Param[3].eCurRStatus = eRStatus;
  }
  if(eUILed & UI_LED_USB)
  {
    UI_LED_Param[4].eCurGStatus = eGStatus;
    UI_LED_Param[4].eCurRStatus = eRStatus;
  }
}