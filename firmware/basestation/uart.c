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
uartFlagsStruct uartFlags;  // Stores command, echo, and promt flags

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

void uart_promt(void) {
	/*
	 * Send UART promt
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
	myputs("\r\n");
	myputs("FRAMenergymeter\r\n===============\r\nMax Groening, 2013\r\nFirmware " FWVERSION "\r\n");
	uart_line();
	myputs("Sensor(s), type, impulses/unit:\r\n");
	myprintf("Memory: %d of %d used. (%d%%)\r\n", current_impulse, IMPULSE_SIZE, (current_impulse * 100 / IMPULSE_SIZE));
	// myputs("Memory: "  " of " " free.\r\n");
	myputs("RTC time:\r\n");
	myputs("RTC date:\r\n");
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
	myprintf("Sending %d impulse(s):\r\n", current_impulse);
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
	 * No valid command recieved
	 */
	myputs("\r\nInvalid command. Type 'help' for a list of valid commands.\r\n");
}
