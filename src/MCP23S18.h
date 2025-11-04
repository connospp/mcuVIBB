/*
* MCP23S18.h
*
* Created: 17/06/2025 13:49:07
*  Author: constantinos.pavlide
*/

#ifndef MCP23S18_H_
#define MCP23S18_H_

#include <SPI.h>

#define INIT_CMD 0b01000000
#define IODIRA_CMD 0b00000000
#define IODIRB_CMD 0b00000001
#define GPPUA_CMD 0b00001100
#define GPPUB_CMD 0b00001101
#define GPIOA_CMD 0b00010010
#define GPIOB_CMD 0b00010011

void setupPortEx(void);
void setupPortExpPorts(volatile uint8_t *port, uint8_t pin,uint8_t pack1,uint8_t pack2,uint8_t pack3);
void EnableSPI_FOR(uint8_t route);
void led(uint8_t lednumber, uint8_t state);
void activeLeds(void);
void manual_tx_filter(uint8_t chain,uint8_t filter_selection);
void manual_txLogDet_filter(uint8_t chain,uint8_t filter_selection);
void manual_rx_filter(uint8_t chain,uint8_t filter_selection);

typedef struct  {
	uint8_t RXA_AUX;
	uint8_t RXA_MAIN;
	uint8_t TXA_MAIN;
	uint8_t RXB_AUX;
	uint8_t RXB_MAIN;
	uint8_t TXB_MAIN;
	uint8_t REF;
	uint8_t HEALTH;
	uint8_t ALL;
} LEDStatus;

typedef struct  {
	uint8_t RXA;
	uint8_t RXB;
	uint8_t TX;
	uint8_t PLL_M;
	uint8_t PLL_S;
	uint8_t TC72;
	uint8_t MCU_ONLY;
} VoltageTranslatorStaus;

LEDStatus ledSelection;
VoltageTranslatorStaus SPI_route;

#endif /* MCP23S18_H_ */