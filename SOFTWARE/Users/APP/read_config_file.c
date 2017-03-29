#include "read_config_file.h"
#include "common.h"
#include "string.h"
#include "stdlib.h"
#include "bl_statemachine.hpp"
#include "log.h"
#include "bsp.h"

uint32_t u32CfgMask = 0;    // every bit corresponding a section

void callback_FILE_INFO_Version(const char *pStr,uint32_t u32StrLen);
void callback_DEVICE_INFO_Device(const char *pStr,uint32_t u32StrLen);
void callback_DEVICE_INFO_Flash(const char *pStr,uint32_t u32StrLen);
void callback_MAC_INFO_Mode(const char *pStr,uint32_t u32StrLen);
void callback_MAC_INFO_ErrorMode(const char *pStr,uint32_t u32StrLen);
void callback_MAC_INFO_StartAddr(const char *pStr,uint32_t u32StrLen);
void callback_MAC_INFO_FileName(const char *pStr,uint32_t u32StrLen);
void callback_OTP_INFO_OTPMask(const char *pStr,uint32_t u32StrLen);
void callback_OTP_INFO_AES(const char *pStr,uint32_t u32StrLen);
void callback_OTP_INFO_UserData0(const char *pStr,uint32_t u32StrLen);
void callback_OTP_INFO_UserData1(const char *pStr,uint32_t u32StrLen);
void callback_OTP_INFO_UserData2(const char *pStr,uint32_t u32StrLen);
void callback_LOG_INFO_LogLevel(const char *pStr,uint32_t u32StrLen);
void callback_LOG_INFO_FileName(const char *pStr,uint32_t u32StrLen);
void callback_LOG_INFO_FileSize(const char *pStr,uint32_t u32StrLen);
void callback_OTHER_INFO_BaudRate(const char *pStr,uint32_t u32StrLen);
void callback_OTHER_INFO_ChannelMask(const char *pStr,uint32_t u32StrLen);
void callback_OTHER_INFO_Verification(const char *pStr,uint32_t u32StrLen);
void callback_OTHER_INFO_PowerOut(const char *pStr,uint32_t u32StrLen);
void callback_OTHER_INFO_ClockOut(const char *pStr,uint32_t u32StrLen);
void callback_OTHER_INFO_IOMode(const char *pStr,uint32_t u32StrLen);

const static CFG_KEYWORD_STR ConfigKeyWord[] = 
{
  // Type                   pUpperLevel                           pLowerLevel								            pSameLevelNext							          strKeyWord      FunCallback
  {INI_KEYWORD_TYPE_ROOT    ,NULL                                 ,(CFG_KEYWORD_STR*)(ConfigKeyWord+1)	,NULL									                ,""             ,NULL},                             // 0
  {INI_KEYWORD_TYPE_SECTION	,NULL                                 ,(CFG_KEYWORD_STR*)(ConfigKeyWord+2)	,(CFG_KEYWORD_STR*)(ConfigKeyWord+3)	,"FILE_INFO"	  ,NULL},	                            // 1
  {INI_KEYWORD_TYPE_KEY     ,(CFG_KEYWORD_STR*)(ConfigKeyWord+1)	,NULL									                ,NULL									                ,"Version"	    ,callback_FILE_INFO_Version},	      // 2
  {INI_KEYWORD_TYPE_SECTION	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+4)	,(CFG_KEYWORD_STR*)(ConfigKeyWord+6)	,"DEVICE_INFO"	,NULL},                     	      // 3
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+3)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+5)	,"Device"	      ,callback_DEVICE_INFO_Device},	    // 4
	{INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+3)	,NULL									                ,NULL									                ,"Flash"	      ,callback_DEVICE_INFO_Flash},	      // 5
	{INI_KEYWORD_TYPE_SECTION	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+7)	,(CFG_KEYWORD_STR*)(ConfigKeyWord+11)	,"MAC_INFO"	    ,NULL},	                            // 6
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+6)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+8)	,"Mode"	        ,callback_MAC_INFO_Mode},	          // 7
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+6)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+9)	,"ErrorMode"	  ,callback_MAC_INFO_ErrorMode},	    // 8
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+6)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+10)	,"StartAddr"	  ,callback_MAC_INFO_StartAddr},	    // 9
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+6)	,NULL									                ,NULL									                ,"FileName"	    ,callback_MAC_INFO_FileName},	      // 10
  {INI_KEYWORD_TYPE_SECTION	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+12)	,(CFG_KEYWORD_STR*)(ConfigKeyWord+17)	,"OTP_INFO"	    ,NULL},	                            // 11
	{INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+11)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+13)	,"OTPMask"	    ,callback_OTP_INFO_OTPMask},	      // 12
	{INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+11)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+14)	,"AES"	        ,callback_OTP_INFO_AES},	          // 13
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+11)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+15)	,"UserData0"	  ,callback_OTP_INFO_UserData0},	    // 14
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+11)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+16)	,"UserData1"	  ,callback_OTP_INFO_UserData1},	    // 15
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+11)	,NULL									                ,NULL									                ,"UserData2"	  ,callback_OTP_INFO_UserData2},	    // 16
  {INI_KEYWORD_TYPE_SECTION	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+18)	,(CFG_KEYWORD_STR*)(ConfigKeyWord+21)	,"LOG_INFO"	    ,NULL},	                            // 17
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+17)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+19)	,"LogLevel"	    ,callback_LOG_INFO_LogLevel},	      // 18
	{INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+17)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+20)	,"FileName"	    ,callback_LOG_INFO_FileName},	      // 19
	{INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+17)	,NULL									                ,NULL									                ,"FileSize"	    ,callback_LOG_INFO_FileSize},	      // 20
  {INI_KEYWORD_TYPE_SECTION	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+22)	,NULL									                ,"OTHER_INFO"	  ,NULL},	                            // 21
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+21)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+23)	,"BaudRate"	    ,callback_OTHER_INFO_BaudRate},	    // 22
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+21)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+24)	,"ChannelMask"	,callback_OTHER_INFO_ChannelMask},	// 23
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+21)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+25)	,"Verification"	,callback_OTHER_INFO_Verification},	// 24
  {INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+21)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+26)	,"PowerOut"	    ,callback_OTHER_INFO_PowerOut},	    // 25
	{INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+21)	,NULL									                ,(CFG_KEYWORD_STR*)(ConfigKeyWord+27)	,"ClockOut"	    ,callback_OTHER_INFO_ClockOut},	    // 26
	{INI_KEYWORD_TYPE_KEY		  ,(CFG_KEYWORD_STR*)(ConfigKeyWord+21)	,NULL									                ,NULL									                ,"IOMode"	      ,callback_OTHER_INFO_IOMode},	      // 27
};
// 
void config_file_statemachine(const char *pStr,uint32_t u32StrLen)
{
  static CFG_KEYWORD_STR *pKeyWordStrCur = (CFG_KEYWORD_STR *)ConfigKeyWord;       // Saved the current pointer
  CFG_KEYWORD_STR *pKeyWordStrTemp;
  
  if(u32StrLen != 0)
  {
    if((*pStr == '[')&&(*(pStr + u32StrLen - 1) == ']'))        // Section
    {
      pKeyWordStrTemp = ConfigKeyWord->pLowerLevel;
      while(1)
      {
        if(pKeyWordStrTemp == NULL)
          return;
        
        if((strlen(pKeyWordStrTemp->pStrKeyWord) == u32StrLen - 2)&&\
          (strncmp(pStr+1,pKeyWordStrTemp->pStrKeyWord,u32StrLen-2) == 0))   // Match
        {
          pKeyWordStrCur = pKeyWordStrTemp;
          return;
        }else
          pKeyWordStrTemp = pKeyWordStrTemp->pSameLevelNext;
      }
    }else if(*pStr == ';')      //Annotation
    {
      asm("nop");
    }else                       // Key
    {
      char *pKeyEnd,*pValueStart,*pTemp;
      pTemp = (char*)strchr(pStr,'=');
      if(pTemp == NULL)
        return;
      else
      {
        pKeyEnd = pTemp - 1;
        pValueStart = pTemp + 1;
      }
      
      pKeyWordStrTemp = pKeyWordStrCur->pLowerLevel;
      while(1)
      {
        if(pKeyWordStrTemp == NULL)
          return;
        
        if((strlen(pKeyWordStrTemp->pStrKeyWord) == (pKeyEnd - pStr + 1))&&\
          (strncmp(pStr,pKeyWordStrTemp->pStrKeyWord,pKeyEnd - pStr + 1) == 0))   // Match
        {
          if(pKeyWordStrTemp->FunCallback != NULL)
            (*(pKeyWordStrTemp->FunCallback))(pValueStart,u32StrLen - (pValueStart - pStr));
          return;
        }else
          pKeyWordStrTemp = pKeyWordStrTemp->pSameLevelNext;
      }
    }
  }
}

void callback_FILE_INFO_Version(const char *pStr,uint32_t u32StrLen)
{
  u32CfgMask |= CFG_FILEINFO_VERSION_MASK;
  return;
}

void callback_DEVICE_INFO_Device(const char *pStr,uint32_t u32StrLen)
{
  uint32_t i;
  for(i = 0;i < sizeof(ChipIDStr)/sizeof(CHIP_ID_STR);i++)
  {
    if(strcmp((const char*)pStr,(ChipIDStr+i)->cChipIDStr) == 0)
    {
      SystemInfo.ChipID.u32ChipID = (ChipIDStr+i)->ChipID.u32ChipID;
      log(LOG_LEVEL_MESSAGE,"Target Chip Set as [%s]",(ChipIDStr+i)->cChipIDStr);
      u32CfgMask |= CFG_DEVICEINFO_DEVICE_MASK;
      return;
    }
  }
  SystemInfo.ChipID.u32ChipID = SystemInfoDef.ChipID.u32ChipID;
  log(LOG_LEVEL_ERROR,"Get Target Chip Error,[%s]",pStr);
}

void callback_DEVICE_INFO_Flash(const char *pStr,uint32_t u32StrLen)
{
  uint32_t i;
  for(i = 0;i < sizeof(FlashTypeStr)/sizeof(FLASH_TYPE_STR);i++)
  {
    if(strcmp((const char*)pStr,(FlashTypeStr+i)->cFlashTypeStr) == 0)
    {
      SystemInfo.FlashType = (FlashTypeStr+i)->FlashType;
      log(LOG_LEVEL_MESSAGE,"Flash Type Set as [%s]",(FlashTypeStr+i)->cFlashTypeStr);
      u32CfgMask |= CFG_DEVICEINFO_FLASH_MASK;
      return;
    }
  }
  SystemInfo.FlashType = SystemInfoDef.FlashType;
  log(LOG_LEVEL_ERROR,"Get Flash Type Failure,[%s]",pStr);
}

void callback_MAC_INFO_Mode(const char *pStr,uint32_t u32StrLen)
{
  uint32_t i;
  for(i = 0;i < sizeof(MacModeStr)/sizeof(MAC_MODE_STR);i++)
  {
    if(strcmp((const char*)pStr,(MacModeStr+i)->cMacModeStr) == 0)
    {
      SystemInfo.Mac_Mode = (MacModeStr+i)->MacMode;
      log(LOG_LEVEL_MESSAGE,"Mac Mode Set as [%s]",(MacModeStr+i)->cMacModeStr);
      u32CfgMask |= CFG_MACINFO_MODE_MASK;
      return;
    }
  }
  SystemInfo.Mac_Mode = SystemInfoDef.Mac_Mode;
  log(LOG_LEVEL_ERROR,"Get Mac Mode Failure,[%s]",pStr);
}

void callback_MAC_INFO_ErrorMode(const char *pStr,uint32_t u32StrLen)
{
  uint32_t i;
  for(i = 0;i < sizeof(MacErrorModeStr)/sizeof(MAC_ERROR_MODE_STR);i++)
  {
    if(strcmp((const char*)pStr,(MacErrorModeStr+i)->cMacErrorModeStr) == 0)
    {
      SystemInfo.Mac_ErrorMode = (MacErrorModeStr+i)->MacErrorMode;
      log(LOG_LEVEL_MESSAGE,"Mac Error Mode Set as [%s]",(MacErrorModeStr+i)->cMacErrorModeStr);
      u32CfgMask |= CFG_MACINFO_ERRORMODE_MASK;
      return;
    }
  }
  SystemInfo.Mac_ErrorMode = SystemInfoDef.Mac_ErrorMode;
  log(LOG_LEVEL_ERROR,"Get Mac Error Mode Failure,[%s]",pStr);
}

void callback_MAC_INFO_StartAddr(const char *pStr,uint32_t u32StrLen)
{
  MAC_ADDR MacAddrTemp;
  uint8_t u8Temp;
  uint8_t u8Err = 0;
  
  if(u32StrLen <= 16)
  {
    MacAddrTemp.u64MacAddr = 0;
    for(uint32_t i = 0;i < u32StrLen;i++)
    {
      if((*(pStr+i) >= '0')&&(*(pStr+i) <= '9'))
        u8Temp = *(pStr+i) - '0';
      else if((*(pStr+i) >= 'A')&&(*(pStr+i) <= 'F'))
        u8Temp = *(pStr+i) - 'A' + 10;
      else if((*(pStr+i) >= 'a')&&(*(pStr+i) <= 'f'))
        u8Temp = *(pStr+i) - 'a' + 10;
      else
      {
        u8Err = 1;
        break;
      }
      MacAddrTemp.u64MacAddr <<= 4;
      MacAddrTemp.u64MacAddr += u8Temp;
    }
  }else
    u8Err = 1;
  
  if(u8Err)
  {
    SystemInfo.Mac_StartAddr.u64MacAddr = SystemInfoDef.Mac_StartAddr.u64MacAddr;
    log(LOG_LEVEL_ERROR,"Get Mac Start Addr Failure,[%s]",pStr);
  }else
  {
    SystemInfo.Mac_StartAddr.u64MacAddr = MacAddrTemp.u64MacAddr;
    log(LOG_LEVEL_MESSAGE,"Get Mac Start Addr:[%08X%08X]",\
      SystemInfo.Mac_StartAddr.u32MacAddr[1],SystemInfo.Mac_StartAddr.u32MacAddr[0]);
    u32CfgMask |= CFG_MACINFO_STARTADDR_MASK;
  }
}

void callback_MAC_INFO_FileName(const char *pStr,uint32_t u32StrLen)
{
  char *p;
  char StrFileName[16];
  memset(StrFileName,'\0',sizeof(StrFileName));
  p = (char*)strchr(pStr,'.');
  if(p-pStr < 1)
    strcpy(StrFileName,"log_def");
  else if(p-pStr > 8)
  {
    strncpy(StrFileName,pStr,7);
    StrFileName[7] = '~';
    StrFileName[8] = '\0';
  }else
    strncpy(StrFileName,pStr,p-pStr);
  
  strcat(StrFileName,".");
  
  if(u32StrLen + pStr - p < 1)
    strcat(StrFileName,"txt");
  else if(u32StrLen + pStr - p > 3)
    strncat(StrFileName,(const char*)(p+1),3);
  else
    strcat(StrFileName,(const char*)(p+1));
  
  log(LOG_LEVEL_MESSAGE,"Get Mac FileName:[%s]",StrFileName);
  strcpy(SystemInfo.Mac_FileName,(const char*)StrFileName);
  u32CfgMask |= CFG_MACINFO_FILENAME_MASK;
}

void callback_OTP_INFO_OTPMask(const char *pStr,uint32_t u32StrLen)
{
  uint8_t u8MaskTemp;
  char *p;
  u8MaskTemp = strtol(pStr,&p,16);
  if((*p == '\0')&&(u8MaskTemp <= 0x0F))
  {
    if(u8MaskTemp&0x01)
      SystemInfo.bPrgAES = 1;
    else
      SystemInfo.bPrgAES = 0;
    if(u8MaskTemp&0x02)
      SystemInfo.bPrgUserData0 = 1;
    else
      SystemInfo.bPrgUserData0 = 0;
    if(u8MaskTemp&0x04)
      SystemInfo.bPrgUserData1 = 1;
    else
      SystemInfo.bPrgUserData1 = 0;
    if(u8MaskTemp&0x08)
      SystemInfo.bPrgUserData2 = 1;
    else
      SystemInfo.bPrgUserData2 = 0;
    log(LOG_LEVEL_MESSAGE,"Get OTP Mask:[%X]",u8MaskTemp);
    u32CfgMask |= CFG_OTPINFO_OTPMASK_MASK;
  }else
  {
    SystemInfo.bPrgAES = SystemInfoDef.bPrgAES;
    SystemInfo.bPrgUserData0 = SystemInfoDef.bPrgUserData0;
    SystemInfo.bPrgUserData1 = SystemInfoDef.bPrgUserData1;
    SystemInfo.bPrgUserData2 = SystemInfoDef.bPrgUserData2;
    log(LOG_LEVEL_ERROR,"Get OTP Mask Failure,[%s]",pStr);
  }
}

void callback_OTP_INFO_AES(const char *pStr,uint32_t u32StrLen)
{
  char cBuffTemp[10],*p;
  uint8_t i = 0;
  uint32_t u32Temp;
  
  if(u32StrLen != 32)
  {
    SystemInfo.AES[0] = SystemInfoDef.AES[0];
    SystemInfo.AES[1] = SystemInfoDef.AES[1];
    SystemInfo.AES[2] = SystemInfoDef.AES[2];
    SystemInfo.AES[3] = SystemInfoDef.AES[3];
    SystemInfo.bPrgAES = SystemInfoDef.bPrgAES;
    log(LOG_LEVEL_ERROR,"Get AES Failure,[%s]",pStr);
  }else
  {
    for(i = 0;i < 4;i++)
    {
      strncpy(cBuffTemp,pStr+8*i,8);
      cBuffTemp[8] = '\0';
      u32Temp = strtol(cBuffTemp,&p,16);
      if(*p == '\0')
        SystemInfo.AES[3-i] = u32Temp;
      else
        break;
    }
    if(i == 4)
    {
      log(LOG_LEVEL_MESSAGE,"Get AES,[%08X %08X %08X %08x]",\
        SystemInfo.AES[3],SystemInfo.AES[2],SystemInfo.AES[1],SystemInfo.AES[0]);
      u32CfgMask |= CFG_OTPINFO_AES_MASK;
    }else
    {
      SystemInfo.AES[0] = SystemInfoDef.AES[0];
      SystemInfo.AES[1] = SystemInfoDef.AES[1];
      SystemInfo.AES[2] = SystemInfoDef.AES[2];
      SystemInfo.AES[3] = SystemInfoDef.AES[3];
      SystemInfo.bPrgAES = SystemInfoDef.bPrgAES;
      log(LOG_LEVEL_ERROR,"Get AES Failure,[%s]",pStr);
    }
  }
}

void callback_OTP_INFO_UserData0(const char *pStr,uint32_t u32StrLen)
{
  char cBuffTemp[10],*p;
  uint8_t i = 0;
  uint32_t u32Temp;
  
  if(u32StrLen != 32)
  {
    SystemInfo.UserData[0][0] = SystemInfoDef.UserData[0][0];
    SystemInfo.UserData[0][1] = SystemInfoDef.UserData[0][1];
    SystemInfo.UserData[0][2] = SystemInfoDef.UserData[0][2];
    SystemInfo.UserData[0][3] = SystemInfoDef.UserData[0][3];
    SystemInfo.bPrgUserData0 = SystemInfoDef.bPrgUserData0;
    log(LOG_LEVEL_ERROR,"Get UserData0 Failure,[%s]",pStr);
    return;
  }else
  {
    for(i = 0;i < 4;i++)
    {
      strncpy(cBuffTemp,pStr+8*i,8);
      cBuffTemp[8] = '\0';
      u32Temp = strtol(cBuffTemp,&p,16);
      if(*p == '\0')
        SystemInfo.UserData[0][3-i] = u32Temp;
      else
        break;
    }
    if(i == 4)
    {
      log(LOG_LEVEL_MESSAGE,"Get UserData0,[%08X %08X %08X %08x]",\
        SystemInfo.UserData[0][3],SystemInfo.UserData[0][2],\
          SystemInfo.UserData[0][1],SystemInfo.UserData[0][0]);
      u32CfgMask |= CFG_OTPINFO_USERDATA0_MASK;
    }else
    {
      SystemInfo.UserData[0][0] = SystemInfoDef.UserData[0][0];
      SystemInfo.UserData[0][1] = SystemInfoDef.UserData[0][1];
      SystemInfo.UserData[0][2] = SystemInfoDef.UserData[0][2];
      SystemInfo.UserData[0][3] = SystemInfoDef.UserData[0][3];
      SystemInfo.bPrgUserData0 = SystemInfoDef.bPrgUserData0;
      log(LOG_LEVEL_ERROR,"Get UserData[0] Failure,[%s]",pStr);
    }
  }
}

void callback_OTP_INFO_UserData1(const char *pStr,uint32_t u32StrLen)
{char cBuffTemp[10],*p;
uint8_t i = 0;
uint32_t u32Temp;

if(u32StrLen != 32)
{
  SystemInfo.UserData[1][0] = SystemInfoDef.UserData[1][0];
  SystemInfo.UserData[1][1] = SystemInfoDef.UserData[1][1];
  SystemInfo.UserData[1][2] = SystemInfoDef.UserData[1][2];
  SystemInfo.UserData[1][3] = SystemInfoDef.UserData[1][3];
  SystemInfo.bPrgUserData1 = SystemInfoDef.bPrgUserData1;
  log(LOG_LEVEL_ERROR,"Get UserData1 Failure,[%s]",pStr);
  return;
}else
{
  for(i = 0;i < 4;i++)
  {
    strncpy(cBuffTemp,pStr+8*i,8);
    cBuffTemp[8] = '\0';
    u32Temp = strtol(cBuffTemp,&p,16);
    if(*p == '\0')
      SystemInfo.UserData[1][3-i] = u32Temp;
    else
      break;
  }
  if(i == 4)
  {
    log(LOG_LEVEL_MESSAGE,"Get UserData1,[%08X %08X %08X %08x]",\
      SystemInfo.UserData[1][3],SystemInfo.UserData[1][2],\
        SystemInfo.UserData[1][1],SystemInfo.UserData[1][0]);
    u32CfgMask |= CFG_OTPINFO_USERDATA1_MASK;
  }else
  {
    SystemInfo.UserData[1][0] = SystemInfoDef.UserData[1][0];
    SystemInfo.UserData[1][1] = SystemInfoDef.UserData[1][1];
    SystemInfo.UserData[1][2] = SystemInfoDef.UserData[1][2];
    SystemInfo.UserData[1][3] = SystemInfoDef.UserData[1][3];
    SystemInfo.bPrgUserData1 = SystemInfoDef.bPrgUserData1;
    log(LOG_LEVEL_ERROR,"Get UserData1 Failure,[%s]",pStr);
  }
}
}

void callback_OTP_INFO_UserData2(const char *pStr,uint32_t u32StrLen)
{
  char cBuffTemp[10],*p;
  uint8_t i = 0;
  uint32_t u32Temp;
  
  if(u32StrLen != 32)
  {
    SystemInfo.UserData[2][0] = SystemInfoDef.UserData[2][0];
    SystemInfo.UserData[2][1] = SystemInfoDef.UserData[2][1];
    SystemInfo.UserData[2][2] = SystemInfoDef.UserData[2][2];
    SystemInfo.UserData[2][3] = SystemInfoDef.UserData[2][3];
    SystemInfo.bPrgUserData2 = SystemInfoDef.bPrgUserData2;
    log(LOG_LEVEL_ERROR,"Get UserData2 Failure,[%s]",pStr);
    return;
  }else
  {
    for(i = 0;i < 4;i++)
    {
      strncpy(cBuffTemp,pStr+8*i,8);
      cBuffTemp[8] = '\0';
      u32Temp = strtol(cBuffTemp,&p,16);
      if(*p == '\0')
        SystemInfo.UserData[2][3-i] = u32Temp;
      else
        break;
    }
    if(i == 4)
    {
      log(LOG_LEVEL_MESSAGE,"Get UserData2,[%08X %08X %08X %08x]",\
        SystemInfo.UserData[2][3],SystemInfo.UserData[2][2],\
          SystemInfo.UserData[2][1],SystemInfo.UserData[2][0]);
      u32CfgMask |= CFG_OTPINFO_USERDATA2_MASK;
    }else
    {
      SystemInfo.UserData[2][0] = SystemInfoDef.UserData[2][0];
      SystemInfo.UserData[2][1] = SystemInfoDef.UserData[2][1];
      SystemInfo.UserData[2][2] = SystemInfoDef.UserData[2][2];
      SystemInfo.UserData[2][3] = SystemInfoDef.UserData[2][3];
      SystemInfo.bPrgUserData2 = SystemInfoDef.bPrgUserData2;
      log(LOG_LEVEL_ERROR,"Get UserData2 Failure,[%s]",pStr);
    }
  }
}

void callback_LOG_INFO_LogLevel(const char *pStr,uint32_t u32StrLen)
{
  uint32_t i;
  for(i = 0;i < sizeof(LogLevelStr)/sizeof(LOG_LEVEL_STR);i++)
  {
    if(strcmp((const char*)pStr,(LogLevelStr+i)->cLogLevelStr) == 0)
    {
      SystemInfo.Log_Level = (LogLevelStr+i)->LogLevel;
      log(LOG_LEVEL_MESSAGE,"Log Level Set as [%s]",(LogLevelStr+i)->cLogLevelStr);
      u32CfgMask |= CFG_LOGINFO_LOGLEVEL_MASK;
      return;
    }
  }
  SystemInfo.Log_Level = SystemInfoDef.Log_Level;
  log(LOG_LEVEL_ERROR,"Get Log Level Failure,[%s]",pStr);
}

void callback_LOG_INFO_FileName(const char *pStr,uint32_t u32StrLen)
{
  char *p;
  char StrFileName[16];
  memset(StrFileName,'\0',sizeof(StrFileName));
  p = (char*)strchr(pStr,'.');
  if(p-pStr < 1)
    strcpy(StrFileName,"mac_def");
  else if(p-pStr > 8)
  {
    strncpy(StrFileName,pStr,7);
    StrFileName[7] = '~';
    StrFileName[8] = '\0';
  }else
    strncpy(StrFileName,pStr,p-pStr);
  
  strcat(StrFileName,".");
  
  if(u32StrLen + pStr - p < 1)
    strcat(StrFileName,"txt");
  else if(u32StrLen + pStr - p > 3)
    strncat(StrFileName,(const char*)(p+1),3);
  else
    strcat(StrFileName,(const char*)(p+1));
  
  log(LOG_LEVEL_MESSAGE,"Get Log FileName:[%s]",StrFileName);
  strcpy(SystemInfo.Log_FileName,(const char*)StrFileName);
  u32CfgMask |= CFG_LOGINFO_FILENAME_MASK;
}

void callback_LOG_INFO_FileSize(const char *pStr,uint32_t u32StrLen)
{
  uint32_t u32SizeTemp;
  char *p;
  u32SizeTemp = strtol(pStr,&p,10);
  if(*p == '\0')
  {
    SystemInfo.Log_FileSizeMax = u32SizeTemp;
    log(LOG_LEVEL_MESSAGE,"Get Log File Max Size:[%d]",SystemInfo.Log_FileSizeMax);
    u32CfgMask |= CFG_LOGINFO_FILESIZE_MASK;
  }else
  {
    SystemInfo.Log_FileSizeMax = SystemInfoDef.Log_FileSizeMax;
    log(LOG_LEVEL_ERROR,"Get Log File Max Size Failure,[%s]",pStr);
  }
}

void callback_OTHER_INFO_BaudRate(const char *pStr,uint32_t u32StrLen)
{
  uint32_t u32BaudRateTemp;
  char *p;
  u32BaudRateTemp = strtol(pStr,&p,10);
  if(*p == '\0')
  {
    SystemInfo.BaudRate = u32BaudRateTemp;
    log(LOG_LEVEL_MESSAGE,"Get Baud Rate:[%d]",SystemInfo.BaudRate);
    u32CfgMask |= CFG_OTHERINFO_BAUDRATE_MASK;
  }else
  {
    SystemInfo.BaudRate = SystemInfoDef.BaudRate;
    log(LOG_LEVEL_ERROR,"Get Baud Rate Failure,[%s]",pStr);
  }
}

void callback_OTHER_INFO_ChannelMask(const char *pStr,uint32_t u32StrLen)
{
  uint8_t u8MaskTemp;
  char *p;
  u8MaskTemp = strtol(pStr,&p,16);
  if((*p == '\0')&&(u8MaskTemp <= 0x0F))
  {
    SystemInfo.ChannelMask = u8MaskTemp;
    log(LOG_LEVEL_MESSAGE,"Get Channel Mask:[%X]",SystemInfo.ChannelMask);
    u32CfgMask |= CFG_OTHERINFO_CHANNELMASK_MASK;
  }else
  {
    SystemInfo.ChannelMask = SystemInfoDef.ChannelMask;
    log(LOG_LEVEL_ERROR,"Get Channel Mask Failure,[%s]",pStr);
  }
}

void callback_OTHER_INFO_Verification(const char *pStr,uint32_t u32StrLen)
{
  if(strcmp((const char*)pStr,"0") == 0)
  {
    SystemInfo.Verification = 0;
    log(LOG_LEVEL_MESSAGE,"Get Verification Option:[0]");
    u32CfgMask |= CFG_OTHERINFO_VERIFICTION_MASK;
  }else if(strcmp((const char*)pStr,"1") == 0)
  {
    SystemInfo.Verification = 1;
    log(LOG_LEVEL_MESSAGE,"Get Verification Option:[1]");
    u32CfgMask |= CFG_OTHERINFO_VERIFICTION_MASK;
  }else
  {
    SystemInfo.Verification = SystemInfoDef.Verification;
    log(LOG_LEVEL_MESSAGE,"Get Verification Failure,[%s]",pStr);
  }
}

void callback_OTHER_INFO_PowerOut(const char *pStr,uint32_t u32StrLen)
{
  if(strcmp((const char*)pStr,"0") == 0)
  {
    SystemInfo.PowerOut = false;
    log(LOG_LEVEL_MESSAGE,"Get PowerOut Option:[0]");
    u32CfgMask |= CFG_OTHERINFO_POWEROUT_MASK;
  }else if(strcmp((const char*)pStr,"1") == 0)
  {
    SystemInfo.PowerOut = true;
    log(LOG_LEVEL_MESSAGE,"Get PowerOut Option:[1]");
    u32CfgMask |= CFG_OTHERINFO_POWEROUT_MASK;
  }else
  {
    SystemInfo.PowerOut = SystemInfoDef.PowerOut;
    log(LOG_LEVEL_MESSAGE,"Get PowerOut Failure,[%s]",pStr);
  }
}

void callback_OTHER_INFO_ClockOut(const char *pStr,uint32_t u32StrLen)
{
  if(strcmp((const char*)pStr,"0") == 0)
  {
    SystemInfo.ClockOut = false;
    log(LOG_LEVEL_MESSAGE,"Get ClockOut Option:[0]");
    u32CfgMask |= CFG_OTHERINFO_CLOCKOUT_MASK;
  }else if(strcmp((const char*)pStr,"1") == 0)
  {
    SystemInfo.ClockOut = true;
    log(LOG_LEVEL_MESSAGE,"Get ClockOut Option:[1]");
    u32CfgMask |= CFG_OTHERINFO_CLOCKOUT_MASK;
  }else
  {
    SystemInfo.ClockOut = SystemInfoDef.ClockOut;
    log(LOG_LEVEL_MESSAGE,"Get ClockOut Failure,[%s]",pStr,SystemInfo.ClockOut);
  }
  BSP_GPIO_CLKOUT_Config(SystemInfo.ClockOut);
}

void callback_OTHER_INFO_IOMode(const char *pStr,uint32_t u32StrLen)
{
  uint32_t i;
  for(i = 0;i < sizeof(IOModeStr)/sizeof(LOG_IOMODE_STR);i++)
  {
    if(strcmp((const char*)pStr,(IOModeStr+i)->cIOModeStr) == 0)
    {
      SystemInfo.IOMode = (IOModeStr+i)->IOMode;
      BSP_GPIO_ChangeRSTSIGMode(SystemInfo.IOMode);
      log(LOG_LEVEL_MESSAGE,"IO Mode Set as [%s]",(IOModeStr+i)->cIOModeStr);
      u32CfgMask |= CFG_OTHERINFO_IOMODE_MASK;
      return;
    }
  }
  SystemInfo.IOMode = SystemInfoDef.IOMode;
  BSP_GPIO_ChangeRSTSIGMode(SystemInfo.IOMode);
  log(LOG_LEVEL_ERROR,"Get IO Mode Failure,[%s]",pStr);
}
