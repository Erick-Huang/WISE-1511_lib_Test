#include "mbed.h"
#include "hal_spi_rf.h"


/******************************************************************************
 * LOCAL FUNCTIONS
 */
//static void trxReadWriteBurstSingle(uint8 addr,uint8 *pData,uint16 len) ;


/******************************************************************************
 * FUNCTIONS
 */



/******************************************************************************
 * @fn          void trxRfSpiInterfaceInit_exp430g2(uint8 prescalerValue)
 *
 * @brief       Function to initialize TRX SPI. CC1101/CC112x is currently
 *              supported. The supported prescalerValue must be set so that
 *              SMCLK/prescalerValue does not violate radio SPI constraints.
 *
 * input parameters
 *
 * @param       prescalerValue - SMCLK/prescalerValue gives SCLK frequency
 *
 * output parameters
 *
 * @return      void
 */
 
 
SPI rf_spi(SPI_RF_MOSI, SPI_RF_MISO, SPI_RF_SCK); //2
DigitalOut rf_cs(SPI_RF_CS);

void trxRfSpiInterfaceInit(uint8 prescalerValue)
{
    rf_spi.format(8,0);
    rf_spi.frequency(1000000);
	rf_cs = 1;
    
	return;
}

/*******************************************************************************
 * @fn          trx8BitRegAccess
 *
 * @brief       This function performs a read or write from/to a 8bit register
 *              address space. The function handles burst and single read/write
 *              as specfied in addrByte. Function assumes that chip is ready.
 *
 * input parameters
 *
 * @param       accessType - Specifies if this is a read or write and if it's
 *                           a single or burst access. Bitmask made up of
 *                           RADIO_BURST_ACCESS/RADIO_SINGLE_ACCESS/
 *                           RADIO_WRITE_ACCESS/RADIO_READ_ACCESS.
 * @param       addrByte - address byte of register.
 * @param       pData    - data array
 * @param       len      - Length of array to be read(TX)/written(RX)
 *
 * output parameters
 *
 * @return      chip status
 */
rfStatus_t trx8BitRegAccess(uint8 accessType, uint8 addrByte, uint8 *pData, uint16 len)
{
	uint8 readValue;
    int i = 0;
	/* Pull CS_N low and wait for SO to go low before communication starts */
	rf_spi.lock();
    rf_cs = 0;
	rf_spi.write(accessType|addrByte);
    switch(accessType) {
        case RADIO_READ_ACCESS|RADIO_BURST_ACCESS:
            for(i = 0 ; i < len ; i++) {
                pData[i] = rf_spi.write(0x00);
            }
        break;
        
        case RADIO_WRITE_ACCESS|RADIO_BURST_ACCESS:
            for(i = 0 ; i < len ; i++) {
                rf_spi.write(pData[i]);
            }
        break;
        
        case RADIO_READ_ACCESS|RADIO_SINGLE_ACCESS:
            pData[0] = rf_spi.write(0x00);
        break;
        
        case RADIO_WRITE_ACCESS|RADIO_SINGLE_ACCESS:
            rf_spi.write(pData[0]);
        break;
        
    }
    rf_cs = 1;
	rf_spi.unlock();
    readValue = pData[0];
    
	/* return the status byte value */
	return(readValue);
}

/******************************************************************************
 * @fn          trx16BitRegAccess
 *
 * @brief       This function performs a read or write in the extended adress
 *              space of CC112X.
 *
 * input parameters
 *
 * @param       accessType - Specifies if this is a read or write and if it's
 *                           a single or burst access. Bitmask made up of
 *                           RADIO_BURST_ACCESS/RADIO_SINGLE_ACCESS/
 *                           RADIO_WRITE_ACCESS/RADIO_READ_ACCESS.
 * @param       extAddr - Extended register space address = 0x2F.
 * @param       regAddr - Register address in the extended address space.
 * @param       *pData  - Pointer to data array for communication
 * @param       len     - Length of bytes to be read/written from/to radio
 *
 * output parameters
 *
 * @return      rfStatus_t
 */
rfStatus_t trx16BitRegAccess(uint8 accessType, uint8 extAddr, uint8 regAddr, uint8 *pData, uint8 len)
{
	uint8 readValue;
    int i = 0;
	/* Pull CS_N low and wait for SO to go low before communication starts */
	rf_spi.lock();
    rf_cs = 0;
	rf_spi.write(accessType|extAddr);
    rf_spi.write(regAddr);
    switch(accessType) {
        case RADIO_READ_ACCESS|RADIO_BURST_ACCESS:
            for(i = 0 ; i < len ; i++) {
                pData[i] = rf_spi.write(0x00);
            }
        break;
        
        case RADIO_WRITE_ACCESS|RADIO_BURST_ACCESS:
            for(i = 0 ; i < len ; i++) {
                rf_spi.write(pData[i]);
            }
        break;
        
        case RADIO_READ_ACCESS|RADIO_SINGLE_ACCESS:
            pData[0] = rf_spi.write(0x00);
        break;
        
        case RADIO_WRITE_ACCESS|RADIO_SINGLE_ACCESS:
            rf_spi.write(pData[0]);
        break;
        
    }
    rf_cs = 1;
	rf_spi.unlock();
    readValue = pData[0];
    
	/* return the status byte value */
	return(readValue);
}

/*******************************************************************************
 * @fn          trxSpiCmdStrobe
 *
 * @brief       Send command strobe to the radio. Returns status byte read
 *              during transfer of command strobe. Validation of provided
 *              is not done. Function assumes chip is ready.
 *
 * input parameters
 *
 * @param       cmd - command strobe
 *
 * output parameters
 *
 * @return      status byte
 */
rfStatus_t trxSpiCmdStrobe(uint8 cmd)
{
    uint8 rc;
	rf_spi.lock();
    rf_cs = 0;
	rf_spi.write(cmd);
    rc = rf_spi.write(0x00);
    rf_cs = 1;
	rf_spi.unlock();
	/* return the status byte value */
	return(rc);
}

/*******************************************************************************
 * @fn          trxReadWriteBurstSingle
 *
 * @brief       When the address byte is sent to the SPI slave, the next byte
 *              communicated is the data to be written or read. The address
 *              byte that holds information about read/write -and single/
 *              burst-access is provided to this function.
 *
 *              Depending on these two bits this function will write len bytes to
 *              the radio in burst mode or read len bytes from the radio in burst
 *              mode if the burst bit is set. If the burst bit is not set, only
 *              one data byte is communicated.
 *
 *              NOTE: This function is used in the following way:
 *
 *              RF_SPI_BEGIN();
 *              while(RF_PORT_IN & RF_SPI_MISO_PIN);
 *              ...[Depending on type of register access]
 *              trxReadWriteBurstSingle(uint8 addr,uint8 *pData,uint16 len);
 *              RF_SPI_END();
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
