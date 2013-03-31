/*
 * basestation.c
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

/* RTC values are stored as Binary Coded Decimals (BCD) (see documentation) */
unsigned int rtc_year = 0x2013;  // Year 2013
unsigned char rtc_mon = 0x3;  // Month march
unsigned char rtc_day = 0x17;  // 17th day of month
unsigned char rtc_dow = 0x0;  // Sunday
unsigned char rtc_hour = 0x12;  // 12 o'clock
unsigned char rtc_min = 0x0;  // Minutes = 0
unsigned char rtc_sec = 0x0;  // Seconds = 0

#pragma DATA_SECTION(impulseData, ".fram_vars")
//#pragma NOINIT(impulseData)
//#pragma PERSISTENT(impulseData)
impulseStruct impulseData[IMPULSE_SIZE];  // Permanently stores data of each impulse

#pragma DATA_SECTION(current_impulse, ".fram_vars")
//#pragma NOINIT(current_impulse)
//#pragma PERSISTENT(current_impulse)
unsigned int current_impulse = 0x0;  // Pointer to impulseData

#pragma DATA_SECTION(sensors, ".fram_vars")
sensorsStruct sensors[MAX_SENSORS];  // Stores settings for each sensor
char * const sensor_type_strings[] = {"disabled", "electric", "water", "gas"};  // Also see enum sensor_type

char * const dow_strings[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

volatile unsigned char flag_sensor = 0x0;
volatile unsigned char flag_rx = 0x0;
volatile unsigned char flag_button = 0x0;

/* Functions */

void init(void) {
	/*
	 * Initialization
	 */
	WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer

	/*
	 * I/Os
	 *
	 * P1.0  Impulse Sensor 1 (input)
	 * P2.0  UCA0TXD
	 * P2.1  UCA0RXD
	 * P3.7  LED 8
	 * P3.6  LED 7
	 * P3.5  LED 6
	 * P3.4  LED 5
	 * P4.0  Pushbutton S1 (input, pull-up)
	 * P4.1  Pushbutton S2 (input, pull-up)
	 * PJ.4  XIN
	 * PJ.5  XOUT
	 *
	 */

	/* Port 1 */
	P1OUT &= ~(BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output low or pulldown
	P1OUT |= BIT0;  // Output high or pullup
	P1DIR |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output direction
	P1REN |= BIT0;  // Enable pullup/pulldown resistors
	P1IES |= BIT0;  // Set interrupt flag on high-to-low transition
	P1IE |= BIT0;  // Enable interrupt
	P1IFG = 0;  // Clear P1 interrupt flags

	/* Port 2 */
	P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output low or pulled down
	P2DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output direction
	P2SEL1 |= (BIT0 + BIT1);  // Select UCA0TXD and UCA0RDX
	P2SEL0 &= ~(BIT0 + BIT1);  //

	/* Port 3 */
	P3OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output low or pulled down
	P3DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output direction

	/* Port 4 */
	P4OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output low or pulled down
	P4OUT |= (BIT0 + BIT1);  // Pull up inputs for pushbuttons S1 and S2
	P3DIR |= (BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output direction
	P4DIR &= ~(BIT0 + BIT1);  // Inputs for pushbuttons S1 and S2
	P4REN |= (BIT0 + BIT1);  // Enable pull-up/pull-down resistor
	P4IES |= (BIT0 + BIT1);  // Set interrupt flag on high-to-low transition
	P4IE |= (BIT0 + BIT1);  // Enable interrupt
	P4IFG = 0;  // Clear P4 interrupt flags

	/* Port J */
	PJOUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5);  // Output '0'
	PJDIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5);  // Outputs
	PJSEL0 |= (BIT4 + BIT5);  // Set XIN and XOUT to crystal mode

	/* Clock system */
	/* Startup clock system in max. DCO setting ~8MHz */
	CSCTL0 = CSKEY;  // Unlock register
	CSCTL1 |= DCOFSEL0 + DCOFSEL1; // Set max. DCO setting
	CSCTL2 = SELA_0 + SELS_3 + SELM_3; // Set ACLK = XT1; SMCLK = DCO; MCLK = DCO
	CSCTL3 = DIVA_0 + DIVS_0 + DIVM_0; // Set all dividers to '/1'

	/* Wake up from LPM.5*/
	PMMCTL0_H = PMMPW_H; // Unlock PMM control register
	PM5CTL0 &= ~LOCKLPM5; // Clear Lock I/O and enable ports
	PMMCTL0_H = 0x00; // Lock PMM control register

	/* Wait for stable XT1 */
	do {
		CSCTL5 &= ~XT1OFFG;  // Clear XT1 fault flag
		SFRIFG1 &= ~OFIFG ;
	} while (SFRIFG1 & OFIFG);  // Test oscillator fault flag

	/* Turn off the temperature sensor */
	REFCTL0 |= REFTCOFF;
	REFCTL0 &= ~REFON;
}


void init_uart(void) {
	/*
	 * Setup the UART
	 */
	UCA0CTL1 |= UCSWRST;  // eUSCI reset state
	UCA0CTL1 = UCSSEL_1;  // Set ACLK = 32768 as UCBRCLK
	UCA0BRW = 3;  // Set prescaler of BRC for 9600 Baud
	UCA0MCTLW = 0x9200;  // 32768/9600 - INT(32768/9600)=0.41 => UCBRSx = 0x92
	UCA0CTL1 &= ~UCSWRST;  // Release from reset
	UCA0IE |= UCRXIE;  // Enable RX interrupt

//	UCA0CTL1 |= UCSWRST;  // eUSCI reset state
//	UCA0CTL1 = UCSSEL_2;  // Set SMCLK = 8000000 as UCBRCLK
//	UCA0BRW = 52;  // Set prescaler of BRG for 9600 Baud
//	UCA0MCTLW = 0x4900 + 0x10 + UCOS16;  // 8000000/9600 - INT(8000000/9600)=0.333
//	UCA0CTL1 &= ~UCSWRST;  // Release from reset
//	UCA0IE |= UCRXIE;  // Enable RX interrupt

//	UCA0CTL1 |= UCSWRST;  // eUSCI reset state
//	UCA0CTL1 = UCSSEL_1;  // Set ACLK = 32768 as UCBRCLK
//	UCA0BRW = 1;  // Set prescaler of BRC for 1200 Baud
//	UCA0MCTLW = 0x2500 + 0xB0 + UCOS16;  // 32768/1200 - INT(32768/1200)=0.306 => UCBRSx = 0x25
//	UCA0CTL1 &= ~UCSWRST;  // Release from reset
//	UCA0IE |= UCRXIE;  // Enable RX interrupt
}


void init_rtc(void) {
	/*
	 * Setup the Real-Time Clock
	 */
	RTCCTL01 |= RTCBCD;  // Select BCD (Binary coded decimal)
	RTCCTL01 &= ~RTCHOLD;  // Real-time clock is operational
}


void set_rtc(void) {
	/*
	 * Set the RTC with values stored in global rtc_* variables
	 */
	while (RTCCTL01 & RTCRDY);  // Wait until RTC is getting updated.
	while (!(RTCCTL01 & RTCRDY));  // Trap it until the update finish.
	RTCCTL01 |= RTCHOLD;
	RTCYEAR = rtc_year;
	RTCMON = rtc_mon;
	RTCDAY = rtc_day;
	RTCDOW = rtc_dow;
	RTCHOUR = rtc_hour;
	RTCMIN = rtc_min;
	RTCSEC = rtc_sec;
	RTCCTL01 &= ~RTCHOLD;
}


void clear_impulse_data(void) {
	/*
	 * Clears the impulse data memory section
	 */
	unsigned int i;
	for (i = 0; i < IMPULSE_SIZE; ++i) {
		impulseData[i].year = 0x0;
		impulseData[i].mon = 0x0;
		impulseData[i].day = 0x0;
//		impulseData[i].dow = 0x0;
		impulseData[i].hour = 0x0;
		impulseData[i].min = 0x0;
		impulseData[i].sec = 0x0;
	}
	current_impulse = 0;
}

int save_impulse(void) {
	/*
	 * Save a impulse with current time and date
	 */
	if (current_impulse < IMPULSE_SIZE) {
		while (RTCCTL01 & RTCRDY);  // Wait until RTC is getting updated.
		while (!(RTCCTL01 & RTCRDY));  // Trap it until the update finish.
		impulseData[current_impulse].year = RTCYEAR;
		impulseData[current_impulse].mon = RTCMON;
		impulseData[current_impulse].day = RTCDAY;
//		impulseData[current_impulse].dow = RTCDOW;
		impulseData[current_impulse].hour = RTCHOUR;
		impulseData[current_impulse].min = RTCMIN;
		impulseData[current_impulse].sec = RTCSEC;
		current_impulse++;
		return 0;
	} else {
		/* No more free space to store the impulse */
		return -1;
	}
}

void send_impulses(transmit_modes tx_mode) {
	/*
	 * Send all saved impulses via the given transmit mode
	 */
	if (tx_mode == uart) {
		unsigned int i;
		for (i = 0; i < current_impulse; i++) {
//			myputc((impulseData[i].year >> 12) + 0x30);  // Send BCD as ASCII
//			myputc(((impulseData[i].year >> 8) & 0xf) + 0x30);
			myputs("20");
			myputc(((impulseData[i].year >> 4) & 0xf) + 0x30);
			myputc((impulseData[i].year & 0xf) + 0x30);
			myputc((impulseData[i].mon >> 4) + 0x30);
			myputc((impulseData[i].mon & 0xf) + 0x30);
			myputc((impulseData[i].day >> 4) + 0x30);
			myputc((impulseData[i].day & 0xf) + 0x30);
//			myputc((impulseData[i].dow >> 4) + 0x30);
//			myputc((impulseData[i].dow & 0xf) + 0x30);
			myputc((impulseData[i].hour >> 4) + 0x30);
			myputc((impulseData[i].hour & 0xf) + 0x30);
			myputc((impulseData[i].min >> 4) + 0x30);
			myputc((impulseData[i].min & 0xf) + 0x30);
			myputc((impulseData[i].sec >> 4) + 0x30);
			myputc((impulseData[i].sec & 0xf) + 0x30);
			myputs("\r\n");
		}
	} else if (tx_mode == cc2500) {
		/* TBD */
	}
}

unsigned int measure_battery(void) {
	/*
	 * Measures the battery voltage and returns it
	 */
	unsigned long result;
	/* ADC setup */
	ADC10CTL0 = ADC10SHT_2 + ADC10ON;  // 16 ADC cycles, ADC on
	ADC10CTL1 = ADC10SHP;  // SAMPCON from sampling timer, single-channel, single-conversion
	ADC10MCTL0 = ADC10SREF_1 + ADC10INCH_11;  // VREF to AVSS, Input channel = (AVCC-AVSS) / 2
	/* Setup voltage reference */
	while(REFCTL0 & REFGENBUSY);  // Trap if reference is busy
	REFCTL0 |= REFVSEL_2 + REFON;  // 1.5 V, Reference enabled
	__delay_cycles(400);  // Wait for reference to settle
	/* ADC sample */
	ADC10CTL0 |= ADC10ENC + ADC10SC;  // Enable and start conversion
	while (ADC10CTL1 & ADC10BUSY);  // Wait while ADC is busy
	result = (ADC10MEM0 * 5000l);
	/* Disable ADC and reference */
	ADC10CTL0 &= ~ADC10ENC;
	ADC10CTL0 &= ~ADC10ON;
	while(REFCTL0 & REFGENBUSY);
	REFCTL0 &= ~REFON;

	return ((unsigned int)(result / 1024l));
}

void bcd_to_ascii_8(unsigned char bcd, char *ascii) {
	/*
	 * Converts a BCD byte to ASCII
	 */
	*ascii++ = (bcd >> 4) + '0';
	*ascii++ = (bcd & 0x0f) + '0';
	*ascii = '\0';
}

void bcd_to_ascii_16(unsigned char bcd, char *ascii) {
	/*
	 * Converts a BCD word to ASCII
	 */
	*ascii++ = (bcd >> 12) + '0';
	*ascii++ = ((bcd >> 8) & 0x0f) + '0';
	*ascii++ = ((bcd >> 4) & 0x0f) + '0';
	*ascii++ = (bcd & 0x0f) + '0';
	*ascii = '\0';
}

void int_to_ascii(int num, char ascii[]) {
	/*
	 * Converts an integer into ASCII
	 */
	unsigned int i = 0;
	unsigned int j;
	do {
		ascii[i++] = '0' + num % 10;
		num /= 10;
	} while (num > 0);
	/* Mirror the string */
	char tmp;
	for (j = 0; j < i / 2; ++j) {
		tmp = ascii[j];
		ascii[j] = ascii[i-j-1];
		ascii[i-j-1] = tmp;
	}
	ascii[i] = '\0';
}

//void sensor_type_string(sensor_type sensor, char description[]) {
//	/*
//	 * Returns the string representation of given sensor type
//	 */
//	if (sensor == disabled) {
////		description = "disabled";
//		description[0] = 'd';
//		description[1] = 'i';
//		description[2] = 's';
//		description[3] = 'a';
//		description[4] = 'b';
//		description[5] = 'l';
//		description[6] = 'e';
//		description[7] = 'd';
//		description[8] = '\0';
//	} else if (sensor == electric) {
////		description = "electric";
//		description[0] = 'e';
//		description[1] = 'l';
//		description[2] = 'e';
//		description[3] = 'c';
//		description[4] = 't';
//		description[5] = 'r';
//		description[6] = 'i';
//		description[7] = 'c';
//		description[8] = '\0';
//	} else if (sensor == water) {
////		description = "water";
//		description[0] = 'w';
//		description[1] = 'a';
//		description[2] = 't';
//		description[3] = 'e';
//		description[4] = 'r';
//		description[5] = '\0';
//	} else if (sensor == gas) {
////		description = "gas";
//		description[0] = 'g';
//		description[1] = 'a';
//		description[2] = 's';
//		description[3] = '\0';
//	} else {
//		description = "error";
//	}
//}
