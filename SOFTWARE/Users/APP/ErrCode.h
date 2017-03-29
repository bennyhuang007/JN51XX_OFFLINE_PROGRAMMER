#ifndef _ERR_CODE_H_
#define _ERR_CODE_H_

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef enum
  {
    ERR_NORMAL = 0x0,
    ERR_PARAM = 0x1,
    /* FIFO Err Code */
    ERR_FIFO_NOENOUGHSPACE = 0x00010000,
    ERR_FIFO_NOENOUGHDATA,
    /* Status Machine Code*/
    ERR_SM_CHIP_ID_WRONG = 0x00020000,    // chip id is not match or get a wrong respond
    ERR_SM_CHIP_ID_TIMEOUT,               // no respond(timeout) while getting chip id
    ERR_SM_TARGET_VOLTAGE_ERROR,          // target voltage is too low or too high
    ERR_SM_WRITE_SR_FAILURE,              // failure while writting status reg
    ERR_SM_WRITE_SR_TIMEOUT,              // no respond(timeout) while writing status reg
    ERR_SM_EREASE_FAILURE,                // failure while earsing chip
    ERR_SM_EREASE_TIMEOUT,                // no respond(timeout) while earsing chip
    ERR_SM_CHANGE_BAUDRATE_FAILURE,       // failure while changing baudrate
    ERR_SM_CHANGE_BAUDRATE_TIMEOUT,       // no respond(timeout) while changing baudrate
    ERR_SM_ASK_DATA_FAILURE,              // error while asking for firmware data
    ERR_SM_PROG_FLASH_FAILURE,            // failure while programming flash
    ERR_SM_PROG_FLASH_TIMEOUT,            // no respond(timeout) while programming flash
    ERR_SM_READ_FLASH_FAILURE,            // failure while reading flash
    ERR_SM_READ_FLASH_TIMEOUT,            // no respond(timeout) while reading flash
    ERR_SM_VERIFICATION_FAILURE,          // failure(CRC32) while verification
    ERR_SM_WRITE_RAM_TIMEOUT,             // no respond(timeout) while writing ram
    ERR_SM_WRITE_RAM_FAILURE,             // failure while writing ram
  }ERR_CODE;
  
#ifdef __cplusplus
}
#endif

#endif