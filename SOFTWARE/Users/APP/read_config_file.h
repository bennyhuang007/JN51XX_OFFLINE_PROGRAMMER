#ifndef _READ_CONFIG_FILE_H_
#define _READ_CONFIG_FILE_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "stdio.h"
#include "stdint.h"
#include "common.h"
  
  typedef void (*pFun)(const char *,uint32_t);
  
  typedef enum
  {
    INI_KEYWORD_TYPE_ROOT = 0,
    INI_KEYWORD_TYPE_SECTION,
    INI_KEYWORD_TYPE_KEY,
  }INI_KEYWORD_TYPE;
  
  typedef struct _STR_CFG_KEYWORD_STR
  {
    INI_KEYWORD_TYPE eType;
    struct _STR_CFG_KEYWORD_STR *pUpperLevel;
    struct _STR_CFG_KEYWORD_STR *pLowerLevel;
    struct _STR_CFG_KEYWORD_STR *pSameLevelNext;
    char *pStrKeyWord;
    pFun FunCallback;
  }CFG_KEYWORD_STR;
  
  //------------------------------------------------------------------------------
  typedef struct
  {
    CHIP_ID ChipID;
    const char *cChipIDStr;
  }CHIP_ID_STR;
  
  const static CHIP_ID_STR ChipIDStr[] = 
  {
    {.ChipID.u32ChipID = CHIP_ID_JN5148,.cChipIDStr = "JN5148"},
    {.ChipID.u32ChipID = CHIP_ID_JN5142,.cChipIDStr = "JN5142"},
    {.ChipID.u32ChipID = CHIP_ID_JN5139,.cChipIDStr = "JN5139"},
    {.ChipID.u32ChipID = CHIP_ID_JN516X,.cChipIDStr = "JN5161"},
    {.ChipID.u32ChipID = CHIP_ID_JN516X,.cChipIDStr = "JN5164"},
    {.ChipID.u32ChipID = CHIP_ID_JN516X,.cChipIDStr = "JN5168"},
  };
  //------------------------------------------------------------------------------
  typedef struct
  {
    FLASH_TYPE FlashType;
    const char *cFlashTypeStr;
  }FLASH_TYPE_STR;
  
  const static FLASH_TYPE_STR FlashTypeStr[] =
  {
    {FLASH_TYPE_INTERNAL,"Internal"},
    {FLASH_TYPE_EXTERNAL,"External"},
  };
  //------------------------------------------------------------------------------
  typedef struct
  {
    MAC_MODE MacMode;
    const char *cMacModeStr;
  }MAC_MODE_STR;
  
  const static MAC_MODE_STR MacModeStr[] =
  {
    {MAC_MODE_NOCHANGE,"NoChange"},
    {MAC_MODE_INCREASE,"Increase"},
    {MAC_MODE_DECREASE,"Decrease"},
    {MAC_MODE_FROMFILE,"FromFile"},
  };
  //------------------------------------------------------------------------------
  typedef struct
  {
    MAC_ERROR_MODE MacErrorMode;
    const char *cMacErrorModeStr;
  }MAC_ERROR_MODE_STR;
  
  const static MAC_ERROR_MODE_STR MacErrorModeStr[] =
  {
    {MAC_ERROR_MODE_CONTINUE,"Continue"},
    {MAC_ERROR_MODE_ABANDON,"Abandon"},
  };
  //------------------------------------------------------------------------------
  typedef struct
  {
    LOG_LEVEL LogLevel;
    const char *cLogLevelStr;
  }LOG_LEVEL_STR;
  
  const static LOG_LEVEL_STR LogLevelStr[] =
  {
    {LOG_LEVEL_NONE,"None"},
    {LOG_LEVEL_ERROR,"Error"},
    {LOG_LEVEL_WARNNING,"Warnning"},
    {LOG_LEVEL_MESSAGE,"Message"},
  };
  //------------------------------------------------------------------------------
  typedef struct
  {
    IO_MODE IOMode;
    const char *cIOModeStr;
  }LOG_IOMODE_STR;
  
  const static LOG_IOMODE_STR IOModeStr[] = 
  {
    {IO_MODE_OD,"OD"},
    {IO_MODE_PP,"PP"},
  };
  //------------------------------------------------------------------------------
  // !!!Caution!!!
  // MARK(A)
#define CFG_FILEINFO_VERSION_MASK               0x00000001ul
#define CFG_DEVICEINFO_DEVICE_MASK              0x00000002ul
#define CFG_DEVICEINFO_FLASH_MASK               0x00000004ul
#define CFG_MACINFO_MODE_MASK                   0x00000008ul
#define CFG_MACINFO_ERRORMODE_MASK              0x00000010ul
#define CFG_MACINFO_STARTADDR_MASK              0x00000020ul
#define CFG_MACINFO_FILENAME_MASK               0x00000040ul
#define CFG_OTPINFO_OTPMASK_MASK                0x00000080ul
#define CFG_OTPINFO_AES_MASK                    0x00000100ul
#define CFG_OTPINFO_USERDATA0_MASK              0x00000200ul
#define CFG_OTPINFO_USERDATA1_MASK              0x00000400ul
#define CFG_OTPINFO_USERDATA2_MASK              0x00000800ul
#define CFG_LOGINFO_LOGLEVEL_MASK               0x00001000ul
#define CFG_LOGINFO_FILENAME_MASK               0x00002000ul
#define CFG_LOGINFO_FILESIZE_MASK               0x00004000ul
#define CFG_OTHERINFO_BAUDRATE_MASK             0x00008000ul
#define CFG_OTHERINFO_CHANNELMASK_MASK          0x00010000ul
#define CFG_OTHERINFO_VERIFICTION_MASK          0x00020000ul
#define CFG_OTHERINFO_POWEROUT_MASK             0x00040000ul
#define CFG_OTHERINFO_CLOCKOUT_MASK             0x00080000ul
#define CFG_OTHERINFO_IOMODE_MASK               0x00100000ul
#define CFG_FULL_MASK                           0x001FFFFFul
  // !!!Caution!!!
  extern uint32_t u32CfgMask;
  extern void config_file_statemachine(const char *pStr,uint32_t u32StrLen);
  
#ifdef __cplusplus
}
#endif

#endif
