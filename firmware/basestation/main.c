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

int main(void) {
    init();
    init_uart();
    init_rtc();
    set_rtc();
	
    _enable_interrupts();

    clear_impulse_data();  // TESTING ONLY

	while(1) {
		// TESTING ONLY
		if (!(P4IN & BIT0)) {
			P3OUT &= ~(BIT7 + BIT6 + BIT5 + BIT4);
			save_impulse();
		} else {
			P3OUT |= BIT7 + BIT6 + BIT5 + BIT4;
		}
		if (!(P4IN & BIT1)) {
//			clear_impulse_data();
			send_impulses(uart);
		}
	}
}
