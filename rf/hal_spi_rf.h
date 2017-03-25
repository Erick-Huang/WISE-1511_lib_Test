#ifndef __HAL_SPI_RF_H__
#define __HAL_SPI_RF_H__


#include "hal_def.h"
/******************************************************************************
 * DEFINE THE TRANSCEIVER TO USE
 */
//#define USE_CC1101                     /* use the CC110x transciever commands */
#define USE_CC112X                     /* use the CC112x transciever commands */
#define RF_XTAL 40000                  /* default is 26000 for CC1101 */
                                       /* 32000 for CC1120 */
									   /* 40000 for CC1125 */


/******************************************************************************
 * CONSTANTS
 */
#define     RADIO_BURST_ACCESS      0x40
#define     RADIO_SINGLE_ACCESS     0x00
#define     RADIO_READ_ACCESS       0x80
#define     RADIO_WRITE_ACCESS      0x00

/* Bit fields in the chip status byte */
#define STATUS_CHIP_RDYn_BM              0x80
#define STATUS_STATE_BM                  0x70
#define STATUS_FIFO_BYTES_AVAILABLE_BM   0x0F

/******************************************************************************
 * TYPEDEFS
 */

typedef struct
{
  uint16  addr;
  uint8   data;
}registerSetting_t;

typedef uint8 rfStatus_t;

/******************************************************************************
 * PROTOTYPES
 */

void trxRfSpiInterfaceInit(uint8 prescalerValue);
rfStatus_t trx8BitRegAccess(uint8 accessType, uint8 addrByte, uint8 *pData, uint16 len);
rfStatus_t trxSpiCmdStrobe(uint8 cmd);

/* CC112X specific prototype function */
rfStatus_t trx16BitRegAccess(uint8 accessType, uint8 extAddr, uint8 regAddr, uint8 *pData, uint8 len);

#endif
