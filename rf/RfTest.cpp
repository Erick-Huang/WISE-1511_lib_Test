#include "mbed.h"
#include "main.h"
#include "RfTest.h"
#include "SpectrumTest.h"
#include "simpleCli.h"

static Serial *serial = NULL;
#define rfprintf serial->printf

DigitalOut rf_reset(SPI_RF_RESET);
DigitalOut rf_hgm(RF_HGM);
DigitalOut rf_lna_en(RF_LNA_EN);
DigitalOut rf_pa_en(RF_PA_EN);



int runTX(int index, char *string, int len, void *printer);
int runRX(int index, char *string, int len, void *printer);

int runLTX(int index, char *string, int len, void *printer);
int runLRX(int index, char *string, int len, void *printer);

int runLTX_4GFSK_500(int index, char *string, int len, void *printer);
int runLRX_4GFSK_500(int index, char *string, int len, void *printer);

int runLTX_2FSK_300(int index, char *string, int len, void *printer);
int runLRX_2FSK_300(int index, char *string, int len, void *printer);

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int RFInit(void) {
    rf_reset = 1;
    rf_hgm = 1;
    rf_lna_en = 0;
    rf_pa_en = 0;

    // Write radio registers
    trxRfSpiInterfaceInit(0);
}

int RFTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    serial = (Serial*)printer;
    char name[64] = {0};
    char *deviceName = "Unknown";
    switch(get_device_id()) {
        case DEV_CC1120:
            deviceName = "CC1120(US)";
            break;
        case DEV_CC1125:
            deviceName = "CC1125(EU)";
            break;
        default:
            break;
    }
    sprintf(name, AP_NAME " TX/RX Test(%s)",deviceName);
    
	simple_cmds cmds[] = {
			{ name/*title*/,  		                    NULL,NULL },
            { "Spectrum Test",                              0   ,SpectrumTest},
//			{ "TX",				                            0   ,runTX },
//			{ "RX",				        	                0   ,runRX },
            { "LTX",				                        0   ,runLTX },
			{ "LRX",				        	            0   ,runLRX },
            { "LTX(4-GFSK, 500 bps)",				    0   ,runLTX_4GFSK_500 },
			{ "LRX(4-GFSK, 500 bps)",				    0   ,runLRX_4GFSK_500 },
            { "LTX(2-FSK, 300 bps)",				    0   ,runLTX_2FSK_300 },
			{ "LRX(2-FSK, 300 bps)",				    0   ,runLRX_2FSK_300 },
			{ NULL,											NULL,NULL }
	};
	return simple_cli_list(cmds);
}

void registerConfig(registerSetting_t *preferred, int number) {
  
  uint8 writeByte;
  if(preferred == NULL || number == 0) {
        preferred = (registerSetting_t *)preferredSettings;
        number = sizeof(preferredSettings)/sizeof(registerSetting_t);
  }
  
  
  // Reset radio
  trxSpiCmdStrobe(CC112X_SRES);
  
  wait_ms(16);
  
  // Write registers to radio
  for(uint16 i = 0; i < number; i++) {
    writeByte =  preferred[i].data;
    cc112xSpiWriteReg( preferred[i].addr, &writeByte, 1);
  }
}

static uint32 txPacketCounter = 0;
static uint32 rxPacketCounter = 0;
/******************************************************************************
* @fn          createPacket
*
* @brief       This function is called before a packet is transmitted. It fills
*              the txBuffer with a packet consisting of a 4 byte preamble, 
*              4 byte sync word, length byte, two bytes packet counter and
*              n random bytes. If fixed packet length is used, the length byte 
*              is excluded.
*                
* @param       none
*
* @return      none
*/
static void createPacket(uint8 txBuffer[]) {
    uint8 i;
    uint8 index;
    uint8 test;
  
    //Create preamble
    for(i=0; i<PREAMBLE_LENGTH; i++) {
      txBuffer[i] = 0xAA;
    }
    
    //Insert syncword from registers
    cc112xSpiReadReg(CC112X_SYNC3, &test, 1);
    cc112xSpiReadReg(CC112X_SYNC3, &txBuffer[PREAMBLE_LENGTH], 1);
    cc112xSpiReadReg(CC112X_SYNC2, &txBuffer[PREAMBLE_LENGTH+1], 1);
    cc112xSpiReadReg(CC112X_SYNC1, &txBuffer[PREAMBLE_LENGTH+2], 1);
    cc112xSpiReadReg(CC112X_SYNC0, &txBuffer[PREAMBLE_LENGTH+3], 1);
    
    //If fixed packet length insert the packet number
    if(FIXED_LENGTH) {
      txBuffer[PREAMBLE_LENGTH+4] = (uint8) txPacketCounter >> 8; // MSB of packetCounter
      txBuffer[PREAMBLE_LENGTH+5] = (uint8) txPacketCounter;      // LSB of packetCounter
      index = PREAMBLE_LENGTH+6;
    } else {
      txBuffer[PREAMBLE_LENGTH+4] = (uint8) (PKTLEN - 1);               // Length byte
      txBuffer[PREAMBLE_LENGTH+5] = (uint8) (txPacketCounter >> 8); // MSB of packetCounter
      txBuffer[PREAMBLE_LENGTH+6] = (uint8) txPacketCounter;      // LSB of packetCounter
      index = PREAMBLE_LENGTH+7;
    }
    
    //Fill rest of buffer with random bytes
    for(i=index; i< (PKTLEN+PREAMBLE_LENGTH+5); i++) {
      txBuffer[i] = (uint8)rand();
    }
}
#if 0
/*******************************************************************************
*   @fn         runTX
*
*   @brief      Continuously sends packets on button push until button is pushed
*               again. After the radio has gone into TX the function waits for
*               interrupt that packet has been sent. Updates packet counter and
*               display for each packet sent.
*
*   @param      none
*
*   @return    none
*/
int runTX(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    // Initialize packet buffer of size PKTLEN + 1
    uint8 txBuffer[PKTLEN+1] = {0};
    registerConfig(NULL, 0);
    // Calibrate radio according to errata
    //manualCalibration();

    // Infinite loop
    while(1) {

        // Wait for button push
        do {
            // Update packet counter
            txPacketCounter++;

            // Create a random packet with PKTLEN + 2 byte packet
            // counter + n x random bytes
            createPacket(txBuffer);

            // Write packet to TX FIFO
            cc112xSpiWriteTxFifo(txBuffer, sizeof(txBuffer));

            // Strobe TX to send packet
            trxSpiCmdStrobe(CC112X_STX);
            
            //Wait for a random amount of time between each packet
            wait_ms(30);
            rfprintf("txPacketCounter = %d\r\n", txPacketCounter);
        } while (1);
        
    }
    
    return true;
}





int runRX(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    uint8 rxBuffer[128] = {0};
    uint8 rxBytes;
    uint8 marcState;
    registerConfig(NULL, 0);
    // Calibrate radio according to errata
    //manualCalibration();

    // Set radio in RX
    trxSpiCmdStrobe(CC112X_SRX);

    // Infinite loop
    while(TRUE) {
            // Read number of bytes in RX FIFO
            cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);

            // Check that we have bytes in FIFO
            if(rxBytes != 0) {

                // Read MARCSTATE to check for RX FIFO error
                cc112xSpiReadReg(CC112X_MARCSTATE, &marcState, 1);

                // Mask out MARCSTATE bits and check if we have a RX FIFO error
                if((marcState & 0x1F) == RX_FIFO_ERROR) {

                    // Flush RX FIFO
                    trxSpiCmdStrobe(CC112X_SFRX);
                } else {

                    // Read n bytes from RX FIFO
                    cc112xSpiReadRxFifo(rxBuffer, rxBytes);

                    // Check CRC ok (CRC_OK: bit7 in second status byte)
                    // This assumes status bytes are appended in RX_FIFO
                    // (PKT_CFG1.APPEND_STATUS = 1)
                    // If CRC is disabled the CRC_OK field will read 1
                    if(rxBuffer[rxBytes - 1] & 0x80) {

                        // Update packet counter
                        rxPacketCounter++;
                    }
                }
                rfprintf("rxPacketCounter = %d\n", rxPacketCounter);
            }
            // Set radio back in RX
            trxSpiCmdStrobe(CC112X_SRX);
    }
    
    return true;
}
#endif

const static unsigned char rand_data[60] =  {228,185,142, 'A','d','v','a','n','t','e','c','h',0,
                51,253,117,172,161,191,79,58,225,215,149,251,15,163,153,236,141,172,3,
                186,224,37,224,210,75,69,117,58,153,207,61,203,19,125,76,90,143,226,11,
                208,16,72,109,217,100,12,128,};

void tx_ber_single(int pktlen = 20, int freq = 902705, int chbw = 500, int channel = 25, int pktnum = 1000)
{
    unsigned char txBuffer[32];
	unsigned int cc, dd;
	unsigned short rx_length, data_error;
	signed int rssi_forward, rssi_reverse, freq_error;
	unsigned int status, total_pkt_count;
	unsigned long error_forward, error_reverse;

	rfprintf("\n\r");
	rfprintf("Perform RF burst transmit function with fixed channel\n\r");

	// Set the packet length to a fixed packet size
	set_rf_packet_length(pktlen);

	/* configure the rx frequency to perform packet sniffing on */
	radio_set_freq(freq+(chbw*channel));

	error_forward = 0;
	error_reverse = 0;
	total_pkt_count = 0;

	// start the timer
	//hal_timer_init(trx_cfg->packet_timer);

	// start transmitting burst
	rfprintf(" PKT FW-RSSI/ERR RW-RSSI/ERR FREQ_ERR\n\r");

	for(cc=0; cc<pktnum; cc++)
	{

		// enter low power mode and wait for TimerA1
		//hal_timer_expire();

		// put some data into the payload
		txBuffer[0] =  SYSTEM_ID;                            // System ID
		txBuffer[1] = (cc>>8) & 0xFF;                        // Burst number (MSB)
		txBuffer[2] =  cc     & 0xFF;                        // Burst number (LSB)
		/*for(dd=3;dd<pktlen;dd++)
		{
			txBuffer[dd] = rand_data[dd];
		}*/
        
        sprintf((char *)&txBuffer[3],"(%d)_Advantech",cc);
		// Transmit packet. The MCU does not exit this until the TX is complete.
        //rfprintf("<%s,%d>\r\n",__FILE__,__LINE__);
		radio_send(txBuffer, pktlen);
        //rfprintf("<%s,%d>\r\n",__FILE__,__LINE__);
		radio_wait_for_idle(0);         // 0 = no timeout, just wait
        //rfprintf("<%s,%d>\r\n",__FILE__,__LINE__);
        if(ENABLE_TWO_WAY_LINK == TRUE)
		{
			radio_receive_on();              // Change state to RX, receive packet
			// wait until end_of_packet is found or timeout (if packet is not found)
			status = radio_wait_for_idle(8000);

			if(status <= 8000)
			{

				rx_length = pktlen;
				radio_read(txBuffer, &rx_length);

				/* calculate the RSSI from the information from the RADIO registers */
				rssi_reverse = radio_get_rssi();
				freq_error = radio_freq_error();
				if(txBuffer[3] >= 128)
				{
					rssi_forward = -(256 - txBuffer[3]);
				} else {
					rssi_forward = txBuffer[3];
				}

				data_error = 0;
				for(dd=5; dd<pktlen; dd++){
					if( txBuffer[dd] != rand_data[dd] )
					{
						data_error++;
					}
				}
				dd = (txBuffer[1] << 8) + txBuffer[2];
				error_forward = error_forward + txBuffer[4];
				error_reverse = error_reverse + data_error;
				total_pkt_count++;
			}
			else
			{
				wait_ms(1);
				radio_idle();                 // Force IDLE state and Flush the RX FIFO's
				rssi_forward = 0;
				rssi_reverse = 0;
				data_error   = pktlen;
				error_forward = error_forward + data_error;
				error_reverse = error_reverse + data_error;
				total_pkt_count++;
			}
		}

		if(ENABLE_TWO_WAY_LINK == TRUE)
		{
			rfprintf("%4d %3i %3i     %3i %3i     %3i\n\r", cc, rssi_forward, txBuffer[4],
					rssi_reverse, data_error, freq_error);
		}
		else
		{
			rfprintf("%4d %3i\n\r", cc, freq_error);
		}

	}

	rfprintf("Total packets Sent     : %5d\n\r", pktnum);
	rfprintf("Total packets Received : %5d\n\r", total_pkt_count);
	rfprintf("Forward Error count    : %7ld\n\r", error_forward);
	rfprintf("Reverse Error count    : %7ld\n\r", error_reverse);

	return;
}

void rx_ber_single(int pktlen = 20, int freq = 902705, int chbw = 500, int channel = 25, int pktnum = 1000)
{
    unsigned char txBuffer[32];
    unsigned char content[32];
	unsigned int cc, dd;
	unsigned short rx_length;
	unsigned char cc_status;
	unsigned int pck_good_cnt = 0;
	int rssi, freq_error;
	char char_rssi;
	unsigned int curr_pkt_id;

	rfprintf("\n\r");
	rfprintf("Perform RF burst receive function with fixed channel\n\r");

	// Set the packet length to a fixed packet size
	set_rf_packet_length(pktlen);

	/* configure the rx frequency to perform packet sniffing on */
	radio_set_freq(freq+(chbw*channel));

	/* Initialize the RX chain, receive packet */
	radio_receive_on();

	// start transmitting burst
	rfprintf(" PKT FW-RSSI/ERR FREQ_ERR\r\n");

	for(cc=0; cc<pktnum; cc++)
	{
		// wait until end_of_packet is found, no timeout
		radio_wait_for_idle(0);
		rx_length = sizeof(txBuffer);
		cc_status = radio_read(txBuffer, &rx_length);         // read content of FIFO
        
		if(cc_status > 0)
		{
			pck_good_cnt++;
            strncpy((char *)content, (char *)&txBuffer[3],sizeof(content));

			rssi = radio_get_rssi();
			freq_error = radio_freq_error();
			if(ENABLE_TWO_WAY_LINK == TRUE)
			{
				char_rssi = (char)rssi;
				txBuffer[3] = (unsigned char)char_rssi;          // insert measurement in
				txBuffer[4] = 0;                                  // return data

				for(dd=5;dd<pktlen;dd++)              // count errors in data
				{
					if( txBuffer[dd] != rand_data[dd] )
					{
						txBuffer[4] = txBuffer[4] + 1;                // error count is byte 3
						txBuffer[dd] = rand_data[dd];                 // clean data for return
					}
				}

				radio_send(txBuffer, pktlen);  // standard packet send

				radio_wait_for_idle(0);         // 0 = no timeout, just wait
			}
		}
		else
		{
			// Check to see if we have lost the connection and we need to stop and hold
			radio_idle();                          // force idle and flush fifos
			// if max_wait == 0 that means we are waiting for first sync burst to appear
		}

		/* Initialize the RX chain, receive packet */
		radio_receive_on();

        curr_pkt_id = (txBuffer[1]<<8) + txBuffer[2];
        rfprintf("%4d %3i %3i    content = %s\n\r", curr_pkt_id, rssi, freq_error, content);
	}

	return;
}



int runLTX(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    registerConfig(NULL, 0);
    tx_ber_single(20, 868000, 500, 0, 1000);
    return true;
}

int runLRX(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    registerConfig(NULL, 0);
    rx_ber_single(20, 868000, 500, 0, 1000);
    return true;
}

int runLTX_4GFSK_500(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    const registerSetting_t settings[]= 
    {
        {CC1125_IOCFG3,            0xB0},
        {CC1125_IOCFG2,            0x06},
        {CC1125_IOCFG1,            0xB0},
        {CC1125_IOCFG0,            0x40},
        {CC1125_SYNC_CFG1,         0x08},
        {CC1125_MODCFG_DEV_E,      0x2B},
        {CC1125_DCFILT_CFG,        0x1C},
        {CC1125_IQIC,              0xC6},
        {CC1125_CHAN_BW,           0x10},
        {CC1125_MDMCFG0,           0x05},
        {CC1125_SYMBOL_RATE2,      0x20},
        {CC1125_SYMBOL_RATE1,      0x62},
        {CC1125_SYMBOL_RATE0,      0x4E},
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
        {CC1125_FREQ2,             0x6C},
        {CC1125_FREQ1,             0x80},
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

    registerConfig((registerSetting_t *)settings, sizeof(settings)/sizeof(registerSetting_t));
    tx_ber_single(20, 868000, 500, 0, 1000);
    return true;
}

int runLRX_4GFSK_500(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    const registerSetting_t settings[]= 
    {
        {CC1125_IOCFG3,            0xB0},
        {CC1125_IOCFG2,            0x06},
        {CC1125_IOCFG1,            0xB0},
        {CC1125_IOCFG0,            0x40},
        {CC1125_SYNC_CFG1,         0x08},
        {CC1125_MODCFG_DEV_E,      0x2B},
        {CC1125_DCFILT_CFG,        0x1C},
        {CC1125_IQIC,              0xC6},
        {CC1125_CHAN_BW,           0x10},
        {CC1125_MDMCFG0,           0x05},
        {CC1125_SYMBOL_RATE2,      0x20},
        {CC1125_SYMBOL_RATE1,      0x62},
        {CC1125_SYMBOL_RATE0,      0x4E},
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
        {CC1125_FREQ2,             0x6C},
        {CC1125_FREQ1,             0x80},
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
    registerConfig((registerSetting_t *)settings, sizeof(settings)/sizeof(registerSetting_t));
    rx_ber_single(20, 868000, 500, 0, 1000);
    return true;
}

int runLTX_2FSK_300(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    const registerSetting_t settings[]= 
    {
        {CC1125_IOCFG3,            0xB0},
        {CC1125_IOCFG2,            0x06},
        {CC1125_IOCFG1,            0xB0},
        {CC1125_IOCFG0,            0x40},
        {CC1125_SYNC_CFG1,         0x0B},
        {CC1125_MODCFG_DEV_E,      0x01},
        {CC1125_DCFILT_CFG,        0x1C},
        {CC1125_IQIC,              0xC6},
        {CC1125_CHAN_BW,           0x69},
        {CC1125_MDMCFG0,           0x05},
        {CC1125_SYMBOL_RATE2,      0x23},
        {CC1125_AGC_REF,           0x15},
        {CC1125_AGC_CS_THR,        0x19},
        {CC1125_AGC_CFG1,          0xA9},
        {CC1125_AGC_CFG0,          0xCF},
        {CC1125_FIFO_CFG,          0x00},
        {CC1125_SETTLING_CFG,      0x03},
        {CC1125_FS_CFG,            0x12},
        {CC1125_PKT_CFG0,          0x20},
        {CC1125_PA_CFG2,           0x7F},
        {CC1125_PA_CFG0,           0x7E},
        {CC1125_PKT_LEN,           0xFF},
        {CC1125_IF_MIX_CFG,        0x00},
        {CC1125_FREQOFF_CFG,       0x22},
        {CC1125_FREQ2,             0x6C},
        {CC1125_FREQ1,             0x80},
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
        {CC1125_PARTNUMBER,        0x58},
        {CC1125_PARTVERSION,       0x21},
        {CC1125_MODEM_STATUS1,     0x10},
        {CC1125_XOSC_TEST1,        0x00},
    };

    registerConfig((registerSetting_t *)settings, sizeof(settings)/sizeof(registerSetting_t));
    tx_ber_single(20, 868000, 500, 0, 1000);
    return true;
}

int runLRX_2FSK_300(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    const registerSetting_t settings[]= 
    {
        {CC1125_IOCFG3,            0xB0},
        {CC1125_IOCFG2,            0x06},
        {CC1125_IOCFG1,            0xB0},
        {CC1125_IOCFG0,            0x40},
        {CC1125_SYNC_CFG1,         0x0B},
        {CC1125_MODCFG_DEV_E,      0x01},
        {CC1125_DCFILT_CFG,        0x1C},
        {CC1125_IQIC,              0xC6},
        {CC1125_CHAN_BW,           0x69},
        {CC1125_MDMCFG0,           0x05},
        {CC1125_SYMBOL_RATE2,      0x23},
        {CC1125_AGC_REF,           0x15},
        {CC1125_AGC_CS_THR,        0x19},
        {CC1125_AGC_CFG1,          0xA9},
        {CC1125_AGC_CFG0,          0xCF},
        {CC1125_FIFO_CFG,          0x00},
        {CC1125_SETTLING_CFG,      0x03},
        {CC1125_FS_CFG,            0x12},
        {CC1125_PKT_CFG0,          0x20},
        {CC1125_PA_CFG2,           0x7F},
        {CC1125_PA_CFG0,           0x7E},
        {CC1125_PKT_LEN,           0xFF},
        {CC1125_IF_MIX_CFG,        0x00},
        {CC1125_FREQOFF_CFG,       0x22},
        {CC1125_FREQ2,             0x6C},
        {CC1125_FREQ1,             0x80},
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
        {CC1125_PARTNUMBER,        0x58},
        {CC1125_PARTVERSION,       0x21},
        {CC1125_MODEM_STATUS1,     0x10},
        {CC1125_XOSC_TEST1,        0x00},
    };
    registerConfig((registerSetting_t *)settings, sizeof(settings)/sizeof(registerSetting_t));
    rx_ber_single(20, 868000, 500, 0, 1000);
    return true;
}