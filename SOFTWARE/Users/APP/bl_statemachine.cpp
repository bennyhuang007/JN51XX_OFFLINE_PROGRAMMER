#include "bl_statemachine.hpp"
#include "app_taskchipx.h"
#include "includes.h"
#include "app_taskmem.h"
#include "usart_driver.h"
#include "app_taskui.h"
#include "app_adc.h"
#include "ram_code.h"

#define TIME_OUT_MS 1000
#define FLASH_CODE_ADDR_OFFSET  0x00000000
#define RAM_CODE_ADDR_OFFSET    0x04000400

BL_STATUSMACHINE::BL_STATUSMACHINE(HARDWARE_PARAM *pInitParam)
{
  pHardwareParam = pInitParam;
  MemMsg.DataExist = 0;
  MacMsg.DataExist = 0;
  eErrCode = ERR_NORMAL;
  eState = BL_STATE_IDAL;
}

BL_STATUSMACHINE::~BL_STATUSMACHINE(void)
{
}

void BL_STATUSMACHINE::StatusMachine(void)
{
  while(1)
  {
    switch(eState)
    {
    case BL_STATE_IDAL:
      if(Idal())
        eState = BL_STATE_POWER_OUT;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATE_POWER_OUT:
      if(PowerOut())
        eState = BL_STATE_POWER_CHECK;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATE_POWER_CHECK:
      if(PowerCheck())
        eState = BL_STATE_INIT;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATE_INIT:
      if(Init())
        eState = BL_STATE_GET_ID;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATE_GET_ID:
      if(GetChipID())
        eState = BL_STATE_SET_STATUS_REG;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATE_SET_STATUS_REG:
      if(SetStatusReg())
        eState = BL_STATE_ERASE;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATE_ERASE:
      if(Erase())
        eState = BL_STATE_CHANGE_BAUDRATE;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATE_CHANGE_BAUDRATE:
      if(SystemInfo.BaudRate != 38400)
      {
        if(ChangeBaudRate())
          eState = BL_STATE_PROG_FLASH;
        else
          eState = BL_STATE_ERROR;
      }else
        eState = BL_STATE_PROG_FLASH;
      break;
    case BL_STATE_PROG_FLASH:
      if(ProgramFlash())
        eState = BL_STATE_VERIFICATION;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATE_VERIFICATION:
      if(SystemInfo.Verification)
      {
        if(Verification())
          eState = BL_STATE_WRITE_RAMCODE;
        else
          eState = BL_STATE_ERROR;
      }else
        eState = BL_STATE_WRITE_RAMCODE;
      break;
    case BL_STATE_WRITE_RAMCODE:
      if(SystemInfo.bPrgAES||SystemInfo.bPrgUserData0||SystemInfo.bPrgUserData1||\
        SystemInfo.bPrgUserData2||(SystemInfo.Mac_Mode!=MAC_MODE_NOCHANGE))
      {
        if(WriteRamCode())
          eState = BL_STATW_PROG_MAC;
        else
          eState = BL_STATE_ERROR;
      }else
        eState = BL_STATE_SUCCESS;
      break;
    case BL_STATE_RUN_RAMCODE:
      if(Run(0x04000524))
        eState = BL_STATW_PROG_MAC;
      else
        eState = BL_STATE_ERROR;
      break;
    case BL_STATW_PROG_MAC:
      if(SystemInfo.Mac_Mode!=MAC_MODE_NOCHANGE)
      {
        if(ProgMAC())
          eState = BL_STATW_PROG_AES;
        else
          eState = BL_STATE_ERROR;
      }else
        eState = BL_STATW_PROG_AES;
      break;
    case BL_STATW_PROG_AES:
      if(SystemInfo.bPrgAES)
      {
        uint8_t u8AES[16];
        uint8_t i;
        for(i = 0;i < 16;i++)
          u8AES[i] = (SystemInfo.AES[i>>2] >> (8*i))&0x0FF;
        if(ProgOTP(OTP_AES,u8AES))
          eState = BL_STATW_PROG_USERDATA0;
        else
          eState = BL_STATE_ERROR;
      }else
        eState = BL_STATW_PROG_USERDATA0;
      break;
    case BL_STATW_PROG_USERDATA0:
      if(SystemInfo.bPrgUserData0)
      {
        uint8_t u8UserData[16];
        uint8_t i;
        for(i = 0;i < 16;i++)
          u8UserData[i] = (SystemInfo.UserData[0][i>>2] >> (8*i))&0x0FF;
        if(ProgOTP(OTP_USERDATA0,u8UserData))
          eState = BL_STATW_PROG_USERDATA1;
        else
          eState = BL_STATE_ERROR;
      }else
        eState = BL_STATW_PROG_USERDATA1;
      break;
    case BL_STATW_PROG_USERDATA1:
      if(SystemInfo.bPrgUserData1)
      {
        uint8_t u8UserData[16];
        uint8_t i;
        for(i = 0;i < 16;i++)
          u8UserData[i] = (SystemInfo.UserData[1][i>>2] >> (8*i))&0x0FF;
        if(ProgOTP(OTP_USERDATA1,u8UserData))
          eState = BL_STATW_PROG_USERDATA2;
        else
          eState = BL_STATE_ERROR;
      }else
        eState = BL_STATW_PROG_USERDATA2;
      break;
    case BL_STATW_PROG_USERDATA2:
      if(SystemInfo.bPrgUserData2)
      {
        uint8_t u8UserData[16];
        uint8_t i;
        for(i = 0;i < 16;i++)
          u8UserData[i] = (SystemInfo.UserData[2][i>>2] >> (8*i))&0x0FF;
        if(ProgOTP(OTP_USERDATA2,u8UserData))
          eState = BL_STATE_SUCCESS;
        else
          eState = BL_STATE_ERROR;
      }else
        eState = BL_STATE_SUCCESS;
      break;
    case BL_STATE_ERROR:
      Error();
      eState = BL_STATE_IDAL;
      break;
    case BL_STATE_SUCCESS:
      Success();
      eState = BL_STATE_IDAL;
      break;
    }
    OSTimeDly(1);
  }
}

bool BL_STATUSMACHINE::Idal(void)
{
  uint8_t u8Err;
  
  GPIO_SetBits(pHardwareParam->GPIOx_RST,pHardwareParam->GPIO_RST_Pin);
  GPIO_SetBits(pHardwareParam->GPIOx_SIG,pHardwareParam->GPIO_SIG_Pin);
  GPIO_ResetBits(pHardwareParam->GPIOx_PWR,pHardwareParam->GPIO_PWR_Pin);
  eErrCode = ERR_NORMAL;
  OSFlagPend(FlagChipxStatus,pHardwareParam->FLAG_CHIPx_START_MASK,\
    OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0,&u8Err);
  if(u8Err == OS_NO_ERR)
  {
    OSFlagPost(FlagChipxStatus,pHardwareParam->FLAG_CHIPx_RUNNING_MASK,OS_FLAG_SET,&u8Err);
    UI_LED_Switch(pHardwareParam->eUILed,UI_LED_STATUS_TWINK,UI_LED_STATUS_TWINK);
    log(LOG_LEVEL_MESSAGE,"Chip%c Started",pHardwareParam->cChipNum);
    return true;
  }else
  {
    log(LOG_LEVEL_ERROR,"Chip%c ID Unknown Error E[%X]",\
      pHardwareParam->cChipNum,u8Err);
    return false;
  }
}

bool BL_STATUSMACHINE::PowerOut(void)
{
  if(SystemInfo.PowerOut)
  {
    GPIO_SetBits(pHardwareParam->GPIOx_PWR,pHardwareParam->GPIO_PWR_Pin);
    OSTimeDly(OS_TICKS_PER_SEC / 2);  // 500ms
  }
  return true;
}

bool BL_STATUSMACHINE::PowerCheck(void)
{
  uint32_t u32VOL_MV = ADC_RESULT_MV(*(pHardwareParam->pu32ADCValue));
  if((u32VOL_MV>2900)&&(u32VOL_MV<3600))
  {
    log(LOG_LEVEL_MESSAGE,"Chip%c Voltage is [%d]mV",pHardwareParam->cChipNum,u32VOL_MV);
    return true;
  }else
  {
    log(LOG_LEVEL_ERROR,"Chip%c Voltage is [%d]mV",pHardwareParam->cChipNum,u32VOL_MV);
    eErrCode = ERR_SM_TARGET_VOLTAGE_ERROR;
    return false;
  }
}

bool BL_STATUSMACHINE::Init(void)
{
  GPIO_ResetBits(pHardwareParam->GPIOx_RST,pHardwareParam->GPIO_RST_Pin);
  GPIO_ResetBits(pHardwareParam->GPIOx_SIG,pHardwareParam->GPIO_SIG_Pin);
  OSTimeDly(OS_TICKS_PER_SEC / 5);  // 200ms
  GPIO_SetBits(pHardwareParam->GPIOx_RST,pHardwareParam->GPIO_RST_Pin);
  OSTimeDly(OS_TICKS_PER_SEC / 3);  // 300+ms
  GPIO_SetBits(pHardwareParam->GPIOx_SIG,pHardwareParam->GPIO_SIG_Pin);
  return true;
}

bool BL_STATUSMACHINE::GetChipID(void)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint32_t u32ReadLen;
  CHIP_ID ChipID;
  
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    pu8Buff[0] = 2;
    pu8Buff[1] = BL_MSG_GET_CHIP_ID_REQ;
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < TIME_OUT_MS;i++)  //<_MARK(A)_ [TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == BL_MSG_GET_CHIP_ID_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          ChipID.u8ChipID[3] = pu8Buff[3];
          ChipID.u8ChipID[2] = pu8Buff[4];
          ChipID.u8ChipID[1] = pu8Buff[5];
          ChipID.u8ChipID[0] = pu8Buff[6];
          if((ChipID.u32ChipID == SystemInfo.ChipID.u32ChipID)&&(pu8Buff[2] == 0x00))
          {
            log(LOG_LEVEL_MESSAGE,"Get Chip%c ID [%08XH]",pHardwareParam->cChipNum,ChipID.u32ChipID);
            return true;
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_CHIP_ID_WRONG;
            break;
          }
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == TIME_OUT_MS)    //<_MARK(A)_ [TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_CHIP_ID_TIMEOUT;
    }
  }
  
  if(eErrCode == ERR_SM_CHIP_ID_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Get Chip%c ID TIMEOUT",pHardwareParam->cChipNum);
  else if(eErrCode == ERR_SM_CHIP_ID_WRONG)
    log(LOG_LEVEL_ERROR,"Get Chip%c ID WRONG [%02X%02X%02X%02XH]",\
      pHardwareParam->cChipNum,ChipID.u32ChipID);
  else
    log(LOG_LEVEL_ERROR,"Get Chip%c ID Unknown Error E[%X]",\
      pHardwareParam->cChipNum,eErrCode);
  return false;
}
/*
bool BL_STATUSMACHINE::GetDeviceID(void)
{
uint8_t pu8Buff[136];
uint8_t u8CheckSum;
uint32_t i,j;
uint8_t u8Retry = 0;
uint32_t u32ReadLen;
CHIP_ID ChipID;

while(u8Retry < SystemInfo.u8RetryMax)
{
pu8Buff[0] = 2;
pu8Buff[1] = BL_MSG_GET_CHIP_ID_REQ;

u8CheckSum = 0;
for(j = 0;j < pu8Buff[0];j++)
u8CheckSum ^= pu8Buff[j];
pu8Buff[pu8Buff[0]] = u8CheckSum;

USART2_RxBuffReset();
USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
u32ReadLen = 0;
//--------------------------------------------------------------------------
for(i = 0;i < TIME_OUT_MS;i++)  //<_MARK(A)_ [TIME_OUT_MS]
{
u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
if(u32ReadLen-1 == pu8Buff[0])
{
u8CheckSum = 0;
for(j = 0;j < pu8Buff[0];j++)
u8CheckSum ^= pu8Buff[j];

if((pu8Buff[1] == BL_MSG_GET_CHIP_ID_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
{
ChipID.u8ChipID[3] = pu8Buff[3];
ChipID.u8ChipID[2] = pu8Buff[4];
ChipID.u8ChipID[1] = pu8Buff[5];
ChipID.u8ChipID[0] = pu8Buff[6];
if((ChipID.u32ChipID == SystemInfo.ChipID.u32ChipID)&&(pu8Buff[2] == 0x00))
{
log(LOG_LEVEL_MESSAGE,"Get Chip%c ID [%08XH]",pHardwareParam->cChipNum,ChipID.u32ChipID);
return true;
          }else
{
u8Retry++;
eErrCode = ERR_SM_CHIP_ID_WRONG;
break;
          }
        }
      }
OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
if(i == TIME_OUT_MS)    //<_MARK(A)_ [TIME_OUT_MS]
{
u8Retry++;
eErrCode = ERR_SM_CHIP_ID_TIMEOUT;
    }
  }

if(eErrCode == ERR_SM_CHIP_ID_TIMEOUT)
log(LOG_LEVEL_ERROR,"Get Chip%c ID TIMEOUT",pHardwareParam->cChipNum);
  else if(eErrCode == ERR_SM_CHIP_ID_WRONG)
log(LOG_LEVEL_ERROR,"Get Chip%c ID WRONG [%02X%02X%02X%02XH]",\
pHardwareParam->cChipNum,ChipID.u32ChipID);
  else
log(LOG_LEVEL_ERROR,"Get Chip%c ID Unknown Error E[%X]",\
pHardwareParam->cChipNum,eErrCode);
return false;
}
*/
bool BL_STATUSMACHINE::SetStatusReg(void)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint32_t u32ReadLen;
  
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    pu8Buff[0] = 3;
    pu8Buff[1] = BL_MSG_WRITE_SR_REQ;
    pu8Buff[2] = 0xFF;
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < TIME_OUT_MS;i++)  //<_MARK(B)_ [TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == BL_MSG_WRITE_SR_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          if(pu8Buff[2] == 0x00)
          {
            log(LOG_LEVEL_MESSAGE,"Chip%c Write SR Success");
            return true;
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_WRITE_SR_FAILURE;
            break;
          }
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == TIME_OUT_MS)    //<_MARK(B)_ [TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_WRITE_SR_TIMEOUT;
    }
  }
  
  if(eErrCode == ERR_SM_WRITE_SR_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Chip%c Write SR TIMEOUT",pHardwareParam->cChipNum);
  else if(eErrCode == ERR_SM_WRITE_SR_FAILURE)
    log(LOG_LEVEL_ERROR,"Get Chip%c Write SR Failure R[%02XH]",\
      pHardwareParam->cChipNum,pu8Buff[2]);
  else
    log(LOG_LEVEL_ERROR,"Chip%c Write SR Unknown Error E[%X]",\
      pHardwareParam->cChipNum,eErrCode);
  return false;
}

bool BL_STATUSMACHINE::Erase(void)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint32_t u32ReadLen;
  
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    pu8Buff[0] = 2;
    pu8Buff[1] = BL_MSG_FLASH_ERASE_REQ;
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < 7*TIME_OUT_MS;i++)    //<_MARK(C)_ [7*TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == BL_MSG_FLASH_ERASE_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          if(pu8Buff[2] == 0x00)
          {
            log(LOG_LEVEL_MESSAGE,"Chip%c Erease Success");
            return true;
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_EREASE_FAILURE;
            break;
          }
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == 7*TIME_OUT_MS)    //<_MARK(C)_ [7*TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_EREASE_TIMEOUT;
    }
  }
  
  if(eErrCode == ERR_SM_EREASE_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Chip%c Erease TIMEOUT",pHardwareParam->cChipNum);
  else if(eErrCode == ERR_SM_WRITE_SR_FAILURE)
    log(LOG_LEVEL_ERROR,"Get Chip%c Erease Failure R[%02XH]",\
      pHardwareParam->cChipNum,pu8Buff[2]);
  else
    log(LOG_LEVEL_ERROR,"Chip%c Erease Unknown Error E[%X]",\
      pHardwareParam->cChipNum,eErrCode);
  return false;
}

bool BL_STATUSMACHINE::ChangeBaudRate(void)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint32_t u32ReadLen;
  
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    pu8Buff[0] = 3;
    pu8Buff[1] = BL_MSG_CHANGE_BAUDRATE_REQ;
    if(SystemInfo.BaudRate == 1000000ul)
      pu8Buff[2] = 1;
    else if(SystemInfo.BaudRate == 500000ul)
      pu8Buff[2] = 2;
    else if(SystemInfo.BaudRate == 115200ul)
      pu8Buff[2] = 9;
    else
    {
      SystemInfo.BaudRate = 38400;
      pu8Buff[2] = 26;        // 38400
    }
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < TIME_OUT_MS;i++)  //<_MARK(D)_ [TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == BL_MSG_CHANGE_BAUDRATE_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          if(pu8Buff[2] == 0x00)
          {
            log(LOG_LEVEL_MESSAGE,"Chip%c Write SR Success");
            // 修改STM32波特率
            // ...............
            return true;
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_CHANGE_BAUDRATE_FAILURE;
            break;
          }
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == TIME_OUT_MS)    //<_MARK(D)_ [TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_CHANGE_BAUDRATE_TIMEOUT;
    }
  }
  if(eErrCode == ERR_SM_CHANGE_BAUDRATE_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Chip%c Change Baudrate TIMEOUT",\
      pHardwareParam->cChipNum);
  else if(eErrCode == ERR_SM_CHANGE_BAUDRATE_FAILURE)
    log(LOG_LEVEL_ERROR,"Get Chip%c Change Baudrate Failure R[%02XH]",\
      pHardwareParam->cChipNum,pu8Buff[2]);
  else
    log(LOG_LEVEL_ERROR,"Chip%c Change Baudrate Unknown Error E[%X]",\
      pHardwareParam->cChipNum,eErrCode);
  return false;
}

bool BL_STATUSMACHINE::ProgramFlash(void)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint8_t u8ReqLen;
  uint32_t u32ReadLen;
  uint32_t u32CurAddr = 0;  // Current Address of the target file reading pointer
  uint32_t u32CurAddrTemp;  // Actual Addr in Flash
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    u8ReqLen = min(128,SystemInfo.u32TargetSize - u32CurAddr - 4);    // Ignore first 4Byte(Version)
    //---------------asking for data in target file-------------------------------
    MemMsg.DataExist = 0;
    MemMsg.pMemData = pu8Buff + 6;    // len(1byte) + cmd(1byte) + addr(4byte)
    MemMsg.u32Addr = u32CurAddr + 4;  // Ignore first 4Byte(Binary Version)
    MemMsg.u32ReqLen = u8ReqLen;
    OSQPost(QMemMsg,&MemMsg);
    // waitting for data
    i = 0;
    while((!MemMsg.DataExist)&&(i++ > TIME_OUT_MS))
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    if(i >= TIME_OUT_MS)
    {
      log(LOG_LEVEL_ERROR,"Chip%c asking data error @[%08XH]",\
        pHardwareParam->cChipNum,u32CurAddr+4);
      eErrCode = ERR_SM_ASK_DATA_FAILURE;
      return BL_STATE_ERROR; // Error is trigged by MCU,so return directly
    }
    // 
    u32CurAddrTemp = u32CurAddr + FLASH_CODE_ADDR_OFFSET;
    pu8Buff[0] = 6 + u8ReqLen;        // 1(len) + 1(cmd) + 4(addr) + data
    pu8Buff[1] = BL_MSG_FLASH_PROGRAM_REQ;
    
    pu8Buff[2] = (uint8_t)(u32CurAddrTemp & 0xFF);    // Addr LSB First
    pu8Buff[3] = (uint8_t)((u32CurAddrTemp>>8) & 0xFF);
    pu8Buff[4] = (uint8_t)((u32CurAddrTemp>>16) & 0xFF);
    pu8Buff[5] = (uint8_t)((u32CurAddrTemp>>24) & 0xFF);
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < 3*TIME_OUT_MS;i++)    //<_MARK(E)_ [3*TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == BL_MSG_FLASH_PROGRAM_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          if(pu8Buff[2] == 0x00)
          {
            // Success
            u8Retry = 0;
            u32CurAddr += u8ReqLen;
            if(u32CurAddr + 4 >= SystemInfo.u32TargetSize)
            {
              log(LOG_LEVEL_MESSAGE,"Chip%c Write Flash Success",pHardwareParam->cChipNum);
              return true;
            }
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_PROG_FLASH_FAILURE;
          }
          break;
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == 3*TIME_OUT_MS)  //<_MARK(E)_ [3*TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_PROG_FLASH_TIMEOUT;
    }
  }
  if(eErrCode == ERR_SM_PROG_FLASH_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Chip%c Program Flash TIMEOUT @[%08XH]",\
      pHardwareParam->cChipNum,u32CurAddrTemp);
  else if(eErrCode == ERR_SM_PROG_FLASH_FAILURE)
    log(LOG_LEVEL_ERROR,"Get Chip%c Program Flash Failure R[%02XH] @[%08XH]",\
      pHardwareParam->cChipNum,pu8Buff[2],u32CurAddrTemp);
  else
    log(LOG_LEVEL_ERROR,"Chip%c Program Flash Unknown Error E[%X] @[08XH]",\
      pHardwareParam->cChipNum,eErrCode,u32CurAddrTemp);
  return false;
}

bool BL_STATUSMACHINE::Verification(void)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint8_t u8ReqLen;
  uint32_t u32ReadLen;
  uint32_t u32CurAddr = 0;  // Current Address of the target file reading pointer
  uint32_t u32CRCValue = 0;
  uint32_t u32CurAddrTemp;
  
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    u8ReqLen = min(128,SystemInfo.u32TargetSize - u32CurAddr - 4);    // Ignore first 4Byte(Version)
    // 
    u32CurAddrTemp = u32CurAddr + FLASH_CODE_ADDR_OFFSET;
    pu8Buff[0] = 8;     // 1(len) + 1(cmd) + 4(addr) + 2(len)
    pu8Buff[1] = BL_MSG_FLASH_READ_REQ;
    
    pu8Buff[2] = (uint8_t)(u32CurAddrTemp & 0xFF);    // Addr LSB First
    pu8Buff[3] = (uint8_t)((u32CurAddrTemp>>8) & 0xFF);
    pu8Buff[4] = (uint8_t)((u32CurAddrTemp>>16) & 0xFF);
    pu8Buff[5] = (uint8_t)((u32CurAddrTemp>>24) & 0xFF);
    
    pu8Buff[6] = (uint8_t)((u8ReqLen) & 0xFF);
    pu8Buff[7] = (uint8_t)((u8ReqLen>>8) & 0xFF);
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < 3*TIME_OUT_MS;i++)    //<_MARK(F)_ [2*TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == BL_MSG_FLASH_READ_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          if(pu8Buff[2] == 0x00)
          {
            u8Retry = 0;
            u32CurAddr += u8ReqLen;
            u32CRCValue = CalculateBlockCRC32(u32CRCValue,pu8Buff+3,u8ReqLen);
            if(u32CurAddr + 4 >= SystemInfo.u32TargetSize)
            {
              if(u32CRCValue == SystemInfo.u32TargetCRC)
              {
                log(LOG_LEVEL_MESSAGE,"Chip%c Verific Success",pHardwareParam->cChipNum);
                return true;
              }else
              {
                eErrCode = ERR_SM_VERIFICATION_FAILURE;
                log(LOG_LEVEL_ERROR,"Chip%c Verific Failure F[%08XH] R[%08XH]",\
                  pHardwareParam->cChipNum,SystemInfo.u32TargetCRC,u32CRCValue);
                return false;
              }
            }
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_READ_FLASH_FAILURE;
          }
          break;
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == 3*TIME_OUT_MS)  //<_MARK(F)_ [2*TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_READ_FLASH_TIMEOUT;
    }
  }
  if(eErrCode == ERR_SM_READ_FLASH_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Chip%c Read Flash TIMEOUT @[%08XH]",\
      pHardwareParam->cChipNum,u32CurAddrTemp);
  else if(eErrCode == ERR_SM_READ_FLASH_FAILURE)
    log(LOG_LEVEL_ERROR,"Get Chip%c Read Flash Failure R[%02XH] @[%08XH]",\
      pHardwareParam->cChipNum,pu8Buff[2],u32CurAddrTemp);
  else
    log(LOG_LEVEL_ERROR,"Chip%c Read Flash Unknown Error E[%X] @[08XH]",\
      pHardwareParam->cChipNum,eErrCode,u32CurAddrTemp);
  return false;
}

bool BL_STATUSMACHINE::WriteRamCode(void)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint8_t u8ReqLen;
  uint32_t u32ReadLen;
  uint32_t u32CurAddr = 0;  // Current Address of the RAM_CODE reading pointer
  uint32_t u32CurAddrTemp;  // Actual Addr in RAM
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    u8ReqLen = min(128,sizeof(RAM_CODE) - u32CurAddr);
    
    u32CurAddrTemp = u32CurAddr + RAM_CODE_ADDR_OFFSET;
    pu8Buff[0] = 6 + u8ReqLen;        // 1(len) + 1(cmd) + 4(addr) + data
    pu8Buff[1] = BL_MSG_WRITE_RAM_REQ;
    
    pu8Buff[2] = (uint8_t)(u32CurAddrTemp & 0xFF);    // Addr LSB First
    pu8Buff[3] = (uint8_t)((u32CurAddrTemp>>8) & 0xFF);
    pu8Buff[4] = (uint8_t)((u32CurAddrTemp>>16) & 0xFF);
    pu8Buff[5] = (uint8_t)((u32CurAddrTemp>>24) & 0xFF);
    
    memcpy(pu8Buff+6,RAM_CODE+u32CurAddr,u8ReqLen);
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < TIME_OUT_MS;i++)    //<_MARK(G)_ [TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == BL_MSG_WRITE_RAM_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          if(pu8Buff[2] == 0x00)
          {
            // Success
            u8Retry = 0;
            u32CurAddr += u8ReqLen;
            if(u32CurAddr >= sizeof(RAM_CODE))
            {
              log(LOG_LEVEL_MESSAGE,"Chip%c Write RamCode Success",pHardwareParam->cChipNum);
              return true;
            }
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_WRITE_RAM_FAILURE;
          }
          break;
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == TIME_OUT_MS)  //<_MARK(G)_ [TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_WRITE_RAM_TIMEOUT;
    }
  }
  if(eErrCode == ERR_SM_WRITE_RAM_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Chip%c Write RamCode TIMEOUT @[%08XH]",\
      pHardwareParam->cChipNum,u32CurAddrTemp);
  else if(eErrCode == ERR_SM_WRITE_RAM_FAILURE)
    log(LOG_LEVEL_ERROR,"Get Chip%c Write RamCode Failure R[%02XH] @[%08XH]",\
      pHardwareParam->cChipNum,pu8Buff[2],u32CurAddrTemp);
  else
    log(LOG_LEVEL_ERROR,"Chip%c Write RamCode Unknown Error E[%X] @[08XH]",\
      pHardwareParam->cChipNum,eErrCode,u32CurAddrTemp);
  return false;
}

bool BL_STATUSMACHINE::Run(uint32_t u32JumpAddr)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint32_t u32ReadLen;
  
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    pu8Buff[0] = 0x06;
    pu8Buff[1] = BL_MSG_RUN_REQ;
    pu8Buff[2] = (u32JumpAddr>>0)&0xFF;
    pu8Buff[3] = (u32JumpAddr>>8)&0xFF;
    pu8Buff[4] = (u32JumpAddr>>16)&0xFF;
    pu8Buff[5] = (u32JumpAddr>>24)&0xFF;
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < TIME_OUT_MS;i++)  //<_MARK(D)_ [TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == BL_MSG_RUN_RES) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          if(pu8Buff[2] == 0x00)
          {
            log(LOG_LEVEL_MESSAGE,"Chip%c Write SR Success");
            // 修改STM32波特率
            // ...............
            return true;
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_CHANGE_BAUDRATE_FAILURE;
            break;
          }
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == TIME_OUT_MS)    //<_MARK(D)_ [TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_CHANGE_BAUDRATE_TIMEOUT;
    }
  }
  if(eErrCode == ERR_SM_CHANGE_BAUDRATE_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Chip%c Change Baudrate TIMEOUT",\
      pHardwareParam->cChipNum);
  else if(eErrCode == ERR_SM_CHANGE_BAUDRATE_FAILURE)
    log(LOG_LEVEL_ERROR,"Get Chip%c Change Baudrate Failure R[%02XH]",\
      pHardwareParam->cChipNum,pu8Buff[2]);
  else
    log(LOG_LEVEL_ERROR,"Chip%c Change Baudrate Unknown Error E[%X]",\
      pHardwareParam->cChipNum,eErrCode);
  return false;
}

bool BL_STATUSMACHINE::ProgMAC(void)
{
  return true;
}

bool BL_STATUSMACHINE::ProgOTP(OTP_TYPE eOTPType,uint8_t *pData)
{
  uint8_t pu8Buff[136];
  uint8_t u8CheckSum;
  uint32_t i,j;
  uint8_t u8Retry = 0;
  uint32_t u32ReadLen;
  
  while(u8Retry < SystemInfo.u8RetryMax)
  {
    pu8Buff[0] = 0x14;
    pu8Buff[1] = 0x38;
    pu8Buff[2] = 0x05;
    pu8Buff[3] = 0x01;
    pu8Buff[4] = 0xFF;
    pu8Buff[5] = 0xFF;
    pu8Buff[6] = 0xFF;
    pu8Buff[7] = 0xED;
    pu8Buff[8] = 0xFF;
    pu8Buff[9] = 0xFF;
    pu8Buff[10] = 0xFF;
    pu8Buff[11] = 0xED;
    pu8Buff[12] = 0xFF;
    pu8Buff[13] = 0xFF;
    pu8Buff[14] = 0xFF;
    pu8Buff[15] = 0xED;
    pu8Buff[16] = 0xFF;
    pu8Buff[17] = 0xFF;
    pu8Buff[18] = 0xFF;
    pu8Buff[19] = 0xED;
    
    u8CheckSum = 0;
    for(j = 0;j < pu8Buff[0];j++)
      u8CheckSum ^= pu8Buff[j];
    pu8Buff[pu8Buff[0]] = u8CheckSum;
    
    USART2_RxBuffReset();
    USART2_SendBuff(pu8Buff,pu8Buff[0]+1);
    u32ReadLen = 0;
    //--------------------------------------------------------------------------
    for(i = 0;i < TIME_OUT_MS;i++)  //<_MARK(D)_ [TIME_OUT_MS]
    {
      u32ReadLen += USART2_ReadBuff(pu8Buff+u32ReadLen,sizeof(pu8Buff)-u32ReadLen);
      if(u32ReadLen-1 == pu8Buff[0])
      {
        u8CheckSum = 0;
        for(j = 0;j < pu8Buff[0];j++)
          u8CheckSum ^= pu8Buff[j];
        
        if((pu8Buff[1] == 0x39) && (u8CheckSum == pu8Buff[pu8Buff[0]]))
        {
          if(pu8Buff[2] == 0x00)
          {
            log(LOG_LEVEL_MESSAGE,"Chip%c Write SR Success");
            // 修改STM32波特率
            // ...............
            return true;
          }else
          {
            u8Retry++;
            eErrCode = ERR_SM_CHANGE_BAUDRATE_FAILURE;
            break;
          }
        }
      }
      OSTimeDly(OS_TICKS_PER_SEC/1000);
    }
    if(i == TIME_OUT_MS)    //<_MARK(D)_ [TIME_OUT_MS]
    {
      u8Retry++;
      eErrCode = ERR_SM_CHANGE_BAUDRATE_TIMEOUT;
    }
  }
  if(eErrCode == ERR_SM_CHANGE_BAUDRATE_TIMEOUT)
    log(LOG_LEVEL_ERROR,"Chip%c Change Baudrate TIMEOUT",\
      pHardwareParam->cChipNum);
  else if(eErrCode == ERR_SM_CHANGE_BAUDRATE_FAILURE)
    log(LOG_LEVEL_ERROR,"Get Chip%c Change Baudrate Failure R[%02XH]",\
      pHardwareParam->cChipNum,pu8Buff[2]);
  else
    log(LOG_LEVEL_ERROR,"Chip%c Change Baudrate Unknown Error E[%X]",\
      pHardwareParam->cChipNum,eErrCode);
  return false;
}

bool BL_STATUSMACHINE::Error(void)
{
  uint8_t u8Err;
  
  GPIO_ResetBits(pHardwareParam->GPIOx_PWR,pHardwareParam->GPIO_PWR_Pin);
  UI_LED_Switch(pHardwareParam->eUILed,UI_LED_STATUS_ON,UI_LED_STATUS_OFF);
  OSFlagPost(FlagChipxStatus,pHardwareParam->FLAG_CHIPx_RUNNING_MASK,OS_FLAG_CLR,&u8Err);
  log(LOG_LEVEL_ERROR,"Chip%c Error Occurred",pHardwareParam->cChipNum);
  return true;
}

bool BL_STATUSMACHINE::Success(void)
{
  uint8_t u8Err;
  
  GPIO_ResetBits(pHardwareParam->GPIOx_PWR,pHardwareParam->GPIO_PWR_Pin);
  UI_LED_Switch(pHardwareParam->eUILed,UI_LED_STATUS_OFF,UI_LED_STATUS_ON);
  OSFlagPost(FlagChipxStatus,pHardwareParam->FLAG_CHIPx_RUNNING_MASK,OS_FLAG_CLR,&u8Err);
  log(LOG_LEVEL_MESSAGE,"Chip%c Success",pHardwareParam->cChipNum);
  return true;
}
