/**
* @file common.h
* @brief Specifies all common definitions used by a lot of files
*
* This file contains the definition of the macros, constants and functions
* which are required a lot of other files.
*
* Company: Celestia Antwerp B.V.
* Project: IBBE
* Created: 2020-12-15
* Author : pg
* Copyright (c) Celestia Antwerp B.V.
*/

#ifndef COMMON_H
#define COMMON_H

#define F_CPU 16000000UL

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

// scale factor for frequency calculations
#define SCALE_FACTOR 100000
//Convert float to int and round up/down to closest int
#define FLOAT_TO_INT(x) ((x) >= 0 ? (int)((x) + 0.5) : (int)((x)-0.5))
//PLL WAIT BEFORE FLAG CHECK
#define FLAG_RESPONSE_WAIT_ms 5
//Tested with 8MHz SPI clock and minimum required is 4ms delay
#define EEPROM_DELAY 5

/***************** DAC LIMITS *****************************/
#define DAC_MIN 750 //Min DAC to target VVA F2480 linearity 956
#define DAC_MAX 3000 //Max DAC to target VVA F2480 linearity 2458
#define START_DAC_VALUE DAC_MIN //Start Value must be between DAC_MAX and DAC_MIN 
/**************************************************/

/***************** AGC default **********************/
#define START_RX_FREQ 228300000LL   // 2283.0 MHz * SCALE_FACTOR
#define START_TX_FREQ  84150000LL   // 841.5 MHz * SCALE_FACTOR

/********************** TX AGC TABLE ******************/
#define START_FREQ_MHZ     60
#define STEP_FREQ_MHZ_TX      22 //Start 60Mhz with 22MHz step, gives a range 60MH-4110MHz
#define STEP_FREQ_MHZ_RX      33 //Start 60Mhz with 33MHz step, gives a range 60MH-6135MHz
#define NUM_POINTS         11
#define TABLE_SIZE_BYTES  352
#define NUM_CAL_TABLES    186
#define DAC_STEP_BIGGEST 4095
//#define DAC_STEP_BIG 250
//#define DAC_STEP_MID 35
#define DAC_STEP_SMALL 1


/******************** CONFIGURABLE VARIABLES *******************************/
#ifndef VERSION_N1
#define VERSION_N1 ((uint8_t)2)         // BIG change (e.g. interface changes)
#endif
#ifndef VERSION_N3
#define VERSION_N2 ((uint8_t)0)         // MSB changeset
#define VERSION_N3 ((uint8_t)0)         // LSB changeset
#endif

/**************************************************************************/

/************  LED registers  **************/
extern volatile uint8_t led_port_a;
extern volatile uint8_t led_port_b;
/**************************************************************************/

/* types */
typedef union {
	uint16_t shrt;
	uint8_t shrt_arr[2];
} short_msg_t;

typedef union {
	uint32_t msg;
	uint8_t msg_arr[4];
} long_msg_t;

/* message sizes */
#define MAX_CMD_SIZE 32
#define MAX_BIN_MSG_SIZE 1024
#define MIN_BIN_MSG_SIZE 5
#define ADDR_SIZE 4
#define LEN_SIZE 2
#define CAL_TABLE_SIZE NUM_POINTS

/* message variables */
char cmd[MAX_CMD_SIZE];
uint8_t cmd_i;

/* constants for the ETHERNET CONTROLLER */
#define SOCKET_0 0
#define SOCKET_1 1
#define RECORDMARKER_LENGTH 4

/* Addresses in EEPROM used by boot loader DO NOT CHANGE !!! */
#define ip_p  ((void *)0x0010)
#define tcp_p ((void *)0x0030)
#define mac_p ((void *)0x0040)
#define sn1_p ((void *)0x0050)
#define sn2_p ((void *)0x0060)
#define fwupgr_p ((void *)0x0070)

volatile short_msg_t temperature;

#define MAX_DATA_LEN 64

uint8_t macAddress[6];      // mac-address
uint8_t ipAddress[4];       // ip-address
uint8_t subAddress[4];      // subnet mask-address
uint8_t gateAddress[4];     // default gateway-address
uint8_t portNumber[2];      // port number


struct eth_Flags {
	uint8_t F_connected;
	uint8_t F_fin;
	uint8_t reset;
} eth_status;

volatile struct interrupts {
	volatile uint8_t powerToggle;
	volatile uint8_t uart;
	volatile uint8_t eth;
	volatile uint8_t timer;
	volatile uint8_t health;
	volatile uint8_t agcTx;
	volatile uint8_t agcRx;
	volatile uint8_t ADC_Tx_Ready;
	volatile uint8_t ADC_Rx_Ready;
} int_flags;

//void default_leds(void);
//void UART_send_string(const char *str);

#endif /* COMMON_H */