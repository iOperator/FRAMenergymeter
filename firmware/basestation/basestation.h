/*
 * basestation.h
 *
 * FRAMenergymeter
 * Track your energy consumption with a TI MSP-EXP430FR5739
 *
 * https://github.com/iOperator/FRAMenergymeter
 *
 * Max Gröning, 2013
 */

#ifndef BASESTATION_H_
#define BASESTATION_H_

/* Globals */
extern unsigned int current_impulse;

/* Functions */

void init(void);  // Initialization

void init_uart(void);  // Setup the UART

void init_rtc(void);  // Setup the Real-Time Clock

void set_rtc(void);  // Set the RTC with values stored in global rtc_* variables

void clear_impulse_data(void);  // Clears the impulse data memory section


#endif /* BASESTATION_H_ */
