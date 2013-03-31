/*
 * uart.c
 *
 * FRAMenergymeter
 * Track your energy consumption with a TI MSP-EXP430FR5739
 *
 * https://github.com/iOperator/FRAMenergymeter
 *
 * Max Gröning, 2013
 */

#include <msp430.h>
#include <stdio.h>
#include "basestation.h"
#include "uart.h"

/* Globals */

char uart_rx_buffer[RX_BUFFER_SIZE];  // Stores received commands
unsigned int uart_rx_buffer_p = 0x0;  // Pointer variable for uart_rx_buffer
char uart_rx_char;  // Stores last received char for comparison and echo
uartFlagsStruct uartFlags;  // Stores command, echo, prompt, and sleep flags

/* Functions */

int myprintf(const char *fmt, ...) {
	/*
	 * Simple printf replacement
	 */
    char buf[256], *p;
    va_list args;
    int n=0;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    p=buf;
    while (*p) {
    	while (UCBUSY & UCA0STATW) {};
    	UCA0TXBUF = *p;
        n++;
        p++;
    }
    return n;
}

void myputc(unsigned b) {
	/*
	 * Simple putc replacement
	 */
	while (UCBUSY & UCA0STATW) {};  // Wait while UART is busy
	UCA0TXBUF = b;
}

void myputs(char *s) {
	/*
	 * Simple puts replacement
	 */
	char c;
	// Loop through each character in string 's'
	while (c = *s++) {
		while (UCBUSY & UCA0STATW) {};  // Wait while UART is busy
		UCA0TXBUF = c;
	}
}

void uart_clear_buffer(void) {
	/*
	 * Clears UART command buffer and resets buffer pointer
	 */
	uart_rx_buffer_p = 0;
	uart_rx_buffer[0] = '\0';
}

void uart_prompt(void) {
	/*
	 * Send UART prompt
	 */
	myputs("\r\n");
	myputs(UART_PROMPT);
}

void uart_line(void) {
	/*
	 * Print a line of dashes
	 */
	unsigned int i;
	for (i = 78; i > 0; --i) {
		myputc('-');
	}
	myputs("\r\n");
}

void uart_info(void) {
	/*
	 * Send info via UART
	 */
	char numbuffer[9];  // Buffer for integer to ASCII conversions
	myputs("\r\n");
	myputs("FRAMenergymeter\r\n===============\r\nMax Groening, 2013\r\nFirmware " FWVERSION "\r\n");
	uart_line();
	myputs("Sensor(s), type, impulses/unit, combine:\r\n");
	unsigned int i;
	for (i = 0; i < MAX_SENSORS; i++) {
		myputc('S');
		myputc(i + '0');
		myputs(", ");
		sensor_type_string(sensors[i].sensor, numbuffer);  // (*sensors[i]).sensor
		myputs(numbuffer);
		myputs(", ");
		int_to_ascii(sensors[i].impulses, numbuffer);
		myputs(numbuffer);
		myputs(", ");
		int_to_ascii(sensors[i].sum_up, numbuffer);
		myputs(numbuffer);
		myputs("\r\n");
	}

//	myprintf("Memory: %d of %d used. (%d%%)\r\n", current_impulse, IMPULSE_SIZE, (current_impulse * 100 / IMPULSE_SIZE));  // Warning: printf uses lot of memory
	myputs("Memory: ");
	int_to_ascii(current_impulse, numbuffer);
	myputs(numbuffer);
	myputs(" of ");
	int_to_ascii(IMPULSE_SIZE, numbuffer);
	myputs(numbuffer);
	myputs(" used. (");
	int_to_ascii((current_impulse * 100 / IMPULSE_SIZE), numbuffer);
	myputs(numbuffer);
	myputs("%)\r\n");

	char timedate[11];
	while (!(RTCCTL01 & RTCRDY));  // Trap RTC until the update finish.
//	bcd_to_ascii_8(RTCHOUR, &timedate[0]);
//	timedate[2] = ':';
//	bcd_to_ascii_8(RTCMIN, &timedate[3]);
//	timedate[5] = ':';
//	bcd_to_ascii_8(RTCSEC, &timedate[6]);
	timedate[0] = '0' + (RTCHOUR >> 4);
	timedate[1] = '0' + (RTCHOUR & 0xf);
	timedate[2] = ':';
	timedate[3] = '0' + (RTCMIN >> 4);
	timedate[4] = '0' + (RTCMIN & 0xf);
	timedate[5] = ':';
	timedate[6] = '0' + (RTCSEC >> 4);
	timedate[7] = '0' + (RTCSEC & 0xf);
	timedate[8] = '\0';
	myputs("RTC time: ");
	myputs(timedate);
	myputs("\r\n");

	while (!(RTCCTL01 & RTCRDY));  // Trap RTC until the update finish.
//	bcd_to_ascii_16(RTCYEAR, &timedate[0]);
//	timedate[4] = ':';
//	bcd_to_ascii_8(RTCMON, &timedate[5]);
//	timedate[7] = ':';
//	bcd_to_ascii_8(RTCDAY, &timedate[8]);
	timedate[0] = '0' + ((RTCYEAR >> 12) & 0xf);
	timedate[1] = '0' + ((RTCYEAR >> 8) & 0xf);
	timedate[2] = '0' + ((RTCYEAR >> 4) & 0xf);
	timedate[3] = '0' + (RTCYEAR & 0xf);
	timedate[4] = '-';
	timedate[5] = '0' + (RTCMON >> 4);
	timedate[6] = '0' + (RTCMON & 0xf);
	timedate[7] = '-';
	timedate[8] = '0' + (RTCDAY >> 4);
	timedate[9] = '0' + (RTCDAY & 0xf);
	timedate[10] = '\0';
	myputs("RTC date: ");
	myputs(timedate);
	myputs("\r\n");

//	myprintf("Battery: %d mV\r\n", measure_battery());  // Warning: printf uses lot of memory
	myputs("Battery: ");
	int_to_ascii(measure_battery(), numbuffer);
	myputs(numbuffer);
	myputs(" mV\r\n");
	uart_line();
}

void uart_help(void) {
	/*
	 * Send help screen
	 */
	myputs("Help: list of commands\r\n");
	uart_line();
	myputs("info     Show info screen and status\r\n");
	myputs("help     Show this help screen\r\n");
	myputs("send     Send impulse data\r\n");
	myputs("clear    Delete impulse data\r\n");
	myputs("setup    Show setup menu\r\n");
}

void uart_send(void) {
	/*
	 * Send stored impulses
	 */
	char numbuffer[5];  // Buffer for integer to ASCII conversions
//	myprintf("Sending %d impulse(s):\r\n", current_impulse);  // Warning: printf uses lot of memory
	myputs("Sending ");
	int_to_ascii(current_impulse, numbuffer);
	myputs(numbuffer);
	myputs(" impulse(s):\r\n");

	uart_line();
	send_impulses(uart);
	uart_line();
	myputs("Done.\r\n");
}

void uart_clear(void) {
	/*
	 * Clear stored impulses
	 */
	myputs("Type 'yes' to DELETE all stored impulse data: ");
	while (!uartFlags.command) {};
	if (uart_rx_buffer[0] == 'y' && uart_rx_buffer[1] == 'e' && uart_rx_buffer[2] == 's') {
		myputs("Clearing impulse data ...\r\n");
		clear_impulse_data();
		myputs("All impulse data deleted.\r\n");
	} else {
		myputs("Canceled.\r\n");
	}
	uartFlags.command = 0;
	uart_clear_buffer();
}

void uart_setup(void) {
	/*
	 * Run configuration dialog
	 */

}

void uart_error(void) {
	/*
	 * No valid command received
	 */
	myputs("\r\nInvalid command. Type 'help' for a list of valid commands.\r\n");
}
