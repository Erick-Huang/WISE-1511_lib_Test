#include <ctype.h>
#include <stdlib.h>
#include "mbed.h"
#include "main.h"
#include "simpleCli.h"

static Serial *serial = NULL;
#define usleep(msec) wait_ms(msec)
#define console_init() serial->baud(115200)
#define console_printf serial->printf

inline int console_getchar(uint8_t *c) {
    *c=serial->getc();
    return true;
}

#define console_clrscr() console_printf("\033[2J\033[0;0H")

#define END_CHAR		'\r'
enum {
	BASIC_KEY,
	EXT_KEY_FIRST_SYMBOL,
	EXT_KEY_SECOND_SYMBOL,
};
static int console_input(char *inbuf, unsigned int *words)
{
	static int state = BASIC_KEY;
	static char second_char;

	if (inbuf == NULL) {
		return 0;
	}



	while (console_getchar((uint8_t *)&inbuf[*words]) == 1) {
		if (state == EXT_KEY_SECOND_SYMBOL) {
			if (second_char == 0x4F) {
				if (inbuf[*words] == 0x4D) {
					/* Num. keypad ENTER */
					inbuf[*words] = '\0';
					*words = 0;
					state = BASIC_KEY;
					//console_printf("\r\n");
					return 1;
				}
			}
		}

		if (state == EXT_KEY_FIRST_SYMBOL) {
			second_char = inbuf[*words];
			if (inbuf[*words] == 0x4F) {
				state = EXT_KEY_SECOND_SYMBOL;
				continue;
			}
			if (inbuf[*words] == 0x5B) {
				state = EXT_KEY_SECOND_SYMBOL;
				continue;
			}
		}
		if (inbuf[*words] == 0x1B) {
			/* We may be seeing a first character from a
			   extended key */
			state = EXT_KEY_FIRST_SYMBOL;
			continue;
		}
		state = BASIC_KEY;


		if (inbuf[*words] == END_CHAR) {	/* end of input line */
			inbuf[*words] = '\0';
			*words = 0;
			//console_printf("\r\n");
			return 1;
		}

		if ((inbuf[*words] == 0x08) ||	/* backspace */
		    (inbuf[*words] == 0x7f)) {	/* DEL */
			if (*words > 0) {
				(*words)--;
				//if (!cli.echo_disabled)
					console_printf("%c %c", 0x08, 0x08);
			}
			continue;
		}

		/*if (inbuf[*words] == '\t') {
			inbuf[*words] = '\0';
			tab_complete(inbuf, words);
			continue;
		}*/

		//if (!cli.echo_disabled)
			console_printf("%c", inbuf[*words]);

		(*words)++;
		if (*words >= INBUF_SIZE) {
			console_printf("Error: input buffer overflow\r\n");
			//console_printf(PROMPT);
			*words = 0;
			return 0;
		}

	}

	return 0;
}

static void console_input_wait(char *inbuf, unsigned int *words) {
	do {
		usleep(200);
	} while(console_input(inbuf, words) == 0);
}

/****************************************************************************************/

static char buf[INBUF_SIZE];
static unsigned int buffersize = 0;

int GetStr_wait(char *inbuf, unsigned int *words) {
	do {
		usleep(200);
	} while(console_input(inbuf, words) == 0);
    return true;
}

char GetChar_wait() {
	do {
		usleep(200);
	} while(console_input(buf, &buffersize) == 0);
    return buf[0];
}

int GetInt_wait() {
	do {
		usleep(200);
	} while(console_input(buf, &buffersize) == 0);
	return strtol(buf, NULL, 0);
}



/****************************************************************************************/
int simple_cli_list(simple_cmds *cmds) {
	unsigned int i = 0;
	int cmdId = 0;
	int cmdcount = 0;
	char buffer[INBUF_SIZE];
	unsigned int buffersize = 0;
	do {
		cmdcount = 0;
		while(cmds[cmdcount].string != NULL) {
			switch(cmdcount) {
				case 0:
				{
					buffersize = strlen(cmds[0].string);
					console_clrscr();
					console_printf("==================");
					for(i = 0 ; i < buffersize ; i++) console_printf("=");
					console_printf("==================\r\n");
					console_printf("================[ %s ]================\r\n",cmds[0].string);
					console_printf("==================");
					for(i = 0 ; i < buffersize ; i++) console_printf("=");
					console_printf("==================\r\n");
				}
				break;
				default:
				{
					buffer[0] = 0;
					if(cmds[cmdcount].func != NULL) {
						cmds[cmdcount].func(cmds[cmdcount].index, (char *)&buffer, sizeof(buffer), serial);
					}
                    
                    if(cmds[cmdcount].string[0] != '$') {
                        if(strlen(buffer) == 0) {
                            console_printf("\t%d) %s\r\n",cmdcount, cmds[cmdcount].string);
                        } else {
                            console_printf("\t%d) %s%s\r\n",cmdcount, cmds[cmdcount].string,buffer);
                        }
                    } else {
                        if(cmds[cmdcount].string[1] == '-' && cmds[cmdcount].string[2] == '-' && cmds[cmdcount].string[3] == '-') {
                            buffersize = strlen(cmds[0].string);
                            console_printf("------------------");
                            for(i = 0 ; i < buffersize ; i++) console_printf("-");
                            console_printf("------------------\r\n");
                            
                        }
                    }

				}
				break;
			}
			cmdcount++;
		}

		console_printf("\t0) Return\r\n\r\n");
		if(cmdId != 0 && cmds[cmdId].func != NULL) {
			console_printf("====================================\r\n");
			console_printf("Result:\r\n\r\n");
			if(cmds[cmdId].func(cmds[cmdId].index,NULL,0,serial) == 0) {
				cmdId = 0;
				continue;
			}
			console_printf("====================================\r\n");
		}
		console_printf("\r\n> ");
		do {
			buffersize = 0;
			cmdId = 0;
			console_input_wait(buffer, &buffersize);
			console_printf("\r\n");
			if(!isdigit(buffer[0])) console_printf("Command Not Found\r\n\r\n> ");
			else {
				cmdId = atoi(buffer);
				if(cmdId == 0) break;
				else if(cmdId >= cmdcount || cmdId < 0) console_printf("Command Not Found\r\n\r\n> ");
				else if(cmds[cmdId].func == NULL) console_printf("Command Not Found\r\n\r\n> ");
				else break;
			}
		} while(1);

		if(cmdId == 0) return 0;
	} while(1);
}

void simple_cli_wait_seconds_and_return(int wait) {
	if(wait > 30) return;
	console_printf("\r\n");
	int i = 0;
	do {
		console_printf("\r");
		for(i = 0 ; i < wait ; i++) {
			console_printf("#");
		}
		console_printf(" Wait %ds and Return.................", wait);
		for(i = 0 ; i < wait ; i++) {
			console_printf(" ");
		}
		usleep(1000);
		wait--;
	} while(wait);
	console_printf("\r\n");
}

int simple_cli_loop(void *pc) {
    serial = (Serial *)pc;
	int result = 0;
    console_init();
	result = Main_Menu(0, NULL,0, serial);
	console_printf("System halted\r\n");
	return result;
}