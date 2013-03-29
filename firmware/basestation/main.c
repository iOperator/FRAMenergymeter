/*
 * main.c
 *
 * FRAMenergymeter
 * Track your energy consumption with a TI MSP-EXP430FR5739
 *
 * https://github.com/iOperator/FRAMenergymeter
 *
 * Max Gröning, 2013
 */

#include <msp430.h>
#include "iv.h"
#include "basestation.h"
#include "uart.h"

int main(void) {
	init();
	init_uart();
	init_rtc();
	set_rtc();

	_enable_interrupts();

//	clear_impulse_data();  // TESTING ONLY
	uart_info();
	uart_promt();
//	uart_clear_buffer();
//	uart_rx_buffer_p = 0x0;  // TESTING ONLY
	uartFlags.echo = 1;
	uartFlags.promt = 1;

	while(1) {
		// TESTING ONLY
		if (!(P4IN & BIT0)) {
			P3OUT &= ~(BIT7 + BIT6);
			save_impulse();
		} else {
			P3OUT |= BIT7 + BIT6;
		}
		if (!(P4IN & BIT1)) {
//			clear_impulse_data();
			send_impulses(uart);
			myputs(uart_rx_buffer);
		}
		if (uartFlags.command) {
			P3OUT |= BIT4;
			__delay_cycles(10000);
			myputs("\r\n");
			uartFlags.command = 0;
			if (uart_rx_buffer[0] == 'i' && uart_rx_buffer[1] == 'n' && uart_rx_buffer[2] == 'f' && uart_rx_buffer[3] == 'o') {  // Info
				uart_clear_buffer();
				uart_info();
				uart_promt();
			} else if (uart_rx_buffer[0] == 'h' && uart_rx_buffer[1] == 'e' && uart_rx_buffer[2] == 'l' && uart_rx_buffer[3] == 'p') {  // Help
				uart_clear_buffer();
				uart_help();
				uart_promt();
			} else if (uart_rx_buffer[0] == 's' && uart_rx_buffer[1] == 'e' && uart_rx_buffer[2] == 'n' && uart_rx_buffer[3] == 'd') {  // Send
				uart_clear_buffer();
				uart_send();
				uart_promt();
			} else if (uart_rx_buffer[0] == 'c' && uart_rx_buffer[1] == 'l' && uart_rx_buffer[2] == 'e' && uart_rx_buffer[3] == 'a' && uart_rx_buffer[4] == 'r') {  // Clear
				uart_clear_buffer();
				uart_clear();
				uart_promt();
			} else if (uart_rx_buffer[0] == 's' && uart_rx_buffer[1] == 'e' && uart_rx_buffer[2] == 't' && uart_rx_buffer[3] == 'u' && uart_rx_buffer[4] == 'p') {  // Setup
				uart_clear_buffer();
				uart_setup();
				uart_promt();
			} else if (uart_rx_buffer[0] == '\x77' && uart_rx_buffer[1] == '\x68' && uart_rx_buffer[2] == '\x79') {
				uart_clear_buffer();
				myputs("\x62\x65\x63\x61\x75\x73\x65\x21");
				uart_promt();
			} else if (uart_rx_buffer[0] == '\n' || uart_rx_buffer[0] == '\r') { // Nothing entered, just enter key
				uart_clear_buffer();
				uart_promt();
			} else {  // Invalid command entered
				uart_clear_buffer();
				uart_error();
				uart_promt();
			}
		} else {
			P3OUT &= ~BIT4;
		}
	}
}
