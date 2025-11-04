/*
* agc.h
*
* Created: 30/06/2025 16:59:54
*  Author: constantinos.pavlide
*/

#ifndef AGC_H_
#define AGC_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <EEPROM_BR25G1M.h>
#include <MAX5742.h>

typedef struct {
	volatile int16_t cbm[NUM_POINTS];
	volatile uint16_t adc[NUM_POINTS];
} CalibrationTable;

typedef struct {
	volatile uint16_t dac;
	volatile int16_t offset;
	//volatile uint16_t t_constant;
} rxCalBlock;

typedef struct {
	volatile rxCalBlock aux;
	volatile rxCalBlock main;
} rxPoints;


extern rxPoints rxa_calibration;
extern rxPoints rxb_calibration;

//extern bool agcEnable;
extern CalibrationTable txa_calibration_table;
extern CalibrationTable txb_calibration_table;


void load_Txcalibration_table(uint8_t Chain);
void load_Rxcalibration(uint8_t chain);
void write_calibration_table_to_eeprom(uint8_t chain); //Tx calibration points
void set_tx_out_power(uint8_t chain);
void calculate_rx_in_power(uint8_t chain);
void correctionloopTx(uint8_t Chain);
void correctionloopRx(uint8_t Chain);
void write_calibration_points_to_eeprom(uint8_t chain); //RX calibration points
void reset_tx_calibration(uint8_t chain);


#endif /* AGC_H_ */