/*
 * uart.h
 *
 * FRAMenergymeter
 * Track your energy consumption with a TI MSP-EXP430FR5739
 *
 * https://github.com/iOperator/FRAMenergymeter
 *
 * Max Gröning, 2013
 */

#ifndef UART_H_
#define UART_H_

/* Defines */

#define RX_BUFFER_SIZE 8  // Length of buffer to store commands from UART (incl. \r\n and \0)
#define UART_PROMPT "> "

/* Types */

typedef struct {
	unsigned command:1;  // 'Enter' key received
	unsigned echo:1;  // Echo received chars
	unsigned prompt:1;  // Show prompt
	unsigned sleep:1;  // Sleep mode
} uartFlagsStruct;

/* Globals */

extern char uart_rx_buffer[RX_BUFFER_SIZE];
extern unsigned int uart_rx_buffer_p;
extern char uart_rx_char;  // Stores last received char for comparison and echo
extern uartFlagsStruct uartFlags;  // Stores command, echo, prompt, and sleep flags

/* Functions */

int myprintf(const char *fmt, ...);  // Simple printf replacement

void myputc(unsigned b);  // Simple putc replacement

void myputs(char *s);  // Simple puts replacement

void uart_clear_buffer(void);  // Clears UART command buffer and resets buffer pointer

void uart_prompt(void);  // Send UART prompt

void uart_line(void);  // Print a line of dashes

void uart_info(void);  // Send info via UART

void uart_help(void);  // Send help screen

void uart_send(void);  // Send stored impulses

void uart_clear(void);  // Clear stored impulses

unsigned int uart_user_ack(void);  // Returns 1 if user enters 'yes', 0 else.

void uart_setup(void);  // Run configuration dialog

void uart_error(void);  // No valid command received

#endif /* UART_H_ */
