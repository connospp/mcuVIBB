/*
 * STW81200.h
 *
 * Created: 31/03/2025 15:05:39
 *  Author: constantinos.pavlide
 */ 


#ifndef STW81200_H_
#define STW81200_H_

#include <stdlib.h>
#include <stdio.h>
#include <PLL.h>
#include <Tx.h>
#include <Rx.h>
#include <SPI.h>
#include <math.h>

#define VCO_MIN 3000
#define VCO_MAX 6000
#define PLL_DELAY_US 10

struct Rx_status;   // Forward declaration
struct Rx_PLLs;   // Forward declaration

void setup_STW(t_STW*, uint16_t, uint32_t, uint32_t, uint8_t);
void setup_STW_Rx(struct Rx_status *activePLL,uint16_t ST0,uint32_t ST1,uint32_t ST2,uint8_t ST6);
uint8_t setVCODivisionBits(uint16_t);
void Calculate_STW(t_STW*);
void Calculate_STW_Rx(struct Rx_PLLs*);


#endif /* STW81200_H_ */