/*
 * main.h
 *
 * Created: 18/06/2025 16:07:50
 *  Author: constantinos.pavlide
 */ 

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>  // Required for `bool`, `true`, `false`
#include <ctype.h>
#include <math.h>
#include <avr/io.h>  // This includes the WDTCR register and other necessary AVR definitions
#include <avr/wdt.h>
#include <MAX5742.h>
#include <MAX11636.h>
#include <MCP23S18.h>
#include <GPIOs.h>
#include <UART.h>
#include <Tx.h>
#include <Rx.h>
#include <w5500.h>
#include <TC72.h>
#include <EEPROM_BR25G1M.h>
#include <interrupts_n_timers.h>

#ifndef MAIN_H_
#define MAIN_H_

#endif /* MAIN_H_ */

void lockAllChains(void);