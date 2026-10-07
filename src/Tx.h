/*
* Tx.h
*
* Created: 31/03/2025 15:56:57
*  Author: constantinos.pavlide
*/
#ifndef TX_H_
#define TX_H_

#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ADF5356.h>
#include <STW81200.h>
#include <MCP23S18.h>
#include <agc.h>
#include <GPIOs.h>

#define BEAT_FREQ 6125
#define Z11_CENTER_FREQ 725
#define LOW_NOISE_SOURCE 3000

#define FIF_LOG_FREQ 870L  // Log Det IF


struct Tx_Ctrl {
	struct Filter_lines{
		const uint8_t Sw_PortB;
	}_60_t_100M,_101_t_180M,_181_t_350M,_351_t_520M,_521_t_680M,_681_t_920M,_921_t_1500M,_1501_t_2200M,_2201_t_3200M,_3201_t_4000M,_4001_t_4401M;
	struct Filter_Log_lines{
		const uint8_t Log_PortA;
	}_60_t_630M,_631_t_850M,_851_t_1400M,_1401_t_1800M,_1801_t_2400M,_2401_t_3100M,_3101_t_3500M,_3501_t_3800M,_3801_t_4300M;
};

struct Tx_status{
	volatile long long FreqMHz;			// Frequency in Mhz*SCALE_FACTOR
	volatile uint8_t Output;            // 0 = Loopback, 1 = Main_Input
	volatile uint8_t agcEnable;         // 0 = OFF 1 =ON (When off will only read ADC because both chains on 1 adc)
	volatile uint16_t timeConst;		// AGC time constant
	volatile uint8_t isItOn;			// 1=ON 0=OFF
	volatile uint8_t health;	        // 1 = BAD 2 = GOOD
	volatile uint16_t targetADC;		// AGC ADC value you wish for
	volatile uint16_t minAllowedADC;		// AGC ADC that lower than that, ADC or Log detector has problem
	volatile uint16_t currentADC;		// AGC ADC real value
	volatile int16_t carrierPower;
	volatile uint16_t currentDACValue[3]; // 3 DAC values per chain
	volatile uint8_t *DAC_PortCS[3];
	uint8_t DAC_CS[3];
	const uint8_t dacChan[3];
	volatile int8_t FaultyChain; //If Fault detected set Flag to 1. Avoid transmitting at max power if Log det or ADC is faulty
	volatile int8_t failedADCattempts;
	volatile int8_t uncalibratedFreq;
};

struct Tx_chains {
	struct Tx_status TxA;
	struct Tx_status TxB;
};

struct Tx_PLLs {
	t_STW STW_TxA;
	t_STW STW_TxB;
};

void change_Tx_Frequency(long long freq,uint8_t Chain);
void Calculate_Frequency_LogDet(uint8_t Chain);
void Calculate_Frequency_Tx(uint8_t Chain);
void setFilters(uint8_t Chain);
void setRheoAt(uint16_t ADF_N);
void init_tx_chains(void);
void setTxPath(uint8_t path, uint8_t chain);

extern struct Tx_chains tx;
extern struct Tx_Ctrl Rf_Ctrl_lines;
struct Tx_PLLs Tx_STW;


#endif /* TX_H_ */