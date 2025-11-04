/*
 * MAX5742.h
 *
 * Created: 17/06/2025 11:21:17
 *  Author: constantinos.pavlide
 */ 


#ifndef MAX5742_H_
#define MAX5742_H_

#include <SPI.h>

void setupDACTxA(void);
void setupDACTxB(void);
void setupDACRxA(void);
void setupDACRxB(void);
void send_DAC_package(volatile uint8_t *port, uint8_t pin,uint8_t channel,volatile uint16_t data);
void wakeupDACs(void);

#endif /* MAX5742_H_ */