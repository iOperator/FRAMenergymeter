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

/* Defines */

#define FWVERSION "v0.0.1"
#define IMPULSE_SIZE 1250  // Allocate memory for this many impulses
#define MAX_SENSORS 4  // Maximum number of supported sensors

/* Types */

typedef enum {uart, cc2500} transmit_modes;
typedef enum {disabled=0, electric=1, water=2, gas=3} sensor_type;  // Also see string representation in sensor_type_strings

typedef struct {
//	unsigned int year;
	unsigned char year;  // Just save 20xx instead of full 4-digit year
	unsigned char mon;
	unsigned char day;
//	unsigned char dow;  // Day of week is redundant information
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
} impulseStruct;

typedef struct {
	sensor_type sensor;  // Type of sensor, see sensor_type enum
	unsigned char impulses;  // Impulses per unit, e.g. 80 impulses = 1 kW*h
	unsigned char sum_up;  // Combine multiple impulses and store them together?
	unsigned char sum;  // Impulses left before storing
} sensorsStruct;

/* Globals */

// Stores time and date for RTC update (in BCD)
extern unsigned int rtc_year;
extern unsigned char rtc_mon;
extern unsigned char rtc_day;
extern unsigned char rtc_dow;
extern unsigned char rtc_hour;
extern unsigned char rtc_min;
extern unsigned char rtc_sec;

extern unsigned int current_impulse;  // Pointer to impulseData
extern sensorsStruct sensors[MAX_SENSORS];  // Stores settings for each sensor
extern char * const sensor_type_strings[];
extern char * const dow_strings[];

/* Functions */

void init(void);  // Initialization

void init_uart(void);  // Setup the UART

void init_rtc(void);  // Setup the Real-Time Clock

void set_rtc(void);  // Set the RTC with values stored in global rtc_* variables

void clear_impulse_data(void);  // Clears the impulse data memory section

int save_impulse(void);  // Save a impulse with current time and date

void send_impulses(transmit_modes tx_mode);  // Send all saved impulses via the given transmit mode

unsigned int measure_battery(void);  // Measures the battery voltage and returns it

void bcd_to_ascii_8(unsigned char bcd, char *ascii);  // Converts a BCD byte to ASCII

void bcd_to_ascii_16(unsigned char bcd, char *ascii);  // Converts a BCD word to ASCII

void int_to_ascii(int num, char ascii[]);  // Converts an integer into ASCII

//void sensor_type_string(sensor_type sensor, char description[]);  // Returns the string representation of given sensor type


#endif /* BASESTATION_H_ */
