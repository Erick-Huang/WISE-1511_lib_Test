#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "GPIOTest.h"
#include "PinTest.h"
#include "I2CTest.h"
#include "SPITest.h"
#include "PWMADCTest.h"
#include "IoTest.h"




int IOTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;

	simple_cmds cmds[] = {
			{ "I/O Function Test" /*title*/,          		0,NULL },
            { "GPIO",                                       0,GPIOTest_Menu},
			{ "I2C",										0,I2CTest_Menu },
			{ "SPI_CS0",									0,SPITest_Menu },
            { "SPI_CS1",									1,SPITest_Menu },
//          { "SPI_RF",									    2,SPITest_Menu },
			{ "PWM",										0,PWMTest_Menu },
			{ "ADC",										0,ADCTest_Menu },
//			{ "Watchdog",									0,NULL },
			{ "Pin Test",									0,PinTest_Menu },
			{ NULL,											0,NULL }
	};
	return simple_cli_list(cmds);
}