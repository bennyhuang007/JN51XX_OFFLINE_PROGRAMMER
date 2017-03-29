#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "stdbool.h"
#include "stdint.h"
#include "log.h"
#include "ucos_ii.h"
  
  // SC for Self Check
#define SC_VCC_MASK             0x01
#define SC_FILESYSTEM_MASK      0x02
#define SC_CONFIGFILE_MASK      0x04
#define SC_TARGETFILE_MASK      0x08
#define SC_ALL_MASK             (SC_VCC_MASK|SC_FILESYSTEM_MASK|SC_TARGETFILE_MASK|SC_CONFIGFILE_MASK)
  
#define min(a,b)        ((a)>(b)?(b):(a))
#define max(a,b)        ((a)>(b)?(a):(b))
  
  typedef union
  {
    uint64_t u64MacAddr;
    uint32_t u32MacAddr[2];
  }MAC_ADDR;
  
  typedef union
  {
    uint32_t u32ChipID;
    uint8_t u8ChipID[4];
  }CHIP_ID;
  
  // !!!Caution!!!
  // if you modify the ID list below,please modify the "ChipIDStr" in "read_config_file.h"
#define CHIP_ID_JN5148  0x10404686
#define CHIP_ID_JN5142  0x00005686
#define CHIP_ID_JN5139  0x10002000
#define CHIP_ID_JN516X  0x10008686
#define CHIP_ID_UNKNOWN 0xFFFFFFFF
  // !!!Caution!!!
  
  // !!!Caution!!!
  // if you modify the Flash Type list below,please modify the "FlashTypeStr" in "read_config_file.h"
  typedef enum
  {
    FLASH_TYPE_INTERNAL = 0,
    FLASH_TYPE_EXTERNAL,
  }FLASH_TYPE;
  // !!!Caution!!!
  
  // !!!Caution!!!
  // if you modify the Mac Mode Type list below,please modify the "MacModeStr" in "read_config_file.h"
  typedef enum
  {
    MAC_MODE_NOCHANGE = 0,
    MAC_MODE_INCREASE,
    MAC_MODE_DECREASE,
    MAC_MODE_FROMFILE,
  }MAC_MODE;
  // !!!Caution!!!
  
  // !!!Caution!!!
  // if you modify the Mac Error Mode Type list below,please modify the "MacErrorModeStr" in "read_config_file.h"
  typedef enum
  {
    MAC_ERROR_MODE_CONTINUE = 0,
    MAC_ERROR_MODE_ABANDON,
  }MAC_ERROR_MODE;
  // !!!Caution!!!
  
  // !!!Caution!!!
  // if you modify the Mac Error Mode Type list below,please modify the "IOModeStr" in "read_config_file.h"
  typedef enum
  {
    IO_MODE_OD = 0,
    IO_MODE_PP,
  }IO_MODE;
  // !!!Caution!!!
  
  // !!!Caution!!!
  // if you modify the struct below,please modify
  // [SystemInfoDef] in common.c
  // CALLBACK functions in read_config_file.c
  // MARK(A) in read_config_file.h
  typedef struct
  {
    //Device
    CHIP_ID ChipID;
    FLASH_TYPE FlashType;                 // 0-Internal Flash;1-External Flash
    // Mac
    MAC_MODE Mac_Mode;                    // 0-NoChange;1-Increase;2-Decrease;3-FromFile
    MAC_ERROR_MODE Mac_ErrorMode;          // 0-Continue;1-Abandon
    MAC_ADDR Mac_StartAddr;
    char Mac_FileName[16];
    // OTP
    bool bPrgAES;
    bool bPrgUserData0;
    bool bPrgUserData1;
    bool bPrgUserData2;
    uint32_t AES[4];
    uint32_t UserData[3][4];
    // Log
    LOG_LEVEL Log_Level;
    uint32_t Log_FileSizeMax;              // Log file max size
    char Log_FileName[16];
    // Other
    bool Verification;                    // 0-No,1-Yes
    bool PowerOut;
    bool ClockOut;
    uint8_t ChannelMask;                  // bit0-Channel1..bit3-Channel4
    uint32_t BaudRate;
    IO_MODE IOMode;
    // system information
    uint32_t u32TargetSize;               // size of taget bin file
    uint32_t u32TargetCRC;                // CRC32 for Target file(Excluding first 4 bytes)
    uint8_t u8RetryMax;
  }SYSTEM_INFO;
  // !!!Caution!!!
  
  extern OS_FLAG_GRP *FlagSelfCheck;
  extern SYSTEM_INFO SystemInfo;
  extern const SYSTEM_INFO SystemInfoDef;
  extern uint32_t CalculateBlockCRC32(uint32_t u32CRCInit,const uint8_t *u8Buff,uint32_t u32BuffCnt);
  
#ifdef __cplusplus
}
#endif

#endif
