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

//Convert float to int and round up/down to closest int
#define FLOAT_TO_INT(x) ((x) >= 0 ? (int)((x) + 0.5) : (int)((x)-0.5))

// scale factor for frequency calculations
#define SCALE_FACTOR 100000

#define FLAG_RESPONSE_WAIT_ms 5 //PLL WAIT BEFORE FLAG CHECK
//Tested with 8MHz SPI clock and minimum required is 4ms delay
#define EEPROM_DELAY 7 //PLL WAIT BEFORE FLAG CHECK

#define AGC_PERIOD_MS 15 //AGC iteration time
#define AGC_FAULT_DELAY_MS 333 // AGC max time to call chain as "failed"
#define AGC_MAX_FAILED_ATTEMPTS (AGC_FAULT_DELAY_MS / AGC_PERIOD_MS) //Number of iterations based on period
#define PERCENTAGE_ADC_ALLOWED 5 //Percentage allowed before failsafe triggers

/***************** DAC LIMITS *****************************/
#define DAC_MIN_TX 0    //Min DAC Tx
#define DAC_MAX_TX 4095 //Max DAC Tx
#define DAC_RESET_VALUE 1950 //For Tx only Default value to be set when enabling chain. Avoid starting from zero to avoid detected as faulty chain
#define DAC_LOOP_TX 2300 //Fixed value for when the unit is in loop mode. No log detector on loop back

#define DAC_MAX_RX 3000  //Max DAC to target VVA F2480 linearity 2458
#define DAC_MIN_RX 750  //Min DAC to target VVA F2480 linearity 956

#define START_RX_DAC_VALUE DAC_MIN_RX //Start Value must be between DAC_MAX and DAC_MIN
#define START_TX_DAC_VALUE DAC_MIN_TX //Start Value must be between DAC_MAX and DAC_MIN 
/**************************************************/

/***************** Default Frequencies **********************/
#define START_RX_FREQ 228300000LL   // 2283.0 MHz * SCALE_FACTOR
#define START_TX_FREQ  84150000LL   // 841.5 MHz * SCALE_FACTOR

/********************** EEPROM TABLE ******************/
#define START_FREQ_MHZ     60
#define STEP_FREQ_MHZ_TX      22 //Start 60Mhz with 22MHz step, gives a range 60MH-4110MHz
#define STEP_FREQ_MHZ_RX      33 //Start 60Mhz with 33MHz step, gives a range 60MH-6135MHz
#define NUM_POINTS         11
#define TABLE_SIZE_BYTES  352
#define NUM_CAL_TABLES    186

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
#define FREQ_TABLE_SIZE sizeof(freqTable.N) / sizeof(freqTable.N[0])

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
#define gstarRxBackup ((void *)0x0100) //There is a possibility for Rx EEPROM getting corrupte. We are saving gstart values in MCU EEPROM so we can easilly recover them without recall

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