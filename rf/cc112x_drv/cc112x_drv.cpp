/******************************************************************************
 *  Filename: cc112x_drv.c
 *
 *  Description: Radio driver abstraction layer, this uses the same concept
 *               as found in Contiki OS.
 *
 *  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************/

/******************************************************************************
 * INCLUDES
 */
#include "mbed.h"
#include "cc112x_def.h"
#include "hal_spi_rf.h"
/******************************************************************************
 * DEFINES
 */
#define RF_XTAL_FREQ           32000       /* XTAL frequency, given in 1kHz steps */
#define RF_LO_DIVIDER          4             /* there is a hardware LO divider CC112x */

/******************************************************************************
 * GLOBALS -  used by the driver
 */
unsigned char rf_end_packet = 0;

/******************************************************************************
 * Configuration extracted from SmartRF Studio version 7 Release 2.0.0
 */
// Address config = No address check
// Performance mode = High Performance
// Packet bit length = 0
// Symbol rate = 1.2
// Whitening = false
// Carrier frequency = 902.750000
// TX power = 15
// Manchester enable = false
// Packet length mode = Fixed
// Packet length = 20
// RX filter BW = 25.000000
// Deviation = 3.997803
// Device address = 0
// Bit rate = 1.2
// Modulation format = 2-FSK
// PA ramping = true
// Append packetinfo = false

const registerSetting_t preferredSettings_1200bps[]=
{
		{IOCFG3,            0xB0},
		{IOCFG2,            0x06},
		{IOCFG1,            0xB0},
		{IOCFG0,            0x06},
		{SYNC3,             0xD3},
		{SYNC2,             0x91},
		{SYNC1,             0xD3},
		{SYNC0,             0x91},
		{SYNC_CFG1,         0x0B},
		{DCFILT_CFG,        0x1C},
		{PREAMBLE_CFG1,     0x18},
		{IQIC,              0xC6},
		{CHAN_BW,           0x08},
		{MDMCFG0,           0x05},
		{SYMBOL_RATE2,      0x43},
		{SYMBOL_RATE1,      0xA9},
		{SYMBOL_RATE0,      0x2A},
		{AGC_REF,           0x20},
		{AGC_CS_THR,        0x19},
		{AGC_CFG1,          0xA9},
		{AGC_CFG0,          0xCF},
		{FIFO_CFG,          0x00},
		{SETTLING_CFG,      0x0B},
		{FS_CFG,            0x12},
		{PKT_CFG2,          0x04},
		{PKT_CFG1,          0x04},
		{PKT_CFG0,          0x00},
		{PA_CFG2,           0x7F},
		{PA_CFG1,           0x56},
		{PA_CFG0,           0x7C},
		{PKT_LEN,           0x14},
		{IF_MIX_CFG,        0x00},
		{FREQOFF_CFG,       0x22},
		{FREQ2,             0x70},
		{FREQ1,             0xD8},
		{FS_DIG1,           0x00},
		{FS_DIG0,           0x5F},
		{FS_CAL1,           0x40},
		{FS_CAL0,           0x0E},
		{FS_DIVTWO,         0x03},
		{FS_DSM0,           0x33},
		{FS_DVC0,           0x17},
		{FS_PFD,            0x50},
		{FS_PRE,            0x6E},
		{FS_REG_DIV_CML,    0x14},
		{FS_SPARE,          0xAC},
		{FS_VCO0,           0xB4},
		{XOSC5,             0x0E},
		{XOSC1,             0x03},
};

// RX filter BW = 100.000000
// Packet bit length = 0
// Deviation = 20.019531
// Packet length mode = Variable
// Packet length = 255
// Carrier frequency = 902.750000
// Manchester enable = false
// TX power = 15
// PA ramping = true
// Device address = 0
// Symbol rate = 38.4
// Address config = No address check
// Bit rate = 38.4
// Modulation format = 2-GFSK
// Whitening = false
// Performance mode = High Performance
// Append packetinfo = false

const registerSetting_t preferredSettings_38400bps[]=
{
		{IOCFG3,            0xB0},
		{IOCFG2,            0x06},
		{IOCFG1,            0xB0},
		{IOCFG0,            0x06},
		{SYNC3,             0xD3},
		{SYNC2,             0x91},
		{SYNC1,             0xD3},
		{SYNC0,             0x91},
		{SYNC_CFG1,         0x08},
		{DEVIATION_M,       0x48},
		{MODCFG_DEV_E,      0x0D},
		{DCFILT_CFG,        0x1C},
		{PREAMBLE_CFG1,     0x18},
		{IQIC,              0x00},
		{CHAN_BW,           0x02},
		{MDMCFG0,           0x05},
		{SYMBOL_RATE2,      0x93},
		{SYMBOL_RATE1,      0xA9},
		{SYMBOL_RATE0,      0x2A},
		{AGC_CS_THR,        0x19},
		{AGC_CFG1,          0xA9},
		{AGC_CFG0,          0xCF},
		{FIFO_CFG,          0x00},
		{SETTLING_CFG,      0x0B},
		{FS_CFG,            0x12},
		{PKT_CFG2,          0x04},
		{PKT_CFG1,          0x04},
		{PKT_CFG0,          0x20},
		{PA_CFG2,           0x7F},
		{PA_CFG1,           0x56},
		{PA_CFG0,           0x7B},
		{PKT_LEN,           0xFF},
		{IF_MIX_CFG,        0x00},
		{FREQOFF_CFG,       0x22},
		{FREQOFF1,          0x00},
		{FREQOFF0,          0x00},
		{FREQ2,             0x70},
		{FREQ1,             0xD8},
		{FREQ0,             0x00},
		{FS_DIG1,           0x00},
		{FS_DIG0,           0x5F},
		{FS_CAL1,           0x40},
		{FS_CAL0,           0x0E},
		{FS_DIVTWO,         0x03},
		{FS_DSM0,           0x33},
		{FS_DVC0,           0x17},
		{FS_PFD,            0x50},
		{FS_PRE,            0x6E},
		{FS_REG_DIV_CML,    0x14},
		{FS_SPARE,          0xAC},
		{FS_VCO0,           0xB4},
		{XOSC5,             0x0E},
		{XOSC1,             0x03},
};

// Device address = 0
// Performance mode = High Performance
// Symbol rate = 50
// RX filter BW = 100.000000
// TX power = 15
// Modulation format = 2-GFSK
// Whitening = false
// Address config = No address check
// Packet length = 255
// Deviation = 24.963379
// Bit rate = 50
// Packet bit length = 0
// PA ramping = true
// Carrier frequency = 902.750000
// Packet length mode = Variable
// Manchester enable = false
// Append packetinfo = false

const registerSetting_t preferredSettings_50kbps[]=
{
		{IOCFG3,            0xB0},
		{IOCFG2,            0x06},
		{IOCFG1,            0xB0},
		{IOCFG0,            0x06},
		{SYNC3,             0xD3},
		{SYNC2,             0x91},
		{SYNC1,             0xD3},
		{SYNC0,             0x91},
		{SYNC_CFG1,         0x08},
		{SYNC_CFG0,         0x17},
		{DEVIATION_M,       0x99},
		{MODCFG_DEV_E,      0x0D},
		{DCFILT_CFG,        0x15},
		{PREAMBLE_CFG1,     0x18},
		{PREAMBLE_CFG0,     0x2A},
		{FREQ_IF_CFG,       0x3A},
		{IQIC,              0x00},
		{CHAN_BW,           0x02},
		{MDMCFG1,           0x46},
		{MDMCFG0,           0x05},
		{SYMBOL_RATE2,      0x99},
		{SYMBOL_RATE1,      0x99},
		{SYMBOL_RATE0,      0x99},
		{AGC_REF,           0x3C},
		{AGC_CS_THR,        0xEF},
		{AGC_CFG1,          0xA9},
		{AGC_CFG0,          0xC0},
		{FIFO_CFG,          0x00},
		{SETTLING_CFG,      0x0B},
		{FS_CFG,            0x12},
		{PKT_CFG2,          0x04},
		{PKT_CFG1,          0x04},
		{PKT_CFG0,          0x20},
		{PA_CFG2,           0x7F},
		{PA_CFG1,           0x56},
		{PA_CFG0,           0x79},
		{PKT_LEN,           0xFF},
		{IF_MIX_CFG,        0x00},
		{FREQOFF_CFG,       0x20},
		{TOC_CFG,           0x0A},
		{FREQOFF1,          0x00},
		{FREQOFF0,          0x00},
		{FREQ2,             0x70},
		{FREQ1,             0xD8},
		{FREQ0,             0x00},
		{FS_DIG1,           0x00},
		{FS_DIG0,           0x5F},
		{FS_CAL1,           0x40},
		{FS_CAL0,           0x0E},
		{FS_DIVTWO,         0x03},
		{FS_DSM0,           0x33},
		{FS_DVC0,           0x17},
		{FS_PFD,            0x50},
		{FS_PRE,            0x6E},
		{FS_REG_DIV_CML,    0x14},
		{FS_SPARE,          0xAC},
		{FS_VCO0,           0xB4},
		{XOSC5,             0x0E},
		{XOSC1,             0x03},
};


/******************************************************************************
 * @fn         radio_init
 *
 * @brief      Initialize the radio hardware
 *
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 *
 */
int radio_init(unsigned char config_select) {

	unsigned char i, writeByte, preferredSettings_length;
	unsigned int bit_rate;
	registerSetting_t *preferredSettings;


	/* Instantiate transceiver RF SPI interface to SCLK ~ 4 MHz */
	/* Input parameter is clockDivider */
	/* SCLK frequency = SMCLK/clockDivider */
	trxRfSpiInterfaceInit(4);

	/* remove the reset from the rf device */
	//rf_reset = 1;

	/* Reset radio */
	trxSpiCmdStrobe(SRES);

	/* give the tranciever time enough to complete reset cycle */
	wait_ms(16);

	switch (config_select) {
	case 1:
		preferredSettings_length = sizeof(preferredSettings_1200bps)/sizeof(registerSetting_t);
		preferredSettings = (registerSetting_t *)preferredSettings_1200bps;
		bit_rate = 12;
		break;
	case 2:
		preferredSettings_length = sizeof(preferredSettings_38400bps)/sizeof(registerSetting_t);
		preferredSettings = (registerSetting_t *)preferredSettings_38400bps;
		bit_rate = 384;
		break;
	case 3:
		preferredSettings_length = sizeof(preferredSettings_50kbps)/sizeof(registerSetting_t);
		preferredSettings = (registerSetting_t *)preferredSettings_50kbps;
		bit_rate = 500;
		break;
	default:
		preferredSettings_length = sizeof(preferredSettings_1200bps)/sizeof(registerSetting_t);
		preferredSettings = (registerSetting_t *)preferredSettings_1200bps;
		bit_rate = 12;
		break;
	}

	/* Write registers to radio */
	for(i = 0; i < preferredSettings_length; i++) {

		if(preferredSettings[i].addr < 0x2F) {
			writeByte = preferredSettings[i].data;
			trx8BitRegAccess(RADIO_WRITE_ACCESS, preferredSettings[i].addr, &writeByte, 1);
		} else {
			writeByte = preferredSettings[i].data;
			trx16BitRegAccess(RADIO_WRITE_ACCESS, 0x2F , (0xFF & preferredSettings[i].addr),
					&writeByte, 1);
		}
	}

	/* enable range extender */
#ifdef ENABLE_RANGE_EXTENDER
	range_extender_init();
#endif

	return bit_rate;
}

/******************************************************************************
 * @fn         radio_prepare
 *
 * @brief      Prepare the radio with a packet to be sent, but do not send
 *
 * input parameters
 *
 * @param       unsigned char *payload     - pointer to payload
 *              unsigned short payload_len - payload length information
 *
 * output parameters
 *
 * @return      0
 *
 */
int radio_prepare(unsigned char *payload, unsigned short payload_len) {

	trx8BitRegAccess(RADIO_WRITE_ACCESS+RADIO_BURST_ACCESS, TXFIFO, payload, payload_len);

	return 0;
}

/******************************************************************************
 * @fn         radio_transmit
 *
 * @brief      Send the packet that has previously been prepared (used for
 *             exact timing)
 *
 * input parameters
 *
 * @param       unsigned char *payload     - pointer to payload
 *              unsigned short payload_len - payload length information
 *
 * output parameters
 *
 * @return      0
 *
 */
int radio_transmit(void) {

	/* Range extender in TX mode */
#ifdef ENABLE_RANGE_EXTENDER
	range_extender_txon();
#endif

	/* Change state to TX, initiating */
	trxSpiCmdStrobe(STX);

	return(0);
}

/******************************************************************************
 * @fn         radio_receive_on
 *
 * @brief      Initiate the RX chain
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 *
 */
int radio_receive_on(void) {

	/* Range extender in RX mode */
#ifdef ENABLE_RANGE_EXTENDER
	range_extender_rxon();
#endif

	/* Strobe RX to initiate the recieve chain */
	trxSpiCmdStrobe(SRX);

	return 0;
}

/******************************************************************************
 * @fn         radio_send
 *
 * @brief      Prepare & transmit a packet in same call
 *
 *
 * input parameters
 *
 * @param       unsigned char *payload
 *              unsigned short payload_len
 *
 * output parameters
 *
 * @return      void
 *
 *
 */
int radio_send(unsigned char *payload, unsigned short payload_len) {

	/* Write packet to TX FIFO */
	trx8BitRegAccess(RADIO_WRITE_ACCESS|RADIO_BURST_ACCESS, TXFIFO, payload, payload_len);

	/* Range extender in TX mode */
#ifdef ENABLE_RANGE_EXTENDER
	range_extender_txon();
#endif

	/* Strobe TX to send packet */
	trxSpiCmdStrobe(STX);               // Change state to TX, initiating

	return 0;
}

/******************************************************************************
 * @fn         radio_read
 *
 * @brief      Read a received packet into a buffer
 *
 *
 * input parameters
 *
 * @param       unsigned char *buf
 *              unsigned short buf_len
 *
 * output parameters
 *
 * @return      void
 *
 *
 */
int radio_read(unsigned char *buf, unsigned short *buf_len) {
	unsigned char status;
	unsigned char pktLen;

	/* Read number of bytes in RX FIFO */
	trx16BitRegAccess(RADIO_READ_ACCESS, 0x2F, 0xff & NUM_RXBYTES, &pktLen, 1);
	pktLen = pktLen  & NUM_RXBYTES;

	/* make sure the packet size is appropriate, that is 1 -> buffer_size */
	if ((pktLen > 0) && (pktLen <= *buf_len)) {

		/* retrieve the FIFO content */
		trx8BitRegAccess(RADIO_READ_ACCESS|RADIO_BURST_ACCESS, RXFIFO, buf, pktLen);

		/* return the actual length of the FIFO */
		*buf_len = pktLen;

		/* retrieve the CRC status information */
		trx16BitRegAccess(RADIO_READ_ACCESS, 0x2F, 0xff & LQI_VAL, &status, 1);

		/* Return CRC_OK bit */
		status  = status & CRC_OK;

	} else {

		/* if the length returned by the transciever does not make sense, flush it */

		*buf_len = 0;                                // Return 0 indicating a failure
		status = 0;                                  // Return 0 indicating a failure
		trxSpiCmdStrobe(SFRX);	                     // Flush RXFIFO

	}

	/* return status information, CRC OK or NOT OK */
	return (status);
}

/******************************************************************************
 * @fn         radio_channel_clear
 *
 * @brief      Perform a Clear-Channel Assessment (CCA) to find out if
 *             channel is clear
 *
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      0  - no carrier found
 *              >0 - carrier found
 *
 */
int radio_channel_clear(void) {
	unsigned char status;

	/* get RSSI0, and return the carrier sense signal */
	trx16BitRegAccess(RADIO_READ_ACCESS, 0x2F, 0xff & RSSI0, &status, 1);

	/* return the carrier sense signal */
	return(status  & 0x04);
}


/******************************************************************************
 * @fn         radio_channel_clear
 *
 * @brief      Wait for end of packet interupt to happen
 *
 *             Timeout is controlled by TimerA running at 8MHz
 *             64000 = 128ms, 32000 = 64ms, 16000 = 32ms
 *             0 = no timeout.
 *
 * input parameters
 *
 * @param       max_hold   :  Watch dog timeout, no end of packet = 0;
 *
 * output parameters
 *
 * @return      timer value:  Interupt happened based on end_of_packet interupt
 *
 */
int radio_wait_for_idle(unsigned short max_hold) {

	unsigned int status = 0;
	unsigned char reg_status = 0x0;
        
	while(!(reg_status == MARCSTATE_IDLE)) {
        if(max_hold > 0 && status > max_hold) break;
        
		trx16BitRegAccess(RADIO_READ_ACCESS, 0x2F, 0xff & MARCSTATE, &reg_status, 1);
		reg_status  = (reg_status & 0x1F);
        //printf("reg_status = %02X\r\n", reg_status);
        
        switch(reg_status) {
            case RX_FIFO_ERR:
                trxSpiCmdStrobe(SFRX);
            break;
            case TX_FIFO_ERR:
                trxSpiCmdStrobe(SFTX);
            break;
            default:
                status+=10;
                wait_ms(10);
            break;
        }
	}

#ifdef ENABLE_RANGE_EXTENDER
	range_extender_idle();
#endif

	return status;
}


/******************************************************************************
 * @fn         radio_set_pwr
 *
 * @brief      Set the output power of the CC112x by looking up in table
 *
 * input parameters
 *
 * @param       int tx_pwr
 *
 * output parameters
 *
 * @return      int tx_pwr
 *
 */
int radio_set_pwr(int tx_pwr) {

	return tx_pwr;
}


/******************************************************************************
 * @fn          radio_set_freq
 *
 * @brief       Calculate the required frequency registers and send the using
 *              serial connection to the RF tranceiver.
 *
 * input parameters
 *
 * @param       freq   -  frequency word provided in [kHz] resolution
 *
 * output parameters
 *
 * @return      void
 */
int radio_set_freq(unsigned long freq) {

	uint8 freq_regs[3];
	uint32 freq_regs_uint32;
	float f_vco;

	/* Radio frequency -> VCO frequency */
	f_vco = freq * RF_LO_DIVIDER;

	/* Divide by oscillator frequency */
	f_vco = f_vco * (1/(float)RF_XTAL_FREQ);

	/* Multiply by 2^16 */
	f_vco = f_vco * 65536;

	/* Convert value into uint32 from float */
	freq_regs_uint32 = (uint32) f_vco;

	/* return the frequency word */
	freq_regs[2] = ((uint8*)&freq_regs_uint32)[0];
	freq_regs[1] = ((uint8*)&freq_regs_uint32)[1];
	freq_regs[0] = ((uint8*)&freq_regs_uint32)[2];

	/* write the frequency word to the transciever */
	trx16BitRegAccess(RADIO_WRITE_ACCESS | RADIO_BURST_ACCESS, 0x2F, (0xFF & FREQ2), freq_regs, 3);

	return 0;
}


/******************************************************************************
 * @fn         radio_idle
 *
 * @brief      Idle the radio, used when leaving low power modes (below)
 *
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 *
 */
int radio_idle(void) {

#ifdef ENABLE_RANGE_EXTENDER
	/* Idle range extender */
	range_extender_idle();
#endif

	/* Force transciever idle state */
	trxSpiCmdStrobe(SIDLE);

	/* Flush the FIFO's */
	trxSpiCmdStrobe(SFRX);
	trxSpiCmdStrobe(SFTX);

	return(0);
}

/******************************************************************************
 * @fn         radio_sleep
 *
 * @brief      Enter sleep mode
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 *
 */
int radio_sleep(void) {

	/* Idle range extender */
#ifdef ENABLE_RANGE_EXTENDER
	range_extender_idle();
#endif

	/* Force transciever idle state */
	trxSpiCmdStrobe(SIDLE);

	/* Enter sleep state on exit */
	trxSpiCmdStrobe(SPWD);

	return(0);
}

/******************************************************************************
 * @fn         radio_wake
 *
 * @brief      Exit sleep mode
 *
 * input parameters
 *
 * @param       void
 *
 * output parameters
 *
 * @return      void
 *
 */
int radio_wake(void) {

	/* Force transciever idle state */
	trxSpiCmdStrobe(SIDLE);

	/* 1 ms delay for letting RX settle */
	wait_ms(1);

	return(0);
}


/******************************************************************************
 * @fn          radio_freq_error
 *
 * @brief       Estimate the frequency error from two complement coded
 *              data to frequency error in Hertz
 *
 * input parameters
 *
 * @param       freq_reg_error -  two complement formatted data from tranceiver
 *
 * output parameters
 *
 * @return      freq_error     - 32 bit signed integer value representing
 *                                frequency error in Hertz
 *
 */
int radio_freq_error(void) {

	unsigned long freq_error_est;
	long freq_error_est_int;
	unsigned char sign, regState, regState1;
	unsigned int freq_reg_error;

    /* Read marcstate to check for frequency error estimate */
	trx16BitRegAccess(RADIO_READ_ACCESS, 0x2F, (0xFF & FREQOFF_EST0), &regState, 1);
	trx16BitRegAccess(RADIO_READ_ACCESS, 0x2F, (0xFF & FREQOFF_EST1), &regState1, 1);

    /* Calculate the frequency error in Hz */
	freq_reg_error = ((unsigned int)regState1 << 8) + regState;

	/* the incoming data is 16 bit two complement format, separate "sign" */
	if (freq_reg_error > 32768) {
		freq_error_est = -(freq_reg_error - 65535);
		sign = 1;
	} else {
		freq_error_est = freq_reg_error;
		sign = 0;
	}

	/* convert the data to hertz format in two steps to avoid integer overuns */
	freq_error_est = (freq_error_est * (RF_XTAL_FREQ/RF_LO_DIVIDER)) >> 8;
	freq_error_est = (freq_error_est * 1000) >> 10;

	/* re-assign the "sign" */
	if(sign == 1) {
		freq_error_est_int = -freq_error_est;
	} else {
		freq_error_est_int = freq_error_est;
	}

	return freq_error_est_int;
}

