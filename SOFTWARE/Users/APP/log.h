#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif
  
#include "stdint.h"
  
#define LOG_HEAD_SIZE   10      // Len(8byte) + \r\n
  // !!!Caution!!!
  // if you modify the Log Level list below,please modify the "LogLevelStr" in "read_config_file.h"
  typedef enum
  {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNNING,
    LOG_LEVEL_MESSAGE,
  }LOG_LEVEL;
  // !!!Caution!!!
  
  const static char LOG_LEVEL_MARK_STR[][2] = 
  {
    "-",
    "E",
    "W",
    "M"
  };
  
  extern void log(LOG_LEVEL eLogLevel,const char *pStr,...);
  extern uint32_t log_getdata(uint8_t *pData,uint32_t iLenght);
  
#ifdef __cplusplus
}
#endif

#endif