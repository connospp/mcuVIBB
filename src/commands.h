/*
* @brief All data coming in over one of the USARTs is handled with functions
* underneath. The commands use functions in other files (led0 in led.c, ...)
*
* Commands USART0 are all ASCII based:
*   led0, led1, led2, leds: modify the LEDs: 1 = on; 0 = off; t = toggle.
*   reset: initialize the USARTs again + flush hanging data
*
*
* Company: Celestia Antwerp B.V.
* Project: IBBE
* Created: 2020-12-15
* Author : pg
* Copyright (c) Celestia Antwerp B.V.
*/

#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "MAX11636.h"
#include "Rx.h"
#include "Tx.h"
#include "common.h"
#include "SPI.h"
#include "UART.h"
#include "MCP23S18.h"
#include "w5500.h"
#include "TC72.h"
#include "GPIOs.h"
#include "interrupts_n_timers.h"
#include <avr/eeprom.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/wdt.h>


/* Notification ID */
#define CID_NAK ((uint8_t)0x00)
#define CID_ADCval ((uint8_t)0x01)
#define CID_Status ((uint8_t)0x02)
#define CID_Serial ((uint8_t)0x03)
/* definitions of the acknowledge messages */
#define ACK ((uint8_t)0x00)
#define UNKNOWN_CMD_ID ((uint8_t)0x01)
#define WRONG_PARAM ((uint8_t)0x02)
#define WRONG_CMD_SIZE ((uint8_t)0x03)
#define WRONG_DATA_SIZE ((uint8_t)0x04)
#define WRONG_RECORDMARKER ((uint8_t)0x05)
#define NOT_IN_USE ((uint8_t)0x06)
#define OTHER_ERROR ((uint8_t)0x08)

#define CMD_STR_SIZE 12

extern const char* REBOOT_CMD;

void CID_readSN(void);
void CID_readSN2(void);
void CID_readTemp(void);
void CID_version(void);
void CID_toggle_agc(uint8_t onoff);
void CID_eth_factrst(void);
uint32_t CID_health_check(void);
void CID_unit_power(uint8_t state);
void CID_select_rx_path(uint8_t chain, uint8_t path);
void CID_manual_filter_selection(uint8_t chain, uint8_t direction, uint8_t filter_selection);
void CID_set_carrier(uint8_t chain, uint8_t carr_state);
void CID_led(uint8_t lednumber, uint8_t state);
void CID_getStatus(uint8_t chain);
void CID_AGCset(uint8_t chain, uint8_t TxRx, int16_t inpt_lvl, uint16_t setpoint, uint16_t t_constant);
void CID_AGCconf(uint8_t chain, uint8_t* calib_points, uint8_t num_bytes);
void CID_loop(uint8_t chain, uint8_t path);
void CID_setDAC(uint8_t chain, uint8_t TxRx, uint8_t DAC, uint16_t dac_val);
void CID_changeIP_sub_default(uint8_t* ipparam);
void CID_changePort(uint8_t* tcpParameters);
void CID_Reference(uint8_t con);
void CID_readADC(uint8_t chain, uint8_t txOrRx);
void CID_resetCalAGC(uint8_t chain);
void CID_set_freq(uint8_t chain, uint8_t TxRx, uint16_t freq);
void CID_readsetpoint(uint8_t chain, uint8_t TxRx);
void CID_read_dac(uint8_t chain, uint8_t TxRx_select);
void CID_read_agc_calibration(uint8_t chain);
void CID_read_agc_calibration(uint8_t chain);

void send_read_agc_calib(uint8_t chain);
void send_ack(uint8_t CID,uint8_t error_id);
void send_adc_value(uint8_t chain, uint8_t ADCselect, uint16_t adc_hex);
void send_status(uint8_t chainid);
void send_health_stat(uint32_t _health);
void CID_version(void);
void send_setpoint(uint8_t chain, uint8_t txrx);
void send_dac_value(uint8_t chain, uint8_t TxRxselect);
void send_temp(void);
void send_serial_nr(uint8_t sn);
uint32_t pow16 (uint8_t x);
uint32_t convert_hex_cmd(void);
uint32_t send_spi_cmd(uint32_t tranceive);
void handle_uart_led_cmd (void);
void UART_execute_cmd(void);
uint8_t eth_recv_command(uint8_t *RX_data);
void CID_returnEthConfig();

void dump_calibration_table(const char *label,uint8_t chain);



#endif /* COMMANDS_H */