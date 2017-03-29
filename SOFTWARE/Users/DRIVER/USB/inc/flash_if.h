/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : flash_if.h
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Header for flash_if.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_MAL_H
#define __FLASH_IF_MAL_H

#ifdef __cplusplus
extern "C" {
#endif
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define FLASH_START_ADDR        0x08018000      // Flash start address
#define FLASH_SIZE              0x00068000      //
#define FLASH_PAGE_SIZE         0x800           // 2K per page
#define FLASH_WAIT_TIMEOUT      100000 
/* Exported functions ------------------------------------------------------- */

uint16_t FLASH_Init(void);
uint16_t FLASH_Write(uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length);
uint16_t FLASH_Read(uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length);

#ifdef __cplusplus
}
#endif
  
#endif /* __FLASH_IF_MAL_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
