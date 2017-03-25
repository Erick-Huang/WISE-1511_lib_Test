#include "mbed.h"
#include "main.h"
#include "IoTest.h"
#include "RfTest.h"
#include "simpleCli.h"

DigitalOut power(CB_PWR_ON);
Serial pc(SERIAL_TX, SERIAL_RX);

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main() {
    power = 1;
      
    pc.baud(115200);
    pc.printf("Hello World !!!!\r\n");
    
    RFInit();
    
    simple_cli_loop(&pc);
    // Enter runTX, never coming back
}


int Main_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
	simple_cmds cmds[] = {
			{ AP_NAME/*title*/,  		                    NULL,NULL },
            { "IO Test",                                    0,IOTest_Menu},
			{ "RF Test",				                    0,RFTest_Menu },
			{ NULL,											NULL,NULL }
	};
	return simple_cli_list(cmds);
}
