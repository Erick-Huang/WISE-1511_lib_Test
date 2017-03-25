#include "mbed.h"
#include "sigfox_api.h"

Serial pc(SERIAL_TX, SERIAL_RX);

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main() {
    int i = 1;
	sfx_u8 *ptr;
	sfx_u8 size;    
    
    pc.baud(115200);
	SIGFOX_API_get_version( &ptr, &size);
    
    while(1) { 
        ;
    }
}

