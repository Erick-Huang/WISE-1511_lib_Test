#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "SPITest.h"

#define console_printf ((Serial *)printer)->printf

//HDC1050 Slave
#define N25Q032A_CMD_READ_ID        0x9F// 0x9E/0x9F

//HDC1050 Offset
#define N25Q032A_MANUFACTURER_ID    0x20//	Byte 1 Manufacturer ID

#define SPI_MAX_DEVICES 3

SPI spi1(SPI_MOSI, SPI_MISO, SPI_SCK); // 0, 1
DigitalOut cs0(SPI_CS0);
DigitalOut cs1(SPI_CS1);

SPI spirf(SPI_RF_MOSI, SPI_RF_MISO, SPI_RF_SCK); //2
DigitalOut csrf(SPI_RF_CS);

static int gBitRate[SPI_MAX_DEVICES] = {0};
static char gOffsetAddress[SPI_MAX_DEVICES] = {N25Q032A_CMD_READ_ID,N25Q032A_CMD_READ_ID,0x8F};

static int BitRate(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len, gBitRate[index] == 0 ? " (1MHz)" : " (100KHz)");
		return 0;
	}
	console_printf("\r\n*) Please input the bitrate ( 0: 1MHz , 1: 100KHz ): \r\n");
	gBitRate[index] = GetInt_wait();
    
    
    if(gBitRate[index] < 0 || gBitRate[index] > 1) gBitRate[index] = 0;
	return 0;
}

static int OffsetAddress(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (0x%02X)",gOffsetAddress[index]);
		return 0;
	}
	console_printf("\r\n*) Please input the offset address ( 0x00 ~ 0xFF ): \r\n");
	gOffsetAddress[index] = (char)(GetInt_wait()&0xFF);
    
    if(gOffsetAddress[index] < 0 || gOffsetAddress[index] > 0xFF) gOffsetAddress[index] = N25Q032A_CMD_READ_ID;
	return 0;
}

static volatile bool complete = false;
static Mutex spi_mutex;
void ReadComplete(int event) {
    //spi_mutex.unlock();
    if ((event & SPI_EVENT_COMPLETE) != SPI_EVENT_COMPLETE) {
        printf("Unexpected event=0x%8X\r\n", event);
    }
    complete = true;
}

static int Read(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    SPI *spi;
    DigitalOut *cs;
    
    switch(index) {
        case 0:
            spi = &spi1;
            cs = &cs0;
        break;
        
        case 1:
            spi = &spi1;
            cs = &cs1;
        break;
        
        case 2:
            spi = &spirf;
            cs = &csrf;
        break;
    }
    
    spi->format(8,0);
    switch(gBitRate[index]) {
        case 1:
            spi->frequency(100000);
        break;
        default:
            spi->frequency(1000000);
        break;
    }
    char tx[5] = {0};
    char rx[5] = {0};
    spi->lock();
    
    *cs = 0;
    
    spi->write(gOffsetAddress[index]);
    rx[0] = spi->write(0x00);
    rx[1] = spi->write(0x00);
    rx[2] = spi->write(0x00);
    rx[3] = spi->write(0x00);

    *cs = 1;
    
    spi->unlock();
    
	console_printf("\r\n");
	console_printf("Result: 0x%02X 0x%02X 0x%02X 0x%02X\n",rx[0],rx[1],rx[2],rx[3]);
	console_printf("\r\n");
    
    
	return true;
}

static int Read_1(int index, char *string, int len, void *printer);
static int Read_2(int index, char *string, int len, void *printer);
static int Read_3(int index, char *string, int len, void *printer);
static int Read_RF_ID(int index, char *string, int len, void *printer);

int SPITest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    if(index < 0 || index >= SPI_MAX_DEVICES) return 0;
    cs0 = 1;
    cs1 = 1;
    csrf = 1;
        
    char *spi_name = NULL;
    switch(index) {
        case 0:
            spi_name = "SPI_CS0 test";
        break;
        case 1:
            spi_name = "SPI_CS1 test";
        break;
        case 2:
            spi_name = "SPI_RF test";
        break;
    }
    
#if 1
    if(index < 2) {
        simple_cmds cmds[] = {
            { spi_name /*title*/,  					NULL,NULL },
            { "BitRate",							    index,BitRate },
            { "Offset Address",							index,OffsetAddress },
            { "Read",			                        index,Read },
            //{ "Read_1",			                        index,Read_1 },
            //{ "Read_2",			                        index,Read_2 },
            //{ "Read_3",			                        index,Read_3 },
            { NULL,										NULL,NULL }
        };
        return simple_cli_list(cmds);
    } else 
#endif
    {
        simple_cmds cmds[] = {
            { spi_name /*title*/,  					NULL,NULL },
            { "BitRate",							    index,BitRate },
            { "Offset Address",							index,OffsetAddress },
            { "Read",			                        index,Read },
            { "Read_RF_ID",			                    index,Read_RF_ID },
            { NULL,										NULL,NULL }
        };
        return simple_cli_list(cmds);
    }
}

static int Read_1(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    SPI *spi;
    DigitalOut *cs;

    switch(index) {
        case 0:
            spi = &spi1;
            cs = &cs0;
        break;
        
        case 1:
            spi = &spi1;
            cs = &cs1;
        break;
        
        case 2:
            spi = &spirf;
            cs = &csrf;
        break;
    }
    
    spi->format(8,0);
    switch(gBitRate[index]) {
        case 1:
            spi->frequency(100000);
        break;
        default:
            spi->frequency(1000000);
        break;
    }
    char tx[5] = {0};
    char rx[5] = {0};
    spi->lock();
    
    *cs = 0;
    
    spi->write(gOffsetAddress[index]);
    rx[0] = spi->write(0x00);
    rx[1] = spi->write(0x00);
    rx[2] = spi->write(0x00);
    rx[3] = spi->write(0x00);
    
    /*tx[0] = gOffsetAddress[index];
    spi->transfer(tx, 5, rx, 5, ReadComplete);
    spi_mutex.lock();
    spi_mutex.lock();
    spi_mutex.unlock();*/
    
    
    /*tx[0] = gOffsetAddress[index];
    spi->transfer(tx, 1, rx, 1, ReadComplete);
    spi_mutex.lock();
    spi_mutex.lock();
    spi_mutex.unlock();
    
    tx[0] = 0;
    spi->transfer(tx, 4, rx, 4, ReadComplete);
    spi_mutex.lock();
    spi_mutex.lock();
    spi_mutex.unlock();*/
    
    *cs = 1;
    
    spi->unlock();
    
	console_printf("\r\n");
	console_printf("Result: 0x%08X 0x%08X 0x%08X 0x%08X\r\n",rx[0],rx[1],rx[2],rx[3]);
	console_printf("\r\n");
    
    
	return true;
}

static int Read_2(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    SPI *spi;
    DigitalOut *cs;

    switch(index) {
        case 0:
            spi = &spi1;
            cs = &cs0;
        break;
        
        case 1:
            spi = &spi1;
            cs = &cs1;
        break;
        
        case 2:
            spi = &spirf;
            cs = &csrf;
        break;
    }
    
    spi->format(8,0);
    switch(gBitRate[index]) {
        case 1:
            spi->frequency(100000);
        break;
        default:
            spi->frequency(1000000);
        break;
    }
    char tx[5] = {0};
    char rx[5] = {0};
    spi->lock();
    
    *cs = 0;
    
    /*spi->write(gOffsetAddress[index]);
    rx[0] = spi->write(0x00);
    rx[1] = spi->write(0x00);
    rx[2] = spi->write(0x00);
    rx[3] = spi->write(0x00);*/
    
    event_callback_t callback;
    callback.attach(ReadComplete);
    
    tx[0] = gOffsetAddress[index];
    spi->transfer(tx, 5, rx, 5, ReadComplete, SPI_EVENT_ALL);
    while (complete != true) {
        wait_us(100);
    }
    
    /*tx[0] = gOffsetAddress[index];
    spi->transfer(tx, 1, rx, 1, ReadComplete);
    spi_mutex.lock();
    spi_mutex.lock();
    spi_mutex.unlock();
    
    tx[0] = 0;
    spi->transfer(tx, 4, rx, 4, ReadComplete);
    spi_mutex.lock();
    spi_mutex.lock();
    spi_mutex.unlock();*/
    
    *cs = 1;
    
    spi->unlock();
    
	console_printf("\r\n");
	console_printf("Result: 0x%08X 0x%08X 0x%08X 0x%08X\n",rx[0],rx[1],rx[2],rx[3]);
	console_printf("\r\n");
    
    
	return true;
}

static int Read_3(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    SPI *spi;
    DigitalOut *cs;
    
    switch(index) {
        case 0:
            spi = &spi1;
            cs = &cs0;
        break;
        
        case 1:
            spi = &spi1;
            cs = &cs1;
        break;
        
        case 2:
            spi = &spirf;
            cs = &csrf;
        break;
    }
    
    spi->format(8,0);
    switch(gBitRate[index]) {
        case 1:
            spi->frequency(100000);
        break;
        default:
            spi->frequency(1000000);
        break;
    }
    char tx[5] = {0};
    char rx[5] = {0};
    spi->lock();
    
    *cs = 0;
    
    /*spi->write(gOffsetAddress[index]);
    rx[0] = spi->write(0x00);
    rx[1] = spi->write(0x00);
    rx[2] = spi->write(0x00);
    rx[3] = spi->write(0x00);*/
    
    /*tx[0] = gOffsetAddress[index];
    spi->transfer(tx, 5, rx, 5, ReadComplete);
    spi_mutex.lock();
    spi_mutex.lock();
    spi_mutex.unlock();*/
    
    
    tx[0] = gOffsetAddress[index];
    spi->transfer(tx, 1, rx, 1, ReadComplete);
    spi_mutex.lock();
    spi_mutex.lock();
    spi_mutex.unlock();
    
    tx[0] = 0;
    spi->transfer(tx, 4, rx, 4, ReadComplete);
    spi_mutex.lock();
    spi_mutex.lock();
    spi_mutex.unlock();
    
    *cs = 1;
    
    spi->unlock();
    
	console_printf("\r\n");
	console_printf("Result: 0x%08X 0x%08X 0x%08X 0x%08X\n",rx[0],rx[1],rx[2],rx[3]);
	console_printf("\r\n");
    
    
	return true;
}


typedef struct
{
  unsigned short  addr;
  unsigned char   data;
}registerSetting_t;

//
// Rf settings for CC1120
//
#define IOCFG3            0x0000      //GPIO3 IO Pin Configuration
#define IOCFG2            0x0001      //GPIO2 IO Pin Configuration
#define IOCFG1            0x0002      //GPIO1 IO Pin Configuration
#define IOCFG0            0x0003      //GPIO0 IO Pin Configuration
#define SYNC3             0x0004      //Sync Word Configuration [31:24]
#define SYNC2             0x0005      //Sync Word Configuration [23:16]
#define SYNC1             0x0006      //Sync Word Configuration [15:8]
#define SYNC0             0x0007      //Sync Word Configuration [7:0]
#define SYNC_CFG1         0x0008      //Sync Word Detection Configuration Reg. 1
#define SYNC_CFG0         0x0009      //Sync Word Length Configuration Reg. 0
#define DEVIATION_M       0x000A      //Frequency Deviation Configuration
#define MODCFG_DEV_E      0x000B      //Modulation Format and Frequency Deviation Configur..
#define DCFILT_CFG        0x000C      //Digital DC Removal Configuration
#define PREAMBLE_CFG1     0x000D      //Preamble Length Configuration Reg. 1
#define PREAMBLE_CFG0     0x000E      //Preamble Detection Configuration Reg. 0
#define FREQ_IF_CFG       0x000F      //RX Mixer Frequency Configuration
#define IQIC              0x0010      //Digital Image Channel Compensation Configuration
#define CHAN_BW           0x0011      //Channel Filter Configuration
#define MDMCFG1           0x0012      //General Modem Parameter Configuration Reg. 1
#define MDMCFG0           0x0013      //General Modem Parameter Configuration Reg. 0
#define SYMBOL_RATE2      0x0014      //Symbol Rate Configuration Exponent and Mantissa [1..
#define SYMBOL_RATE1      0x0015      //Symbol Rate Configuration Mantissa [15:8]
#define SYMBOL_RATE0      0x0016      //Symbol Rate Configuration Mantissa [7:0]
#define AGC_REF           0x0017      //AGC Reference Level Configuration
#define AGC_CS_THR        0x0018      //Carrier Sense Threshold Configuration
#define AGC_GAIN_ADJUST   0x0019      //RSSI Offset Configuration
#define AGC_CFG3          0x001A      //Automatic Gain Control Configuration Reg. 3
#define AGC_CFG2          0x001B      //Automatic Gain Control Configuration Reg. 2
#define AGC_CFG1          0x001C      //Automatic Gain Control Configuration Reg. 1
#define AGC_CFG0          0x001D      //Automatic Gain Control Configuration Reg. 0
#define FIFO_CFG          0x001E      //FIFO Configuration
#define DEV_ADDR          0x001F      //Device Address Configuration
#define SETTLING_CFG      0x0020      //Frequency Synthesizer Calibration and Settling Con..
#define FS_CFG            0x0021      //Frequency Synthesizer Configuration
#define WOR_CFG1          0x0022      //eWOR Configuration Reg. 1
#define WOR_CFG0          0x0023      //eWOR Configuration Reg. 0
#define WOR_EVENT0_MSB    0x0024      //Event 0 Configuration MSB
#define WOR_EVENT0_LSB    0x0025      //Event 0 Configuration LSB
#define PKT_CFG2          0x0026      //Packet Configuration Reg. 2
#define PKT_CFG1          0x0027      //Packet Configuration Reg. 1
#define PKT_CFG0          0x0028      //Packet Configuration Reg. 0
#define RFEND_CFG1        0x0029      //RFEND Configuration Reg. 1
#define RFEND_CFG0        0x002A      //RFEND Configuration Reg. 0
#define PA_CFG2           0x002B      //Power Amplifier Configuration Reg. 2
#define PA_CFG1           0x002C      //Power Amplifier Configuration Reg. 1
#define PA_CFG0           0x002D      //Power Amplifier Configuration Reg. 0
#define PKT_LEN           0x002E      //Packet Length Configuration
#define IF_MIX_CFG        0x2F00      //IF Mix Configuration
#define FREQOFF_CFG       0x2F01      //Frequency Offset Correction Configuration
#define TOC_CFG           0x2F02      //Timing Offset Correction Configuration
#define MARC_SPARE        0x2F03      //MARC Spare
#define ECG_CFG           0x2F04      //External Clock Frequency Configuration
#define CFM_DATA_CFG      0x2F05      //Custom frequency modulation enable
#define EXT_CTRL          0x2F06      //External Control Configuration
#define RCCAL_FINE        0x2F07      //RC Oscillator Calibration Fine
#define RCCAL_COARSE      0x2F08      //RC Oscillator Calibration Coarse
#define RCCAL_OFFSET      0x2F09      //RC Oscillator Calibration Clock Offset
#define FREQOFF1          0x2F0A      //Frequency Offset MSB
#define FREQOFF0          0x2F0B      //Frequency Offset LSB
#define FREQ2             0x2F0C      //Frequency Configuration [23:16]
#define FREQ1             0x2F0D      //Frequency Configuration [15:8]
#define FREQ0             0x2F0E      //Frequency Configuration [7:0]
#define IF_ADC2           0x2F0F      //Analog to Digital Converter Configuration Reg. 2
#define IF_ADC1           0x2F10      //Analog to Digital Converter Configuration Reg. 1
#define IF_ADC0           0x2F11      //Analog to Digital Converter Configuration Reg. 0
#define FS_DIG1           0x2F12      //Frequency Synthesizer Digital Reg. 1
#define FS_DIG0           0x2F13      //Frequency Synthesizer Digital Reg. 0
#define FS_CAL3           0x2F14      //Frequency Synthesizer Calibration Reg. 3
#define FS_CAL2           0x2F15      //Frequency Synthesizer Calibration Reg. 2
#define FS_CAL1           0x2F16      //Frequency Synthesizer Calibration Reg. 1
#define FS_CAL0           0x2F17      //Frequency Synthesizer Calibration Reg. 0
#define FS_CHP            0x2F18      //Frequency Synthesizer Charge Pump Configuration
#define FS_DIVTWO         0x2F19      //Frequency Synthesizer Divide by 2
#define FS_DSM1           0x2F1A      //FS Digital Synthesizer Module Configuration Reg. 1
#define FS_DSM0           0x2F1B      //FS Digital Synthesizer Module Configuration Reg. 0
#define FS_DVC1           0x2F1C      //Frequency Synthesizer Divider Chain Configuration ..
#define FS_DVC0           0x2F1D      //Frequency Synthesizer Divider Chain Configuration ..
#define FS_LBI            0x2F1E      //Frequency Synthesizer Local Bias Configuration
#define FS_PFD            0x2F1F      //Frequency Synthesizer Phase Frequency Detector Con..
#define FS_PRE            0x2F20      //Frequency Synthesizer Prescaler Configuration
#define FS_REG_DIV_CML    0x2F21      //Frequency Synthesizer Divider Regulator Configurat..
#define FS_SPARE          0x2F22      //Frequency Synthesizer Spare
#define FS_VCO4           0x2F23      //FS Voltage Controlled Oscillator Configuration Reg..
#define FS_VCO3           0x2F24      //FS Voltage Controlled Oscillator Configuration Reg..
#define FS_VCO2           0x2F25      //FS Voltage Controlled Oscillator Configuration Reg..
#define FS_VCO1           0x2F26      //FS Voltage Controlled Oscillator Configuration Reg..
#define FS_VCO0           0x2F27      //FS Voltage Controlled Oscillator Configuration Reg..
#define GBIAS6            0x2F28      //Global Bias Configuration Reg. 6
#define GBIAS5            0x2F29      //Global Bias Configuration Reg. 5
#define GBIAS4            0x2F2A      //Global Bias Configuration Reg. 4
#define GBIAS3            0x2F2B      //Global Bias Configuration Reg. 3
#define GBIAS2            0x2F2C      //Global Bias Configuration Reg. 2
#define GBIAS1            0x2F2D      //Global Bias Configuration Reg. 1
#define GBIAS0            0x2F2E      //Global Bias Configuration Reg. 0
#define IFAMP             0x2F2F      //Intermediate Frequency Amplifier Configuration
#define LNA               0x2F30      //Low Noise Amplifier Configuration
#define RXMIX             0x2F31      //RX Mixer Configuration
#define XOSC5             0x2F32      //Crystal Oscillator Configuration Reg. 5
#define XOSC4             0x2F33      //Crystal Oscillator Configuration Reg. 4
#define XOSC3             0x2F34      //Crystal Oscillator Configuration Reg. 3
#define XOSC2             0x2F35      //Crystal Oscillator Configuration Reg. 2
#define XOSC1             0x2F36      //Crystal Oscillator Configuration Reg. 1
#define XOSC0             0x2F37      //Crystal Oscillator Configuration Reg. 0
#define ANALOG_SPARE      0x2F38      //Analog Spare
#define PA_CFG3           0x2F39      //Power Amplifier Configuration Reg. 3
#define WOR_TIME1         0x2F64      //eWOR Timer Counter Value MSB
#define WOR_TIME0         0x2F65      //eWOR Timer Counter Value LSB
#define WOR_CAPTURE1      0x2F66      //eWOR Timer Capture Value MSB
#define WOR_CAPTURE0      0x2F67      //eWOR Timer Capture Value LSB
#define BIST              0x2F68      //MARC Built-In Self-Test
#define DCFILTOFFSET_I1   0x2F69      //DC Filter Offset I MSB
#define DCFILTOFFSET_I0   0x2F6A      //DC Filter Offset I LSB
#define DCFILTOFFSET_Q1   0x2F6B      //DC Filter Offset Q MSB
#define DCFILTOFFSET_Q0   0x2F6C      //DC Filter Offset Q LSB
#define IQIE_I1           0x2F6D      //IQ Imbalance Value I MSB
#define IQIE_I0           0x2F6E      //IQ Imbalance Value I LSB
#define IQIE_Q1           0x2F6F      //IQ Imbalance Value Q MSB
#define IQIE_Q0           0x2F70      //IQ Imbalance Value Q LSB
#define RSSI1             0x2F71      //Received Signal Strength Indicator Reg. 1
#define RSSI0             0x2F72      //Received Signal Strength Indicator Reg.0
#define MARCSTATE         0x2F73      //MARC State
#define LQI_VAL           0x2F74      //Link Quality Indicator Value
#define PQT_SYNC_ERR      0x2F75      //Preamble and Sync Word Error
#define DEM_STATUS        0x2F76      //Demodulator Status
#define FREQOFF_EST1      0x2F77      //Frequency Offset Estimate MSB
#define FREQOFF_EST0      0x2F78      //Frequency Offset Estimate LSB
#define AGC_GAIN3         0x2F79      //Automatic Gain Control Reg. 3
#define AGC_GAIN2         0x2F7A      //Automatic Gain Control Reg. 2
#define AGC_GAIN1         0x2F7B      //Automatic Gain Control Reg. 1
#define AGC_GAIN0         0x2F7C      //Automatic Gain Control Reg. 0
#define CFM_RX_DATA_OUT   0x2F7D      //Custom Frequency Modulation RX Data
#define CFM_TX_DATA_IN    0x2F7E      //Custom Frequency Modulation TX Data
#define ASK_SOFT_RX_DATA  0x2F7F      //ASK Soft Decision Output
#define RNDGEN            0x2F80      //Random Number Generator Value
#define MAGN2             0x2F81      //Signal Magnitude after CORDIC [16]
#define MAGN1             0x2F82      //Signal Magnitude after CORDIC [15:8]
#define MAGN0             0x2F83      //Signal Magnitude after CORDIC [7:0]
#define ANG1              0x2F84      //Signal Angular after CORDIC [9:8]
#define ANG0              0x2F85      //Signal Angular after CORDIC [7:0]
#define CHFILT_I2         0x2F86      //Channel Filter Data Real Part [18:16]
#define CHFILT_I1         0x2F87      //Channel Filter Data Real Part [15:8]
#define CHFILT_I0         0x2F88      //Channel Filter Data Real Part [7:0]
#define CHFILT_Q2         0x2F89      //Channel Filter Data Imaginary Part [18:16]
#define CHFILT_Q1         0x2F8A      //Channel Filter Data Imaginary Part [15:8]
#define CHFILT_Q0         0x2F8B      //Channel Filter Data Imaginary Part [7:0]
#define GPIO_STATUS       0x2F8C      //General Purpose Input/Output Status
#define FSCAL_CTRL        0x2F8D      //Frequency Synthesizer Calibration Control
#define PHASE_ADJUST      0x2F8E      //Frequency Synthesizer Phase Adjust
#define PARTNUMBER        0x2F8F      //Part Number
#define PARTVERSION       0x2F90      //Part Revision
#define SERIAL_STATUS     0x2F91      //Serial Status
#define MODEM_STATUS1     0x2F92      //Modem Status Reg. 1
#define MODEM_STATUS0     0x2F93      //Modem Status Reg. 0
#define MARC_STATUS1      0x2F94      //MARC Status Reg. 1
#define MARC_STATUS0      0x2F95      //MARC Status Reg. 0
#define PA_IFAMP_TEST     0x2F96      //Power Amplifier Intermediate Frequency Amplifier T..
#define FSRF_TEST         0x2F97      //Frequency Synthesizer Test
#define PRE_TEST          0x2F98      //Frequency Synthesizer Prescaler Test
#define PRE_OVR           0x2F99      //Frequency Synthesizer Prescaler Override
#define ADC_TEST          0x2F9A      //Analog to Digital Converter Test
#define DVC_TEST          0x2F9B      //Digital Divider Chain Test
#define ATEST             0x2F9C      //Analog Test
#define ATEST_LVDS        0x2F9D      //Analog Test LVDS
#define ATEST_MODE        0x2F9E      //Analog Test Mode
#define XOSC_TEST1        0x2F9F      //Crystal Oscillator Test Reg. 1
#define XOSC_TEST0        0x2FA0      //Crystal Oscillator Test Reg. 0
#define RXFIRST           0x2FD2      //RX FIFO Pointer First Entry
#define TXFIRST           0x2FD3      //TX FIFO Pointer First Entry
#define RXLAST            0x2FD4      //RX FIFO Pointer Last Entry
#define TXLAST            0x2FD5      //TX FIFO Pointer Last Entry
#define NUM_TXBYTES       0x2FD6      //TX FIFO Status
#define NUM_RXBYTES       0x2FD7      //RX FIFO Status
#define FIFO_NUM_TXBYTES  0x2FD8      //TX FIFO Status
#define FIFO_NUM_RXBYTES  0x2FD9      //RX FIFO Status

// Strobe commands
#define SRES              0x30        // Reset chip.
#define SFSTXON           0x31        // Enable/calibrate freq synthesizer
#define SXOFF             0x32        // Turn off crystal oscillator.
#define SCAL              0x33        // Calibrate freq synthesizer & disable
#define SRX               0x34        // Enable RX.
#define STX               0x35        // Enable TX.
#define SIDLE             0x36        // Exit RX / TX
#define SWOR              0x38        // Start automatic RX polling sequence
#define SPWD              0x39        // Enter pwr down mode when CSn goes hi
#define SFRX              0x3A        // Flush the RX FIFO buffer.
#define SFTX              0x3B        // Flush the TX FIFO buffer.
#define SWORRST           0x3C        // Reset real time clock.
#define SNOP              0x3D        // No operation.

// Other memory locations
#define PATABLE           0x3E
#define TXFIFO            0x3F
#define RXFIFO            0x3F

// Masks for appended status bytes
#define RSSI_RX           0x00        // Position of RSSI byte
#define LQI_RX            0x01        // Position of LQI byte
#define CRC_OK            0x80        // Mask "CRC_OK" bit within LQI byte
#define MARCSTATE_IDLE    0x01        // The status register indicates idle

// Definitions to support burst/single access:
#define WRITE_BURST       0x40
#define READ_SINGLE       0x80
#define READ_BURST        0xC0


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


static int Read_RF_ID(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    SPI *spi;
    DigitalOut *cs;
    
    unsigned char i, writeByte, preferredSettings_length = sizeof(preferredSettings_1200bps)/sizeof(registerSetting_t);
	unsigned int bit_rate;
	registerSetting_t *preferredSettings = (registerSetting_t *)preferredSettings_1200bps;
    
    switch(index) {
        case 0:
            spi = &spi1;
            cs = &cs0;
        break;
        
        case 1:
            spi = &spi1;
            cs = &cs1;
        break;
        
        case 2:
            spi = &spirf;
            cs = &csrf;
        break;
    }
    
    spi->format(8,0);
    switch(gBitRate[index]) {
        case 1:
            spi->frequency(100000);
        break;
        default:
            spi->frequency(1000000);
        break;
    }
    char tx[5] = {0};
    char rx[5] = {0};
    spi->lock();
    
    *cs = 0;
    
    spi->write(SRES);
    wait_ms(100);
    
    
    for(i = 0; i < preferredSettings_length; i++) {
		if(preferredSettings[i].addr < 0x2F) {
			writeByte = preferredSettings[i].data;
            spi->write(preferredSettings[i].addr);
            spi->write(writeByte);
		} else {
			writeByte = preferredSettings[i].data;
            spi->write(0x2F);
            spi->write((0xFF & preferredSettings[i].addr));
            spi->write(writeByte);
		}
	}
    
    
    spi->write(0x2F);
    spi->write((0xFF & PARTNUMBER));
    rx[0] = spi->write(0x00);
     
    spi->write(0x2F);
    spi->write((0xFF & PARTVERSION));
    rx[1] = spi->write(0x00);
    
    *cs = 1;
    
    spi->unlock();
    
	console_printf("\r\n");
    console_printf("Write(index %d): 0x%08X 0x%08X\r\n",index, 0x2F,gOffsetAddress[index]);
	console_printf("Result: 0x%08X 0x%08X 0x%08X 0x%08X\r\n",rx[0],rx[1],rx[2],rx[3]);
	console_printf("\r\n");
    
    
	return true;
}