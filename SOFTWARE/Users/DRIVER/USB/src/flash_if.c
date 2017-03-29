/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : flash_if.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : specific media access Layer for internal flash
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "mass_mal.h"
#include "ucos_ii.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : FLASH_If_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t FLASH_Init(void)
{
  FLASH_Unlock();
  return MAL_OK;
}

uint16_t FLASH_Write(uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{
  u16 i;
  OSSchedLock();
  for(i=0; i<Transfer_Length;i+=FLASH_PAGE_SIZE)
    FLASH_ErasePage(FLASH_START_ADDR + Memory_Offset + i);
  for(i=0;i<Transfer_Length;i+=4)
    FLASH_ProgramWord(FLASH_START_ADDR + Memory_Offset + i , Writebuff[i>>2]);
  OSSchedUnlock();
  return MAL_OK;
}

uint16_t FLASH_Read(uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{
  u16 i;
  OSSchedLock();
  for(i=0;i<Transfer_Length;i+=4)
    Readbuff[i>>2] = *(vu32*)(FLASH_START_ADDR + Memory_Offset + i);
  OSSchedUnlock();
  return MAL_OK; 
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
