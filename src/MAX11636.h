/*
 * MAX11636.h
 *
 * Created: 17/06/2025 11:21:32
 *  Author: constantinos.pavlide
 */ 


#ifndef MAX11636_H_
#define MAX11636_H_

#include <SPI.h>

#define MAX11636_RESET_CMD 0b00010000   // Reset command
//#define MAX11636_CLR_FIFO_CMD 0b00001000  // Clear FIFO command
#define MAX11636_AVERAGE_CMD 0b00110000   // Averaging: max averaging, no scan "32sample averaging = 0b00111100" "8sample averaging = 0b00110100"  "4sample averaging = 0b00110000" "1sample no averaging = 0b00100000"

#define MAX11636_SETUP_CMD_TX 0b01000100   // Setup: clock-timed, external ref, unipolar
#define MAX11636_CONV_REG_CMD_TX 0b10001000   // Conversion: get CH1 result, CH0/CH1 range

#define MAX11636_SETUP_CMD_RX 0b01000110;     // MSB 01 Setup command . 00 clock timed. 10 Ref . 10 Unipolar conf follows LSB
#define MAX11636_UNIPOLAR_RX 0b11000000;  // MSB 1 Input AIN0 AIN1 used as unipolar. 1 Input AIN2 AIN3 used as unipolar. 0 AIN4 AIN5.  0 AIN6 AIN7. rest "dontcare" LSB
#define MAX11636_CONV_REG_CMD_RX 0b10010000;  // MSB 1 Conversion. 0 not care. 010 Get conversion CH3. 00 Convert from Ch0 up to CH of previous byte (Ch2 here. Differential inputs operate as one. We are expected to receive only 2 samples AN0 AN2. AN1 and AN3 operates as differential). 0 Not care  LSB

void initADCs(void);
void setupADCTx(void);
void setupADCRx(void);
void readTXPower(void);
void readRXPower(void);
void send_package2x8(volatile uint8_t *port, uint8_t pin,uint8_t data,uint8_t data2);
uint8_t check_adc_health(void);
void requestNewSample(bool isTx);

#endif /* MAX11636_H_ */