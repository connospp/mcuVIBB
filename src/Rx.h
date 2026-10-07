/*
* Rx.h
*
* Created: 19/05/2025 16:24:20
*  Author: constantinos.pavlide
*/


#ifndef RX_H_
#define RX_H_

#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <STW81200.h>
#include <MCP23S18.h>
#include <GPIOs.h>
#include <EEPROM_BR25G1M.h>
#include <agc.h>

struct Rx_status {
	volatile uint8_t *PortCS1;
	uint8_t CS_1;
	volatile uint8_t *PortFlag1;
	uint8_t LockFlag_1;
	long double PFD;
};

#define  Subband1_END_MHz 1021
#define  Subband2_END_MHz 2001
#define  Subband3_END_MHz 2321 //(used to be 2301, changed to correlate with Rx cal tables)
#define  Subband4_END_MHz 6661

struct Rx_PLLs {
	volatile uint8_t Subband;  // Subband 1�4
	volatile long long FreqMHz;         // Frequency in Mhz*SCALE_FACTOR
	volatile uint8_t Input;    // 0 = Loopback, 1 = AUX, 2 = Main_Input
	volatile uint8_t agcEnable;    // 0 = OFF 1 =ON (When off will only read ADC because both chains on 1 adc)
	volatile uint16_t timeConst;
	volatile uint8_t isItOn;
	volatile uint8_t health;   // 1 = BAD 2 = GOOD
	volatile uint16_t targetADC;
	volatile uint16_t currentADC;
	volatile int16_t carrierPower;
	volatile uint16_t currentDACValue[3]; // 3 DAC values per chain
	volatile uint8_t *DAC_PortCS[3];
	uint8_t DAC_CS[3];
	uint8_t dacChan[3];
	struct Rx_status STW_PLL1;
	struct Rx_status STW_PLL2;
};

// Container for all Rx chains
struct RxContainer {
	struct Rx_PLLs RxA;
	struct Rx_PLLs RxB;
};

struct RxContainer Rx_Chains;

void configurePortExpRx(void);
void change_Rx_Frequency(long long freq,uint8_t Chain);
void Calculate_Frequency_Rx(uint8_t Chain);
void init_rx_chains(void);

#endif /* RX_H_ */