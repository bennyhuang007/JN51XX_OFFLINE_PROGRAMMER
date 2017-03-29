#ifndef _BL_STATEMACHINE_H_
#define _BL_STATEMACHINE_H_

#include "stdbool.h"
#include "stdint.h"
#include "ErrCode.h"
#include "stm32f10x_gpio.h"
#include "ucos_ii.h"

typedef enum
{
  BL_STATE_IDAL = 0,
  BL_STATE_POWER_OUT,
  BL_STATE_POWER_CHECK,
  BL_STATE_INIT,
  BL_STATE_GET_ID,
  BL_STATE_SET_STATUS_REG,
  BL_STATE_ERASE,
  BL_STATE_CHANGE_BAUDRATE,
  BL_STATE_PROG_FLASH,
  BL_STATE_VERIFICATION,
  BL_STATE_WRITE_RAMCODE,    // if MAC/AES/UserData need
  BL_STATE_CHECK_RAMCODE,
  BL_STATE_RUN_RAMCODE,
  BL_STATW_PROG_MAC,
  BL_STATW_PROG_AES,
  BL_STATW_PROG_USERDATA0,
  BL_STATW_PROG_USERDATA1,
  BL_STATW_PROG_USERDATA2,
  BL_STATE_ERROR,
  BL_STATE_SUCCESS,
}BL_STATE;

typedef enum
{
  BL_MSG_FLASH_ERASE_REQ = 0x07,        // All Sector
  BL_MSG_FLASH_ERASE_RES = 0x08,
  BL_MSG_FLASH_PROGRAM_REQ = 0x09,
  BL_MSG_FLASH_PROGRAM_RES = 0x0A,
  BL_MSG_FLASH_READ_REQ = 0x0B,
  BL_MSG_FLASH_READ_RES = 0x0C,
  BL_MSG_SECTOR_ERASE_REQ = 0x0D,
  BL_MSG_SECTOR_ERASE_RES = 0x0E,
  BL_MSG_WRITE_SR_REQ = 0x0F,
  BL_MSG_WRITE_SR_RES = 0x10,
  BL_MSG_WRITE_RAM_REQ = 0x1D,
  BL_MSG_WRITE_RAM_RES = 0x1E,
  BL_MSG_READ_RAM_REQ = 0x1F,
  BL_MSG_READ_RAM_RES = 0x20,
  BL_MSG_RUN_REQ = 0x21,
  BL_MSG_RUN_RES = 0x22,
  BL_MSG_READ_FLASH_ID_REQ = 0x25,
  BL_MSG_READ_FLASH_ID_RES = 0x26,
  BL_MSG_CHANGE_BAUDRATE_REQ = 0x27,
  BL_MSG_CHANGE_BAUDRATE_RES = 0x28,
  BL_MSG_SELECT_FLASH_TYPE_REQ = 0x2C,
  BL_MSG_SELECT_FLASH_TYPE_RES = 0x2D,
  BL_MSG_GET_CHIP_ID_REQ = 0x32,
  BL_MSG_GET_CHIP_ID_RES = 0x33,
}BL_MSG;

typedef struct
{
  uint8_t *pMemData;
  uint32_t u32Addr;
  uint32_t u32ReqLen;
  uint8_t DataExist;
}MEM_MSG;

typedef struct
{
  uint64_t MacData;
  uint8_t DataExist;
}MAC_MSG;

typedef struct 
{
  GPIO_TypeDef *GPIOx_RST;
  GPIO_TypeDef *GPIOx_SIG;
  GPIO_TypeDef *GPIOx_PWR;
  uint16_t GPIO_RST_Pin;
  uint16_t GPIO_SIG_Pin;
  uint16_t GPIO_PWR_Pin;
  
  USART_TypeDef *USARTx;
  
  OS_FLAGS FLAG_CHIPx_START_MASK;
  OS_FLAGS FLAG_CHIPx_RUNNING_MASK;
  
  uint8_t eUILed;
  
  char cChipNum;
  
  uint32_t *pu32ADCValue;
  
}HARDWARE_PARAM;

typedef enum
{
  OTP_AES,
  OTP_USERDATA0,
  OTP_USERDATA1,
  OTP_USERDATA2,
}OTP_TYPE;

class BL_STATUSMACHINE
{
private:
  HARDWARE_PARAM *pHardwareParam;       // Hardware Param
  MEM_MSG MemMsg;                       // message for asking firmware data,send to [QMemMsg]
  MAC_MSG MacMsg;                       // message for asking mac addr,send to [QMacMsg]
  ERR_CODE eErrCode;
  BL_STATE eState;
  
private:
  bool Idal(void);
  bool PowerOut(void);
  bool PowerCheck(void);
  bool Init(void);
  bool GetChipID(void);
  bool SetStatusReg(void);
  bool Erase(void);
  bool ChangeBaudRate(void);
  bool ProgramFlash(void);
  bool Verification(void);
  bool WriteRamCode(void);
  bool CheckRamCode(void);
  bool RunRamCode(void);
  bool ProgMAC(void);
  bool ProgOTP(OTP_TYPE eOTPType,uint8_t *pData);   // include AES UserData
  bool Run(uint32_t u32JumpAddr);
  bool Error(void);
  bool Success(void);
public:
  BL_STATUSMACHINE(HARDWARE_PARAM *pInitParam);
  ~BL_STATUSMACHINE();
  
  void StatusMachine(void);
};

#endif