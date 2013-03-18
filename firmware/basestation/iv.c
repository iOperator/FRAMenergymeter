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

#pragma vector = PORT4_VECTOR
__interrupt void Port4InterruptHandler(void) {
	switch (__even_in_range(P4IV, P4IV_P4IFG7)) {
		case P4IV_NONE:
			break;
		case P4IV_P4IFG0:
			break;
		case P4IV_P4IFG1:
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

#pragma vector = ADC10_VECTOR, COMP_D_VECTOR, DMA_VECTOR, \
	PORT1_VECTOR, PORT2_VECTOR, PORT3_VECTOR, \
	RTC_VECTOR, SYSNMI_VECTOR, \
	TIMER0_A0_VECTOR, TIMER0_A1_VECTOR, TIMER0_B0_VECTOR, TIMER0_B1_VECTOR, \
	TIMER1_A0_VECTOR, TIMER1_A1_VECTOR, TIMER1_B0_VECTOR, TIMER1_B1_VECTOR, \
	TIMER2_B0_VECTOR, TIMER2_B1_VECTOR, \
	UNMI_VECTOR, USCI_A0_VECTOR, USCI_A1_VECTOR, USCI_B0_VECTOR, WDT_VECTOR
__interrupt void ISR_trap(void) {
	// Causing an access violation which will result in a PUC reset
	WDTCTL = 0;
}
