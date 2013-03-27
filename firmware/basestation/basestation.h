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

/* Types */
typedef struct{
	unsigned int year;
	unsigned char mon;
	unsigned char day;
	unsigned char dow;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
} impulseStruct;

typedef enum {uart, cc2500} transmit_modes;

/* Globals */
extern unsigned int current_impulse;

/* Functions */

void init(void);  // Initialization

void init_uart(void);  // Setup the UART

void init_rtc(void);  // Setup the Real-Time Clock

void set_rtc(void);  // Set the RTC with values stored in global rtc_* variables

void clear_impulse_data(void);  // Clears the impulse data memory section

int save_impulse(void);  // Save a impulse with current time and date

void send_impulses(transmit_modes tx_mode);  // Send all saved impulses via the given transmit mode


#endif /* BASESTATION_H_ */
