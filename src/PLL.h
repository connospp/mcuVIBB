/*
* PLL_M.h
*
* Created: 31/03/2025 15:15:37
*  Author: constantinos.pavlide
*/

#ifndef PLL_H_
#define PLL_H_

#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <GPIOs.h>

struct FreqCalTable {
	const uint16_t N[17];
	volatile uint16_t ATT[17];
	volatile uint16_t RHE[17];
} freqTable;

typedef struct ADF5352 {
	volatile uint8_t Subband;
	volatile float FreqMHz;
	volatile uint8_t *PortCS;
	uint8_t CS;
	volatile uint8_t *PortFlag;
	uint8_t LockFlag;
} t_ADF5352;

typedef struct STW{
	volatile uint8_t Subband;
	long long FreqMHz;
	volatile uint8_t *PortCS;
	uint8_t CS;
	volatile uint8_t *PortFlag;
	uint8_t LockFlag;
	long double PFD;
} t_STW;

typedef struct Master {
	volatile uint8_t Subband;
	long long FreqMHz;
	volatile uint8_t *Port_CS_ATT_QPC;
	uint8_t CS_ATT_QPC;
	volatile uint8_t *Port_CS_REO_AD5270;
	uint8_t CS_REO_AD5270;
	volatile uint8_t *Port_LockFlag_100;
	uint8_t LockFlag_100;
	volatile uint8_t *Port_LockFlag_3500;
	uint8_t LockFlag_3500;
	volatile uint8_t *Port_LockFlag_3500_Slave;
	uint8_t LockFlag_3500_Slave;
} t_Master;

struct PLLs {
	t_STW _645_PLL1_STW_A;
	t_STW VHF_PLL3_STW_TxA;
	t_STW VHF_PLL3_STW_TxB;
	t_ADF5352 _8_12Ghz_PLL2_ADF_TxA;
	t_ADF5352 _8_12Ghz_PLL2_ADF_TxB;
	t_Master Master1;
};

void Prepare645M(void);
void setupRheo(void);
void init_pll(void);
void load_freqTable_from_eeprom(void);
void write_freqTable_to_eeprom(void);

struct PLLs Rf_PLL;

#endif /* PLL_M_H_ */