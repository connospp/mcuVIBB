/*
 * ADF5356.h
 *
 * Created: 31/03/2025 15:29:49
 *  Author: constantinos.pavlide
 */ 


#ifndef ADF5356_H_
#define ADF5356_H_

#include <stdlib.h>
#include <stdio.h>
#include <PLL.h>
#include <SPI.h>
#include <math.h>

#define ADF_DELAY_US 10

uint16_t setup_ADF(t_ADF5352 *);


#endif /* ADF5356_H_ */