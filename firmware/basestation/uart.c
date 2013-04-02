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
	char numbuffer[5];  // Buffer for integer to ASCII conversions
	myputs("\r\n");
	myputs("FRAMenergymeter\r\n===============\r\nMax Groening, 2013\r\nFirmware " FWVERSION "\r\n");
	uart_line();
	myputs("Sensor, type, impulses/unit, combined impulses:\r\n");
	unsigned int i;
	for (i = 0; i < MAX_SENSORS; i++) {
		myputc('S');
		myputc(i + '0');
		myputs(", ");
//		sensor_type_string(sensors[i].sensor, numbuffer);  // sensors[i]->sensor
//		myputs(numbuffer);
		myputs(sensor_type_strings[sensors[i].sensor]);
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

//	timedate[0] = '0' + ((RTCYEAR >> 12) & 0xf);
//	timedate[1] = '0' + ((RTCYEAR >> 8) & 0xf);
	timedate[0] = '2';
	timedate[1] = '0';
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
	while (!(RTCCTL01 & RTCRDY));
	myputs(dow_strings[RTCDOW]);
	myputs(", ");
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

unsigned int uart_user_ack (void) {
	/*
	 * Returns 1 if user enters 'yes', 0 else.
	 */
	while (!uartFlags.command) {};
	if (uart_rx_buffer[0] == 'y' && uart_rx_buffer[1] == 'e' && uart_rx_buffer[2] == 's') {
		uartFlags.command = 0;
		uart_clear_buffer();
		return 1;
	} else {
		uartFlags.command = 0;
		uart_clear_buffer();
		return 0;
	}
}

void uart_setup(void) {
	/*
	 * Run configuration dialog
	 */
	myputs("Setup\r\n");
	uart_line();
	unsigned char input[4];
	myputs("Please type 'yes' to edit corresponding settings.\r\n");
	myputs("Setup time and date? ");
	if (uart_user_ack()) {
		myputs("Year: 'YYYY' ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		input[1] = uart_rx_buffer[1];
		input[2] = uart_rx_buffer[2];
		input[3] = uart_rx_buffer[3];
		uart_clear_buffer();
//		myputs("Correct? ");
//		if (uart_user_ack()) {
			// Convert year to hex
			rtc_year = (0x2000 + (input[2] - '0') << 4) + ((input[3] - '0'));
//		} else {
//			myputs("Canceled.\r\n");
//			return;
//		}
		myputs("Month: 'MM' ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		input[1] = uart_rx_buffer[1];
		uart_clear_buffer();
//		myputs("Correct? ");
//		if (uart_user_ack()) {
			// Convert month to hex
			rtc_mon = ((input[0] - '0') << 4) + ((input[1] - '0'));
//		} else {
//			myputs("Canceled.\r\n");
//			return;
//		}
		myputs("Day: 'DD' ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		input[1] = uart_rx_buffer[1];
		uart_clear_buffer();
//		myputs("Correct? ");
//		if (uart_user_ack()) {
			// Convert day to hex
			rtc_day = ((input[0] - '0') << 4) + ((input[1] - '0'));
//		} else {
//			myputs("Canceled.\r\n");
//			return;
//		}
		myputs("Day of week: 0=Sun, 1=Mon, 2=Tue, 3=Wed, 4=Thu, 5=Fri, 6=Sat ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		uart_clear_buffer();
//		myputs("Correct? ");
//		if (uart_user_ack()) {
			// Convert day to hex
			rtc_dow = (input[0] - '0');
//		} else {
//			myputs("Canceled.\r\n");
//			return;
//		}
		myputs("Hour: 'hh' (24h notation) ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		input[1] = uart_rx_buffer[1];
		uart_clear_buffer();
//		myputs("Correct? ");
//		if (uart_user_ack()) {
			// Convert hour to hex
			rtc_hour = ((input[0] - '0') << 4) + ((input[1] - '0'));
//		} else {
//			myputs("Canceled.\r\n");
//			return;
//		}
		myputs("Minute: 'mm' ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		input[1] = uart_rx_buffer[1];
		uart_clear_buffer();
//		myputs("Correct? ");
//		if (uart_user_ack()) {
			// Convert minute to hex
			rtc_min = ((input[0] - '0') << 4) + ((input[1] - '0'));
//		} else {
//			myputs("Canceled.\r\n");
//			return;
//		}
		myputs("Second: 'ss' ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		input[1] = uart_rx_buffer[1];
		uart_clear_buffer();
//		myputs("Correct? ");
//		if (uart_user_ack()) {
			// Convert day to hex
			rtc_sec = ((input[0] - '0') << 4) + ((input[1] - '0'));
//		} else {
//			myputs("Canceled.\r\n");
//			return;
//		}
		// Setting new date
		myputs("Correct? ");
		if (uart_user_ack()) {
			set_rtc();
			myputs("Updated.\r\n");
		} else {
			myputs("Canceled.\r\n");
		}
	}
	myputs("Setup sensors? ");
	if (uart_user_ack()) {
		unsigned int input_sensor;
		unsigned char input_sensor_type, input_sensor_impulses, input_sensor_sum_up;
		myputs("Which sensor? (0-");
		myputc(MAX_SENSORS + '0' - 1);
		myputs(") ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		uart_clear_buffer();
		input_sensor = input[0] - '0';
		myputs("Type of sensor? 0=disabled, 1=electric, 2=water, 3=gas ");  // See enum sensor_type and sensor_type_strings
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		uart_clear_buffer();
		input_sensor_type = input[0] - '0';
		myputs("How many impulses/unit? (001-255) ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		input[1] = uart_rx_buffer[1];
		input[2] = uart_rx_buffer[2];
		uart_clear_buffer();
		input_sensor_impulses = ((input[0] - '0') * 100) + ((input[1] - '0') * 10) + (input[2] - '0');
		myputs("Combine impulses? (001-255) 000=no ");
		while (!uartFlags.command) {};
		uartFlags.command = 0;
		input[0] = uart_rx_buffer[0];
		input[1] = uart_rx_buffer[1];
		input[2] = uart_rx_buffer[2];
		uart_clear_buffer();
		input_sensor_sum_up = ((input[0] - '0') * 100) + ((input[1] - '0') * 10) + (input[2] - '0');
		myputs("Correct? ");
		if (uart_user_ack()) {
			sensors[input_sensor].sensor = (sensor_type)input_sensor_type;
			sensors[input_sensor].impulses = input_sensor_impulses;
			sensors[input_sensor].sum_up = input_sensor_sum_up;
			sensors[input_sensor].sum = input_sensor_sum_up;  // Initial setup of sum
			myputs("Updated.\r\n");
		} else {
			myputs("Canceled.\r\n");
		}
	}
}

void uart_error(void) {
	/*
	 * No valid command received
	 */
	myputs("\r\nInvalid command. Type 'help' for a list of valid commands.\r\n");
}
