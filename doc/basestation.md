FRAMenergymeter
===============
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013

Concept
-------

### MSP430FR5739 I/Os
Pinheaders on the MSP-EXP430FR5739:

VCC                          GND
P2.0  TXD               S1  P4.0
P2.5                        P1.5
P2.6                        P1.4
P2.1  RXD                   P1.3
P2.2                        P3.3
P3.4  LED5                  P3.2
P3.5  LED6                  P3.1
P3.6  LED7                  P3.0
P3.7  LED8                  P1.2
P1.6                        P1.1
P1.7               ImpSens  P1.0


P1.0 Impulse Sensor 1

P4.0  Pushbutton S1
P4.1  Pushbutton S2

P2.0 UCA0TXD
P2.1 UCA0RXD

P3.7  LED 8
P3.6  LED 7
P3.5  LED 6
P3.4  LED 5
PJ.3  LED 4
PJ.2  LED 3
PJ.1  LED 2
PJ.0  LED 1

PJ.4  XIN
PJ.5  XOUT

* use input Schmitt trigger on P1.0
* use comparator CD0 on P1.0 if Schmitt trigger solution does not work

### Storing impulses
#### Store as BCD

YYMMDDhhmmss

Two bytes for each entry, 6 bytes in total.

##### Improvements

- Don't store seconds (saves 2 bytes)
- Don't store the year (saves 2 bytes)

#### Store as hex

YY: 7 bit (0-99)
MM: 4 bit (1-12)
DD: 5 bit (1-31)
hh: 5 bit (1-24)
mm: 6 bit (1-60)
ss: 6 bit (1-60)

33 bits in total (31,25% improvement compared to BCD)

##### Improvements
same as above
