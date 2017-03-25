#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "GPIOTest.h"


DigitalOut gpio1(GPIO1);
DigitalOut gpio2(GPIO2);
DigitalOut gpio3(GPIO3);
DigitalOut gpio4(GPIO4);
DigitalOut gpio5(GPIO5);
DigitalOut gpio6(GPIO6);
DigitalOut gpio7(GPIO7);
DigitalOut gpio8(GPIO8);
    
DigitalOut *GPIO_switch(int index) {
    switch(index) {
        case 1:
            return &gpio1;
        case 2:
            return &gpio2;
        case 3:
            return &gpio3;
        case 4:
            return &gpio4;
        case 5:
            return &gpio5;
        case 6:
            return &gpio6;
        case 7:
            return &gpio7;
        case 8:
            return &gpio8;
    }
}
    
int GPIO_Read(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) {
        int value = *GPIO_switch(index);
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len," (%d)",value);
		return 0;
	}
    return 0;
}

int GPIO_High(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    *GPIO_switch(index) = 1;
    return 0;
}

int GPIO_Low(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    *GPIO_switch(index) = 0;
    return 0;
}

int GPIO_Menu(int index, char *string, int len, void *printer) {
    if(string != NULL && len != 0) return 0;
    simple_cmds cmds[] = {
            { "GPIO Function" /*title*/,  					NULL,NULL },
            { "Read",								        index,GPIO_Read },
            { "High",								        index,GPIO_High },
            { "Low",								        index,GPIO_Low },
            { NULL,										    NULL,NULL }
    };
	return simple_cli_list(cmds);
}

int GPIOTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;

    gpio1 = 0;
    gpio2 = 0;
    gpio3 = 0;
    gpio4 = 0;
    gpio5 = 0;
    gpio6 = 0;
    gpio7 = 0;
    gpio8 = 0;
    
	simple_cmds cmds[] = {
				{ "GPIO Test" /*title*/,  					NULL,NULL },
				{ "GPIO_1",								    1,GPIO_Menu },
                { "GPIO_2",								    2,GPIO_Menu },
                { "GPIO_3",								    3,GPIO_Menu },
                { "GPIO_4",								    4,GPIO_Menu }, 
                { "GPIO_5",								    5,GPIO_Menu }, 
                { "GPIO_6",								    6,GPIO_Menu }, 
                { "GPIO_7",								    7,GPIO_Menu },
                { "GPIO_8",								    8,GPIO_Menu }, 
				{ NULL,										NULL,NULL }
    };
	return simple_cli_list(cmds);
}

