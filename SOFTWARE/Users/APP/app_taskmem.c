#include "app_taskmem.h"
#include "includes.h"
#include "ff.h"
#include "read_config_file.h"
#include "common.h"
#include "app_taskui.h"
#include "bl_statemachine.hpp"

void *ArrayMemMsg[MSG_Q_MAX],*ArrayMacMsg[MSG_Q_MAX];
OS_EVENT *QMemMsg,*QMacMsg;

void App_TaskMemCtrl(void *p_arg)
{
  FATFS fs;
  FIL fp_target,fp_mac,fp_log,fp_temp;
  MEM_MSG *pMemMsg;
  FRESULT fres;
  uint8_t err;
  
  uint8_t u8DataBuff[256];
  uint32_t u32ReqLen = 0;
  uint32_t u32ActLen;
  
  uint32_t u32CurLogPoint;      // the writting pointer for log file
  
  if(f_mount(&fs,"",1) == FR_OK)
  {
    //SelfCheck----------------------------
    UI_LED_Switch(UI_LED_CHIP2,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
    OSFlagPost(FlagSelfCheck,SC_FILESYSTEM_MASK,OS_FLAG_SET,&err);
    //-------------------------------------
    // config.ini
    if(f_open(&fp_temp,"config.ini", FA_OPEN_EXISTING | FA_READ) == FR_OK)
    {
      //SelfCheck----------------------------
      UI_LED_Switch(UI_LED_CHIP3,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
      OSFlagPost(FlagSelfCheck,SC_CONFIGFILE_MASK,OS_FLAG_SET,&err);
      //-------------------------------------
      TCHAR *p;
      f_lseek(&fp_temp,0);
      while(1)
      {
        p = f_gets((TCHAR*)u8DataBuff,sizeof(u8DataBuff),&fp_temp);
        if(p != NULL)
        {
          u32ActLen = strlen(p);
          if(u32ActLen >= 2)
          {
            *(p + u32ActLen - 2) = '\0';  // delect the '\r\n'
            config_file_statemachine(p,u32ActLen - 2);
          }
        }else
          break;
        
        if(f_eof(&fp_temp))
          break;
      }
      if(u32CfgMask != CFG_FULL_MASK)
        log(LOG_LEVEL_ERROR,"Some Section in config.ini is missing,[%X]",u32CfgMask);
    }else
    {
      //SelfCheck----------------------------
      UI_LED_Switch(UI_LED_CHIP3,UI_LED_STATUS_TWINK,UI_LED_STATUS_OFF);
      OSFlagPost(FlagSelfCheck,SC_CONFIGFILE_MASK,OS_FLAG_CLR,&err);
      //-------------------------------------
      log(LOG_LEVEL_ERROR,"Can\'t Open \"config.ini\"");
    }
    f_close(&fp_temp);
    // status.ini
    //if(f_open(&fp_temp,"status.ini", FA_OPEN_EXISTING | FA_READ) == FR_OK)
    //  ;
    //f_close(&fp_temp);
    // target.bin
    if(f_open(&fp_target,"target.bin", FA_OPEN_EXISTING | FA_READ) == FR_OK)
    {
      //SelfCheck----------------------------
      UI_LED_Switch(UI_LED_CHIP4,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
      OSFlagPost(FlagSelfCheck,SC_TARGETFILE_MASK,OS_FLAG_SET,&err);
      //-------------------------------------
      uint32_t u32CRC = 0;
      uint32_t u32CurAddr = 4;
      
      SystemInfo.u32TargetSize = f_size(&fp_target);
      f_lseek(&fp_target,u32CurAddr);
      while(u32CurAddr < SystemInfo.u32TargetSize)
      {
        u32ReqLen = min(sizeof(u8DataBuff),SystemInfo.u32TargetSize - u32CurAddr);
        f_read(&fp_target,u8DataBuff,u32ReqLen,&u32ActLen);
        u32CRC = CalculateBlockCRC32(u32CRC,u8DataBuff,u32ActLen);
        u32CurAddr += u32ReqLen;
      }
      SystemInfo.u32TargetCRC = u32CRC;
    }else
    {
      //SelfCheck----------------------------
      OSFlagPost(FlagSelfCheck,SC_TARGETFILE_MASK,OS_FLAG_CLR,&err);
      UI_LED_Switch(UI_LED_CHIP4,UI_LED_STATUS_TWINK,UI_LED_STATUS_OFF);
      //-------------------------------------
      log(LOG_LEVEL_ERROR,"Can\'t Open \"target.bin\"");
    }
    // log.txt
    if(f_open(&fp_log,"log.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK)
    {
      TCHAR *p,*pEnd;
      uint32_t u32Temp;
      f_lseek(&fp_log,0);
      p = f_gets((char*)u8DataBuff,sizeof(u8DataBuff),&fp_log);
      u32Temp = strtol((const char *)p,&pEnd,16);
      if((strlen((const char*)p) == 10)&&(*pEnd == '\r'))
        u32CurLogPoint = u32Temp;
      else
      {
        // save log file head len(8byte) + \r\n,if modify,change the LOG_HEAD_SIZE in log.h & change the mark (B) in the file
        u32CurLogPoint = sprintf((char*)u8DataBuff,"%08X\r\n",LOG_HEAD_SIZE);   // (A)
        f_lseek(&fp_log,0);
        f_write(&fp_log,u8DataBuff,u32CurLogPoint,&u32ActLen);
        f_truncate(&fp_log);
        f_sync(&fp_log);
      }
    }
  }else
  {
    //SelfCheck----------------------------
    OSFlagPost(FlagSelfCheck,SC_FILESYSTEM_MASK|SC_CONFIGFILE_MASK|SC_TARGETFILE_MASK,OS_FLAG_CLR,&err);
    UI_LED_Switch(UI_LED_CHIP2|UI_LED_CHIP3|UI_LED_CHIP4,UI_LED_STATUS_TWINK,UI_LED_STATUS_OFF);
    //-------------------------------------
  }
  // whether if the filesystem is all OK...
  OSFlagPend(FlagSelfCheck,SC_FILESYSTEM_MASK|SC_CONFIGFILE_MASK|SC_TARGETFILE_MASK,\
    OS_FLAG_WAIT_SET_ALL,0,&err);
  
  while(1)
  {
    pMemMsg = (MEM_MSG*)OSQPend(QMemMsg,10,&err);
    if(err == OS_NO_ERR)
    {
      f_lseek(&fp_target,pMemMsg->u32Addr);
      f_read(&fp_target,pMemMsg->pMemData,pMemMsg->u32ReqLen,&u32ActLen);
      if(u32ActLen >= pMemMsg->u32ReqLen)
        pMemMsg->DataExist = 1;
    }
    //------------------------------------------
    u32ReqLen = log_getdata(u8DataBuff,sizeof(u8DataBuff));
    if(u32ReqLen)
    {
      if(u32CurLogPoint + u32ReqLen > SystemInfo.Log_FileSizeMax)
      {
        f_lseek(&fp_log,u32CurLogPoint);
        f_truncate(&fp_log);
        u32CurLogPoint = LOG_HEAD_SIZE;
        f_lseek(&fp_log,LOG_HEAD_SIZE);
      }else
        f_lseek(&fp_log,u32CurLogPoint);
      f_write(&fp_log,u8DataBuff,u32ReqLen,&u32ActLen);
      u32CurLogPoint += u32ActLen;
      
      f_lseek(&fp_log,0);
      // save log file head len(8byte) + \r\n,if modify,change the LOG_HEAD_SIZE in log.h & change the mark (A) in the file
      u32ReqLen = sprintf((char*)u8DataBuff,"%08X\r\n",u32CurLogPoint); // (B)
      f_write(&fp_log,u8DataBuff,u32ReqLen,&u32ActLen);
      f_sync(&fp_log);
    }
    OSTimeDly(1);
  }
}