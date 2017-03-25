#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "I2CTest.h"

#define console_printf ((Serial *)printer)->printf


//HDC1050 Slave
#define TI_HDC1050_DEVICE_ADDR  0x40

//HDC1050 Offset
#define TI_HDC1050_MANUFACTURER_ID_ADDR	0xFE

static int gBitRate = 0;
static unsigned char gSlaveAddress = TI_HDC1050_DEVICE_ADDR;
static unsigned char gOffsetAddress = TI_HDC1050_MANUFACTURER_ID_ADDR;

I2C i2c0(I2C_SDA,I2C_SCL);

static int BitRate(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (%dKHz)",gBitRate == 0 ? 100 : 400);
		return 0;
	}
	console_printf("\r\n*) Please input the bitrate ( 0: 100KHz, 1: 400KHz ): \r\n");
	gBitRate = GetInt_wait();
    
    if(gBitRate < 0 || gBitRate > 1) gBitRate = 0;
	return 0;
}

static int SlaveAddress(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (0x%02X)",gSlaveAddress);
		return 0;
	}
	console_printf("\r\n*) Please input the slave address ( 0x00 ~ 0xFF ): \r\n");
	gSlaveAddress = (unsigned char)(GetInt_wait()&0xFF);
	return 0;
}


static int OffsetAddress(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (0x%02X)",gOffsetAddress);
		return 0;
	}
	console_printf("\r\n*) Please input the offset address ( 0x00 ~ 0xFF ): \r\n");
	gOffsetAddress = (unsigned char)(GetInt_wait()&0xFF);
	return 0;
}

static void I2C_Read(I2C *i2c, char slave, char offset, char *rx, int rxlen) {
    i2c->write(slave<<1, &offset, 1);
    i2c->read(slave<<1, rx, rxlen);
}

static int Read(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    I2C *i2c = &i2c0;
    switch(gBitRate) {
        case 1:
            i2c->frequency(400000);
        break;
        default:
            i2c->frequency(100000);
        break;
    }
    char buffer[8] = {0};
    console_printf("\r\n");
    console_printf("Read from 0x%02X--%02X\r\n",gSlaveAddress, gOffsetAddress);
    I2C_Read(i2c, gSlaveAddress, gOffsetAddress, buffer, 8);
	console_printf("Result: 0x%02X%02X 0x%02X%02X 0x%02X%02X 0x%02X%02X\r\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
	console_printf("\r\n");
	return true;
}

int I2CTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	simple_cmds cmds[] = {
            { "I2C Test" /*title*/,  					NULL, NULL },
            { "BitRate",							    index, BitRate },
            { "Slave Address",							index, SlaveAddress },
            { "Offset Address",							index, OffsetAddress },
            { "Read",			                        index, Read },
            { NULL,										NULL, NULL }
    };
	return simple_cli_list(cmds);
}

