/*
 * timers.h
 *
 * Created: 31/03/2025 14:34:52
 *  Author: constantinos.pavlide
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>  // This includes the WDTCR register and other necessary AVR definitions
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <UART.h>
#include <MAX11636.h>
#include <w5500.h>
#include <avr/sleep.h>
#include <MCP23S18.h>
#include <commands.h>
#include <agc.h>
#include <common.h>
//#include <util/delay.h>


#ifndef TIMERS_H_
#define TIMERS_H_

#define TWO_SECONDS 265  // Wait for power ON
#define FIVE_SECONDS 666  // Wait for etherner reset
#define MANUAL_DELAY_2SEC 200000L  //Wait for power OFF

extern volatile uint32_t perif_health;
extern volatile bool eth_reset_pressed;
extern volatile uint32_t eth_reset_pressed_time;
extern volatile uint32_t timer_ticks;  // Variable to track the timer ticks (in ms)
extern volatile char uart_buffer[UART_BUFFER_SIZE];

void setup_timer4(void); //Used for health check
void setup_timer3(void); //Request new ADC sample
void setup_timer1(void); //Interruprs
void delay_us(uint16_t delay);
//void configure_ports_low(void); //Sets all ports to low to avoid leakage 5V/3V3
void setup_ext_interrupt(void);
void delay_ms(uint16_t delay);
void interruptHandler(void);
void powerHandling(void);
void RebootHandling(void);

ISR(TIMER1_COMPA_vect);
ISR(TIMER2_COMPA_vect);
ISR(TIMER3_COMPA_vect);
ISR(INT2_vect); //ADC readings
ISR(INT5_vect); //Front switch On/OFF
ISR(INT7_vect); //Ethernet interrupt 

#endif /* TIMERS_H_ */