/*
 * PLL_M.h
 *
 * Created: 31/03/2025 15:15:37
 *  Author: constantinos.pavlide
 */ 

#ifndef PLL_S_H_
#define PLL_S_H_

#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct PLLs {
	struct STW{
		volatile uint8_t Subband;
		long long FreqMHz;
		const uint8_t *PortCS;
		const uint8_t CS;
		const uint8_t *PortFlag;
		const uint8_t LockFlag;
		const long double PFD;
	}VHF_PLL3_STW_B;	
	struct ADF5352{
		volatile uint8_t Subband;
		float FreqMHz;
		const uint8_t *PortCS;
		const uint8_t CS;
		const uint8_t *PortFlag;
		const uint8_t LockFlag;
	}_8_12Ghz_PLL2_ADF;
	
	struct Master {
		volatile uint8_t Subband;
		long long FreqMHz;
		const uint8_t CS_ATT_QPC;
		const uint8_t CS_REO_AD5270;
		const uint8_t LockFlag_100;
		const uint8_t LockFlag_3500;
	}Master1;
};

void Prepare645M();

extern struct PLLs Rf_PLL;

#endif /* PLL_M_H_ */