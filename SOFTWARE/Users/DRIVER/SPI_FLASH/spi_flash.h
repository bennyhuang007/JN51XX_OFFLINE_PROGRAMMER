#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include "spi_flash.h"
#include "stm32f10x_spi.h"

#define SPI_FLASH_CMD_WRITE_ENABLE              (0x06)
#define SPI_FLASH_CMD_WRITE_DISABLE             (0x04)
#define SPI_FLASH_CMD_READ_STATUS_REG           (0x05)
#define SPI_FLASH_CMD_WRITE_STATUS_REG          (0x01)
#define SPI_FLASH_CMD_READ_DATA                 (0x03)
#define SPI_FLASH_CMD_FAST_READ                 (0x0B)
#define SPI_FLASH_CMD_FAST_READ_DUAL_OUTPUT     (0x3B)
#define SPI_FLASH_CMD_PAGE_PROGRAM              (0x02)
#define SPI_FLASH_CMD_BLOCK_ERASE               (0xD8)
#define SPI_FLASH_CMD_SECTOR_ERASE              (0x20)
#define SPI_FLASH_CMD_CHIP_ERASE                (0xC7)
#define SPI_FLASH_CMD_POWER_DOWN                (0xB9)
#define SPI_FLASH_CMD_DEVICE_ID                 (0xAB)
#define SPI_FLASH_CMD_RELEASE_POWER_DOWN        (0xAB)
#define SPI_FLASH_CMD_MANUFACTURER              (0x90)
#define SPI_FLASH_CMD_JEDEC_ID                  (0x9F)

void SPI_FLASH_Config(void);
uint8_t SPI_FLASH_TXRXByte(uint16_t TXByte,uint8_t RXByte);

#endif