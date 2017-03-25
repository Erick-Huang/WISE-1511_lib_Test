#include <ctype.h>
#include "mbed.h"
#include "simpleCli.h"
#include "PWMADCTest.h"

#define console_printf ((Serial *)printer)->printf

//PWM
static PwmOut pwmled(PWM_OUT);
static int gPeriod = 10;
static float gDutyCycle = 0.5;

static int Period(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len, " (%dms)",gPeriod);
		return 0;
	}
	console_printf("\r\n*) Please input the period(ms): \r\n");
	gPeriod = GetInt_wait();
    
    
    if(gPeriod < 0) gPeriod = 10;
    
    pwmled.period_ms(gPeriod);
    pwmled.write(gDutyCycle);
	return 0;
}

static int DutyCycle(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) {
		snprintf(string, len > INBUF_SIZE ? INBUF_SIZE : len, " (%d%%)",(int)(gDutyCycle * 100));
		return 0;
	}
	console_printf("\r\n*) Please input the duty-cycle(%%): \r\n");
	gDutyCycle = GetInt_wait();
    if(gDutyCycle < 0) gDutyCycle = 50;
    gDutyCycle /= 100;
    
    pwmled.period_ms(gPeriod);
    pwmled.write(gDutyCycle);
    
	return 0;
}



int PWMTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	simple_cmds cmds[] = {
            { "PWM Test" /*title*/,  					NULL,NULL },
            { "Period",							        NULL,Period },
            { "Duty-Cycle",							    NULL,DutyCycle },
            { NULL,										NULL,NULL }
    };
    
    pwmled.period_ms(gPeriod);
    pwmled.write(gDutyCycle);
    
	return simple_cli_list(cmds);
}



//ADC

static AnalogIn adc0(ADC0);
static AnalogIn adc2(ADC2);
static AnalogIn adc3(ADC3);
static AnalogIn adc4(ADC4);

static int Read(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    AnalogIn *adc = NULL;
    switch(index) {
        case 0:
            adc = &adc0;
        break;
        
        case 2:
            adc = &adc2;
        break;
        
        case 3:
            adc = &adc3;
        break;
        
        case 4:
            adc = &adc4;
        break;
    }
    if(adc == NULL) return 0;
    
	console_printf("\r\n");
	console_printf("Result: %f\n",adc->read() * 3.3);
	console_printf("\r\n");
	return true;
}

int ADCTestByIndex(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	simple_cmds cmds[] = {
            { "ADC-IN Test" /*title*/,  				NULL,NULL },
            { "Read",							        index,Read },
            { NULL,										NULL,NULL }
    };
	return simple_cli_list(cmds);
}

int ADCTest_Menu(int index, char *string, int len, void *printer) {
	if(string != NULL && len != 0) return 0;
    
	simple_cmds cmds[] = {
            { "ADC list" /*title*/,                 NULL,NULL },
            { "ADC_0",                              0,ADCTestByIndex },
            { "ADC_2",                              2,ADCTestByIndex },
            { "ADC_3",                              3,ADCTestByIndex },
            { "ADC_4",                              4,ADCTestByIndex },
            { NULL,                                 NULL,NULL }
    };
	return simple_cli_list(cmds);
}

