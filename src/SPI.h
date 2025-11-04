/*
 * SPI.h
 *
 * Created: 31/03/2025 14:31:08
 *  Author: constantinos.pavlide
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>  // This includes the WDTCR register and other necessary AVR definitions
#include <GPIOs.h>
#include <interrupts_n_timers.h>
#include <common.h>

#ifndef SPI_H_
#define SPI_H_

void setup_spi(void);
void setup_slow_spi(void);
void SPI_send8(volatile uint8_t*, uint8_t, uint8_t);
void SPI_send16(volatile uint8_t*, uint8_t, uint16_t);
void SPI_send32(volatile uint8_t*, uint8_t, uint32_t);
void SPI_send16_LSB_First(volatile uint8_t*, uint8_t , uint16_t );
uint8_t send_spi(uint8_t);
long_msg_t long_wr_spi(long_msg_t *spi_msg);
long_msg_t three_byte_wr_spi(long_msg_t *spi_msg);
short_msg_t short_wr_spi(short_msg_t *spi_msg);
uint8_t writeread_spi(uint8_t spi_msg);

#endif /* SPI_H_ */