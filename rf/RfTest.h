#ifndef __RF_TEST_H__
#define __RF_TEST_H__

#include "cc112x.h"
#include "cc112x_def.h"

#define FIXED_LENGTH        FALSE
#define PREAMBLE_LENGTH     8
#define PKTLEN              125

#define RX_FIFO_ERROR       0x11
//#define ENABLE_TWO_WAY_LINK TRUE       // a reverse packet is expected
#define ENABLE_TWO_WAY_LINK FALSE       // a reverse packet is expected

#define BITRATE_SMARTRF

#define SYSTEM_ID           0          // 0 and 255


#ifdef BITRATE_SMARTRF
static const registerSetting_t preferredSettings[]= 
{
  {CC1125_IOCFG3,            0xB0},
  {CC1125_IOCFG2,            0x06},
  {CC1125_IOCFG1,            0xB0},
  {CC1125_IOCFG0,            0x40},
  {CC1125_SYNC_CFG1,         0x0B},
  {CC1125_DCFILT_CFG,        0x1C},
  {CC1125_IQIC,              0xC6},
  {CC1125_CHAN_BW,           0x19},
  {CC1125_MDMCFG0,           0x05},
  {CC1125_AGC_REF,           0x20},
  {CC1125_AGC_CS_THR,        0x19},
  {CC1125_AGC_CFG1,          0xA9},
  {CC1125_AGC_CFG0,          0xCF},
  {CC1125_FIFO_CFG,          0x00},
  {CC1125_FS_CFG,            0x12},
  {CC1125_PKT_CFG0,          0x20},
  {CC1125_PA_CFG0,           0x7E},
  {CC1125_PKT_LEN,           0xFF},
  {CC1125_IF_MIX_CFG,        0x00},
  {CC1125_FREQOFF_CFG,       0x22},
  {CC1125_FREQ2,             0x70},
  {CC1125_FREQ1,             0xD6},
  {CC1125_FREQ0,             0x8F},
  {CC1125_FS_DIG1,           0x00},
  {CC1125_FS_DIG0,           0x5F},
  {CC1125_FS_CAL0,           0x0E},
  {CC1125_FS_DIVTWO,         0x03},
  {CC1125_FS_DSM0,           0x33},
  {CC1125_FS_DVC0,           0x17},
  {CC1125_FS_PFD,            0x50},
  {CC1125_FS_PRE,            0x6E},
  {CC1125_FS_REG_DIV_CML,    0x14},
  {CC1125_FS_SPARE,          0xAC},
  {CC1125_XOSC5,             0x0E},
  {CC1125_XOSC3,             0xC7},
  {CC1125_XOSC1,             0x07},
};

#endif
/*#ifdef BITRATE_1200
static const registerSetting_t preferredSettings[]= 
{
  {CC112X_IOCFG3,            0xB0},
  {CC112X_IOCFG2,            0x06},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x40},
  {CC112X_SYNC_CFG1,         0x0B},
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_PREAMBLE_CFG1,     0x18},
  {CC112X_IQIC,              0xC6},
  {CC112X_CHAN_BW,           0x08},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_AGC_REF,           0x20},
  {CC112X_AGC_CS_THR,        0x19},
  {CC112X_AGC_CFG1,          0xA9},
  {CC112X_AGC_CFG0,          0xCF},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x12},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_PA_CFG2,           0x5D},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x22},
  {CC112X_FREQ2,             0x6C},
  {CC112X_FREQ1,             0x80},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL1,           0x40},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_FS_VCO0,           0xB4},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC1,             0x03},
};
#endif
#ifdef BITRATE_4800
static const registerSetting_t preferredSettings[]= 
{
  {CC112X_IOCFG3,            0xB0},
  {CC112X_IOCFG2,            0x06},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x40},
  {CC112X_SYNC_CFG1,         0x0B},
  {CC112X_DEVIATION_M,       0x26},
  {CC112X_MODCFG_DEV_E,      0x05},
  {CC112X_DCFILT_CFG,        0x13},
  {CC112X_PREAMBLE_CFG1,     0x18},
  {CC112X_PREAMBLE_CFG0,     0x33},
  {CC112X_IQIC,              0x00},
  {CC112X_CHAN_BW,           0x03},
  {CC112X_MDMCFG0,           0x04},
  {CC112X_SYMBOL_RATE2,      0x63},
  {CC112X_AGC_REF,           0x30},
  {CC112X_AGC_CS_THR,        0xEC},
  {CC112X_AGC_CFG3,          0xD1},
  {CC112X_AGC_CFG2,          0x3F},
  {CC112X_AGC_CFG1,          0x32},
  {CC112X_AGC_CFG0,          0x9F},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x12},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_PA_CFG2,           0x5D},
  {CC112X_PA_CFG0,           0x7D},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x00},
  {CC112X_TOC_CFG,           0x0A},
  {CC112X_FREQ2,             0x6C},
  {CC112X_FREQ1,             0x80},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL1,           0x40},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_FS_VCO0,           0xB4},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC1,             0x03},
  };
#endif
#ifdef BITRATE_38400
static const registerSetting_t preferredSettings[]= 
{
  {CC112X_IOCFG3,            0xB0},
  {CC112X_IOCFG2,            0x06},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x40},
  {CC112X_SYNC_CFG1,         0x08},
  {CC112X_DEVIATION_M,       0x48},
  {CC112X_MODCFG_DEV_E,      0x0D},
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_IQIC,              0x00},
  {CC112X_CHAN_BW,           0x02},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_SYMBOL_RATE2,      0x93},
  {CC112X_AGC_CS_THR,        0x19},
  {CC112X_AGC_CFG1,          0xA9},
  {CC112X_AGC_CFG0,          0xCF},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x12},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_PA_CFG0,           0x7B},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x22},
  {CC112X_FREQ2,             0x6C},
  {CC112X_FREQ1,             0x80},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL1,           0x40},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_FS_VCO0,           0xB4},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC1,             0x03},
};
#endif*/

int RFInit(void);
int RFTest_Menu(int index, char *string, int len, void *printer);

void registerConfig(registerSetting_t *preferred, int number);
void tx_ber_single(int pktlen, int freq, int chbw, int channel, int pktnum);
void rx_ber_single(int pktlen, int freq, int chbw, int channel, int pktnum);

#endif //__RF_TEST_H__