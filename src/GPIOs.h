/*
* GPIOs.h
*
* Created: 17/06/2025 13:56:50
*  Author: constantinos.pavlide
*/
#ifndef GPIOS_H_
#define GPIOS_H_

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>  // This includes the WDTCR register and other necessary AVR definitions

struct ALL_GPIOs{
	struct SPI_IC{
		const uint8_t CS_PIN;
		volatile  uint8_t *CS_PORT;
	}ADC_TX,ADC_RX,DAC_1,DAC_2,DAC_3,P_EXPANDER_A,P_EXPANDER_B,P_EXPANDER_C,P_EXPANDER_D,P_EXPANDER_E,TXA_STW,TXB_STW,RXA_STW_1,RXA_STW_2,RXB_STW_1,RXB_STW_2,PLL_S_STW,PLL_M_STW_1,PLL_M_STW_2,PLL_S_ADF,PLL_M_ADF,ATT_QPC,RHEO_AD,TC72_MCU,TC72_TXA,TC72_TXB,TC72_RXA,TC72_RXB,TC72_EXTRA;

	struct PLL_LOCK{
		const uint8_t PIN;
		volatile  uint8_t *PORT;
	}RXA_PLLA,RXA_PLLB,RXB_PLLA,RXB_PLLB,TXA_LOGDET,TXB_LOGDET,PLL_M_645,PLL_M_100,PLL_M_3500,PLL_M_STW_VHF,PLL_M_8_12_ADF,PLL_S_STW_VHF,PLL_S_3500,PLL_S_8_12_ADF;

	struct GPIOs{
		const uint8_t PIN;
		volatile  uint8_t *PORT;
	}PSU_CONTROL,POWER_SWITCH_SENSE,CNVST_ADC_TX,CNVST_ADC_RX,EOC_ADC_TX,EOC_ADC_RX,PSU_TTL1,PSU_TTL2,PSU_SMB_ALERT,ETH_RESET,LED1,TXA_LOOP_SW,TXB_LOOP_SW,MAIN_VOLT_TRANSL;

	struct ETH_IC{
		const uint8_t CS_PIN;
		volatile  uint8_t *CS_PORT;
		const uint8_t RESET_PIN;
		volatile  uint8_t *RESET_PORT;
		const uint8_t RDY_PIN;
		volatile  uint8_t *RDY_PORT;
		const uint8_t INT_PIN;
		volatile  uint8_t *INT_PORT;
	}W5500;
	
	struct EEPROMs_IC{
		const uint8_t CS_PIN;
		volatile  uint8_t *CS_PORT;
		const uint8_t WPB_PIN;
		volatile  uint8_t *WPB_PORT;
	}MCU_1_ROM,MCU_2_ROM,TXA_ROM,TXB_ROM,RXA_ROM,RXB_ROM,PLLM_ROM,PLLS_ROM;
};

void setup_GIO_directions(void);
void setup_Output_state(void);
void power_On_routine(void);
void configure_ports_low(void);
void readUartBuff(void);

extern struct ALL_GPIOs SPI_GPIOs;
extern struct ALL_GPIOs PLL_GPIOs;
extern struct ALL_GPIOs REST;
extern struct ALL_GPIOs ETH_GPIOs;
extern struct ALL_GPIOs EEPROMs_GPIOs;

#endif /* GPIOS_H_ */