/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/
  
#include "diskio.h"		/* FatFs lower layer API */
#include "mass_mal.h"
/* Definitions of physical drive number for each media */
#define INTER_FLASH		0

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
                         BYTE pdrv				/* Physical drive nmuber (0..) */
                           )
{
  if(pdrv == INTER_FLASH)
    //FLASH_Init();
    return 0;
  else
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
                     BYTE pdrv		/* Physical drive nmuber (0..) */
                       )
{
  if(pdrv == INTER_FLASH)
    return 0;
  else
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
                   BYTE pdrv,		/* Physical drive nmuber (0..) */
                   BYTE *buff,		/* Data buffer to store read data */
                   DWORD sector,	/* Sector address (LBA) */
                   UINT count		/* Number of sectors to read (1..128) */
                     )
{
  switch (pdrv) {
  case INTER_FLASH:
    if(FLASH_Read(sector * FLASH_PAGE_SIZE,(uint32_t*)buff,count * FLASH_PAGE_SIZE) == MAL_OK)
      return RES_OK;
    else
      return RES_ERROR;
  }
  return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
                    BYTE pdrv,          /* Physical drive nmuber (0..) */
                    const BYTE *buff,   /* Data to be written */
                    DWORD sector,       /* Sector address (LBA) */
                    UINT count          /* Number of sectors to write (1..128) */
                      )
{
  switch (pdrv) {
  case INTER_FLASH:
    if(FLASH_Write(sector * FLASH_PAGE_SIZE,(uint32_t*)buff,count * FLASH_PAGE_SIZE) == MAL_OK)
      return RES_OK;
    else
      return RES_ERROR;
  }
  return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
                    BYTE pdrv,		/* Physical drive nmuber (0..) */
                    BYTE cmd,		/* Control code */
                    void *buff		/* Buffer to send/receive control data */
                      )
{
  DRESULT res = RES_PARERR;
  if(pdrv == 0)
  {
    switch(cmd)
    {
    case CTRL_SYNC:
      res = RES_PARERR;
      break;
    case GET_SECTOR_COUNT:
      *(DWORD*)buff = FLASH_SIZE / FLASH_PAGE_SIZE;
      res = RES_OK;
      break;
    case GET_SECTOR_SIZE:
      *(WORD*)buff = FLASH_PAGE_SIZE;
      res = RES_OK;
      break;
    case GET_BLOCK_SIZE:
      *(DWORD*)buff = 1;
      res = RES_OK;
      break;
    case CTRL_ERASE_SECTOR:
      res = RES_PARERR;
      break;
    }
  }else
    res = RES_PARERR;
  return res;
}

DWORD get_fattime (void)
{
  return ((2014UL-1980) << 25) /* Year = 2010 */
    | (8UL << 21) /* Month = 11 */
    | (22UL << 16) /* Day = 2 */
    | (0U << 11) /* Hour = 15 */
    | (0U << 5) /* Min = 0 */
    | (0U << 1); /* Sec = 0 */
}
#endif
