/*
 * iv.c
 *
 * FRAMenergymeter
 * Track your energy consumption with a TI MSP-EXP430FR5739
 *
 * https://github.com/iOperator/FRAMenergymeter
 *
 * Max Gröning, 2013
 */

#include <msp430.h>
#include "basestation.h"
#include "uart.h"

/* ISR */

#pragma vector = PORT1_VECTOR
__interrupt void Port1InterruptHandler(void) {
	switch (__even_in_range(P1IV, P1IV_P1IFG7)) {
		case P1IV_NONE:
			break;
		case P1IV_P1IFG0:
			flag_sensor |= BIT0;
			LPM3_EXIT;  // Stay awake
			break;
		case P1IV_P1IFG1:
			flag_sensor |= BIT1;
			LPM3_EXIT;
			break;
		case P1IV_P1IFG2:
			flag_sensor |= BIT2;
			LPM3_EXIT;
			break;
		case P1IV_P1IFG3:
			flag_sensor |= BIT3;
			LPM3_EXIT;
			break;
		case P1IV_P1IFG4:
			break;
		case P1IV_P1IFG5:
			break;
		case P1IV_P1IFG6:
			break;
		case P1IV_P1IFG7:
			break;
	}
}

#pragma vector = PORT4_VECTOR
__interrupt void Port4InterruptHandler(void) {
	switch (__even_in_range(P4IV, P4IV_P4IFG7)) {
		case P4IV_NONE:
			break;
		case P4IV_P4IFG0:
			LPM3_EXIT;  // Pushbutton S1 (for debuging)
			break;
		case P4IV_P4IFG1:
			LPM3_EXIT;  // Pushbutton S1 (for debuging)
			break;
		case P4IV_P4IFG2:
			break;
		case P4IV_P4IFG3:
			break;
		case P4IV_P4IFG4:
			break;
		case P4IV_P4IFG5:
			break;
		case P4IV_P4IFG6:
			break;
		case P4IV_P4IFG7:
			break;
	}
}

#pragma vector = USCI_A0_VECTOR
__interrupt void USCIA0InterruptHandler(void) {
	switch (__even_in_range(UCA0IV, UCTXCPTIFG)) {
		case USCI_NONE:  // No interrupt pending
			break;
		case USCI_UART_UCRXIFG:  // Receive buffer full
			uart_rx_char = UCA0RXBUF;
			/* Check for 'enter' key: CR, LF, or CRLF */
			if (uart_rx_char == '\n' && uart_rx_buffer[uart_rx_buffer_p] == '\r') {  // Ignore LF if last char was a CR
				break;
			} else if (uart_rx_char == '\r') {  // CR
				uartFlags.command = 1;
//				while (UCBUSY & UCA0STATW) {};
//				UCA0TXBUF = '\r';
//				while (UCBUSY & UCA0STATW) {};
//				UCA0TXBUF = '\n';
			} else if (uart_rx_char == '\n') {  // LF
				uartFlags.command = 1;
//				myputs("\r\n");
			}
			/* Append current char to command buffer */
			if (uart_rx_buffer_p < (RX_BUFFER_SIZE - 1)) {
				uart_rx_buffer[uart_rx_buffer_p++] = uart_rx_char;
				uart_rx_buffer[uart_rx_buffer_p + 1] = '\0';
			}
			/* Echo char */
			if (uartFlags.echo) {
				while (UCBUSY & UCA0STATW) {};
				UCA0TXBUF = uart_rx_char;
//				myputc(uart_rx_char);
			}
			LPM3_EXIT;
			break;
		case USCI_UART_UCTXIFG:  // Transmit buffer empty
			break;
		case USCI_UART_UCSTTIFG:  // Start bit received
			break;
		case USCI_UART_UCTXCPTIFG:  // Transmit complete
			break;
	}
}

#pragma vector = ADC10_VECTOR, COMP_D_VECTOR, DMA_VECTOR, \
	PORT2_VECTOR, PORT3_VECTOR, \
	RTC_VECTOR, SYSNMI_VECTOR, \
	TIMER0_A0_VECTOR, TIMER0_A1_VECTOR, TIMER0_B0_VECTOR, TIMER0_B1_VECTOR, \
	TIMER1_A0_VECTOR, TIMER1_A1_VECTOR, TIMER1_B0_VECTOR, TIMER1_B1_VECTOR, \
	TIMER2_B0_VECTOR, TIMER2_B1_VECTOR, \
	UNMI_VECTOR, USCI_A1_VECTOR, USCI_B0_VECTOR, WDT_VECTOR
__interrupt void ISR_trap(void) {
	// Causing an access violation which will result in a PUC reset
	WDTCTL = 0;
}
