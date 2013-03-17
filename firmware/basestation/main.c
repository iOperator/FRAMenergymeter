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

void init(void);  // Initialization
void init_uart(void);  // Setup the UART
void init_rtc(void);  // Setup the Real-Time Clock
void set_rtc();  // Set the RTC with values stored in rtc_* variables.

// RTC values stored in BCD (see documentation)
unsigned char rtc_sec = 0x0;  // Seconds = 0
unsigned char rtc_min = 0x0;  // Minutes = 0
unsigned char rtc_hour = (1 << 4) | 2;  // 12 o'clock
unsigned char rtc_dow = 0x0;  // Sunday
unsigned char rtc_day = 0x17;  // 17th day of month
unsigned char rtc_mon = 0x3;  // Month march
unsigned int rtc_year = 0x2013;  // Year 2013

int main(void) {
    init();
    init_uart();
    init_rtc();
    set_rtc();
	
	while(1);
}

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
	P1OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output low or pulled down
	P1DIR |= (BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output direction
	P1IES |= BIT0;  // Set interrupt flag on high-to-low transition
	P1IE |= BIT0;  // Enable interrupt
	P1IFG = 0;  // Clear P1 interrupt flags

	/* Port 2 */
	P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output low or pulled down
	P2DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output direction
	P2SEL1 |= (BIT0 + BIT1);  // Select UCA0TXD and UCA0RDX

	/* Port 3 */
	P3OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output low or pulled down
	P3DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);  // Output direction

	/* Port 4 */
//	P4OUT &= ~(BIT0 + BIT1);  // Output low or pulled down
//	P4DIR |= (BIT0 + BIT1);  // Output direction
	P4REN |= (BIT0 + BIT1);  // Enable pullup/pulldown resistor
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

	// Turn off temperature sensor
	REFCTL0 |= REFTCOFF;
	REFCTL0 &= ~REFON;

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
	UCA0BR0 = 3;  // Set prescaler of BRG for 9600 Baud
	UCA0MCTLW |= 0x9200;  // 32768/9600 - INT(32768/9600)=0.41
	UCA0BR1 = 0;  // UCBRSx value = 0x92 (See UG)
	UCA0CTL1 &= ~UCSWRST;  // Release from reset
}

void init_rtc(void) {
	/*
	 * Setup the Real-Time Clock
	 */
	RTCCTL01 |= RTCBCD;  // Select BCD (Binary coded decimal)
	RTCCTL01 &= ~RTCHOLD;  // Real-time clock is operational
}

void set_rtc(void){
	/*
	 * Set the RTC with values stored in rtc_* variables.
	 */
	while (RTCCTL01 & RTCRDY);  // Wait until RTC is getting updated.
	while (!(RTCCTL01 & RTCRDY));  // Trap it until the update finish.
	RTCSEC = rtc_sec;
	RTCMIN = rtc_min;
	RTCHOUR = rtc_hour;
	RTCDOW = rtc_dow;
	RTCDAY = rtc_day;
	RTCMON = rtc_mon;
	RTCYEAR = rtc_year;
}
