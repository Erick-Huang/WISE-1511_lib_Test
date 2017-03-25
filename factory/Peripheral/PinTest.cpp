#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "PinTest.h"

enum {
    PIN_GROUP_A = 65,
    PIN_GROUP_B,
    PIN_GROUP_C,
    PIN_GROUP_D,
    PIN_GROUP_E
};

#define console_printf ((Serial *)printer)->printf

static unsigned char gPinGroup = PIN_GROUP_A;
static unsigned char gPinIndex = 0;

static int InputPinName(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (P%c_%d)",gPinGroup, gPinIndex);
		return 0;
	}
	console_printf("\r\n*) Please input the pin group( A - E ): \r\n");
	gPinGroup = toupper((char)GetChar_wait());
    
    if(gPinGroup < 'A' || gPinGroup > 'E') gPinGroup = 'A';
    
	console_printf("\r\n*) Please input the pin number( 0 - 15 ): \r\n");
	gPinIndex = (unsigned char)GetInt_wait();
    if(gPinIndex < (unsigned char)0 || gPinIndex > (unsigned char)15) gPinIndex = 0;

	return 0;
}

static int PullHigh(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    int pin = 0x0;
    switch(gPinGroup) {
        case PIN_GROUP_A:
            pin |= 0x00;
        break;
        case PIN_GROUP_B:
            pin |= 0x10;
        break;
        case PIN_GROUP_C:
            pin |= 0x20;
        break;
        case PIN_GROUP_D:
            pin |= 0x30;
        break;
        case PIN_GROUP_E:
            pin |= 0x40;
        break;
    }
    
    pin |= gPinIndex;
    
    
    DigitalOut gpo((PinName)pin);
    gpo = 1;
    
	return 0;
}

static int PullLow(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	int pin = 0x0;
    switch(gPinGroup) {
        case PIN_GROUP_A:
            pin |= 0x00;
        break;
        case PIN_GROUP_B:
            pin |= 0x10;
        break;
        case PIN_GROUP_C:
            pin |= 0x20;
        break;
        case PIN_GROUP_D:
            pin |= 0x30;
        break;
        case PIN_GROUP_E:
            pin |= 0x40;
        break;
    }
    
    pin |= gPinIndex;
    
    
    DigitalOut gpo((PinName)pin);
    gpo = 0;
    
	return 0;
}

static int ListAllPin(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
	int i = 0, j = 0;
	console_printf("\r\n");
	for(i = 'A' ; i <= 'E' ; i++) {
        for(j = 0 ; j <= 15 ; j++) {
            console_printf("\t* P%c_%d\r\n",i,j);
        }
	}
	console_printf("\r\n");
	return true;
}

int PinTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;

	simple_cmds cmds[] = {
				{ "Pin Test" /*title*/,  					NULL,NULL },
				{ "Pin Name",								index,InputPinName },
				{ "Pull High",								index,PullHigh },
				{ "Pull Low",								index,PullLow },
				{ "List All Supported Pin Number",			index,ListAllPin },
				{ NULL,										NULL,NULL }
		};
	return simple_cli_list(cmds);
}

