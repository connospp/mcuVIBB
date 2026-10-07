/*
* MCP23S18.c
*
* Created: 17/06/2025 13:49:00
*  Author: constantinos.pavlide
*/

#include <MCP23S18.h>

volatile uint8_t led_port_a = 0b00000000;
volatile uint8_t led_port_b = 0b00000000;

LEDStatus ledSelection = {
	.RXA_AUX  = 0,
	.RXA_MAIN = 1,
	.TXA_MAIN = 2,
	.RXB_AUX  = 3,
	.RXB_MAIN = 4,
	.TXB_MAIN = 5,
	.REF      = 6,
	.HEALTH   = 7,
	.ALL= 0xFF
};


VoltageTranslatorStaus SPI_route ={
	.MCU_ONLY = 0,
	.TX = 8,
	.RXA = 16,
	.RXB = 32,
	.PLL_S = 48,
	.PLL_M = 64,
	.TC72 = 128,
};


void EnableSPI_FOR(uint8_t route) //Enables voltage translator for specified path. Without this, SPI signals will only reach components within the MCU board
{
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_E.CS_PORT, SPI_GPIOs.P_EXPANDER_E.CS_PIN,0b01000000,0b00010010,route); //Enable correct route for each
}

void setupPortExpPorts(volatile uint8_t *port, uint8_t pin,uint8_t pack1,uint8_t pack2,uint8_t pack3)
{
	*port &= ~(1 << pin);
	delay_us(500);
	// Send 3 packages (8 bits each)
	send_spi(pack1);  // Package 1 (example data)
	send_spi(pack2);  // Package 2 (example data)
	send_spi(pack3);  // Package 3 (example data)
	delay_us(100);
	// Raise CS to indicate the end of communication
	*port |= (1 << pin); // Set pin high
	delay_us(100);
}

void setupPortEx()
{
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_A.CS_PORT, SPI_GPIOs.P_EXPANDER_A.CS_PIN,INIT_CMD,IODIRA_CMD,0b00000000); // IODIRA:SET ALL PORTA OUTPUT Port expander A CS:PF0 U3
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_A.CS_PORT, SPI_GPIOs.P_EXPANDER_A.CS_PIN,INIT_CMD,IODIRB_CMD,0b00000000); // IODIRB:SET ALL PORTB OUTPUT Port expander A CS:PF0 U3
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_A.CS_PORT, SPI_GPIOs.P_EXPANDER_A.CS_PIN,INIT_CMD,GPPUA_CMD,0b11111111); // GPPUA: Set pullups for: Port expander A LOG DET SW
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_A.CS_PORT, SPI_GPIOs.P_EXPANDER_A.CS_PIN,INIT_CMD,GPPUB_CMD,0b11111111); // GPPUB: Set pullups for: Port expander A TX Power SW
	
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_B.CS_PORT, SPI_GPIOs.P_EXPANDER_B.CS_PIN,INIT_CMD,IODIRA_CMD,0b00000000); // IODIRA:SET ALL PORTA OUTPUT Port expander B CS:PF1
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_B.CS_PORT, SPI_GPIOs.P_EXPANDER_B.CS_PIN,INIT_CMD,IODIRB_CMD,0b00000000); // IODIRB:SET ALL PORTB OUTPUT Port expander B CS:PF1
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_B.CS_PORT, SPI_GPIOs.P_EXPANDER_B.CS_PIN,INIT_CMD,GPPUA_CMD,0b11111111); // GPPUA: Set pullups for: Port expander B LOG DET SW
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_B.CS_PORT, SPI_GPIOs.P_EXPANDER_B.CS_PIN,INIT_CMD,GPPUB_CMD,0b11111111); // GPPUB: Set pullups for: Port expander B TX Power SW
	
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_C.CS_PORT, SPI_GPIOs.P_EXPANDER_C.CS_PIN,INIT_CMD,IODIRA_CMD,0b00000000); // IODIRA:SET ALL PORTA OUTPUT Port expander C CS:PF3 U14
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_C.CS_PORT, SPI_GPIOs.P_EXPANDER_C.CS_PIN,INIT_CMD,IODIRB_CMD,0b00000000); // IODIRB:SET ALL PORTB OUTPUT Port expander C CS:PF3 U14
	
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_C.CS_PORT, SPI_GPIOs.P_EXPANDER_C.CS_PIN,INIT_CMD,GPPUA_CMD,0b01111111); // GPPUA: Set pullups for: All but GPA7 (floating)
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_C.CS_PORT, SPI_GPIOs.P_EXPANDER_C.CS_PIN,INIT_CMD,GPPUB_CMD,0b01111111); // GPPUB: Set pullups for: All but GPB7 (floating)
	
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_C.CS_PORT, SPI_GPIOs.P_EXPANDER_C.CS_PIN,INIT_CMD,GPIOA_CMD,0b01010000); //U14 PortExp_A TxA Unused lines 2550 & 2350-2550 PERM HIGH
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_C.CS_PORT, SPI_GPIOs.P_EXPANDER_C.CS_PIN,INIT_CMD,GPIOB_CMD,0b01010000); //U14 PortExp_A TxB Unused lines 2550 & 2350-2550 PERM HIGH
	
	delay_us(1000);

	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_D.CS_PORT, SPI_GPIOs.P_EXPANDER_D.CS_PIN,INIT_CMD,IODIRA_CMD,0b00000000); //IODIRA:SET ALL PORTA OUTPUT Front Panel LEDs
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_D.CS_PORT, SPI_GPIOs.P_EXPANDER_D.CS_PIN,INIT_CMD,IODIRB_CMD,0b00000000); //IODIRB:SET ALL PORTB OUTPUT
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_D.CS_PORT, SPI_GPIOs.P_EXPANDER_D.CS_PIN,INIT_CMD,GPPUA_CMD,0b11111111); //GPPUA:PULLUPS for all LEDs
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_D.CS_PORT, SPI_GPIOs.P_EXPANDER_D.CS_PIN,INIT_CMD,GPPUB_CMD,0b11111111); //GPPUA:PULLUPS for all LEDs
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_D.CS_PORT, SPI_GPIOs.P_EXPANDER_D.CS_PIN,INIT_CMD,GPIOA_CMD,0b00000000); //All LEDs start as OFF
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_D.CS_PORT, SPI_GPIOs.P_EXPANDER_D.CS_PIN,INIT_CMD,GPIOB_CMD,0b00000000); //All LEDs start as OFF
	
	delay_us(1000);

	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_E.CS_PORT, SPI_GPIOs.P_EXPANDER_E.CS_PIN,INIT_CMD,IODIRA_CMD,0b00000000); //IODIRA:SET ALL PORTA OUTPUT
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_E.CS_PORT, SPI_GPIOs.P_EXPANDER_E.CS_PIN,INIT_CMD,IODIRB_CMD,0b00000000); //IODIRB:SET ALL PORTB OUTPUT Voltage translators
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_E.CS_PORT, SPI_GPIOs.P_EXPANDER_E.CS_PIN,INIT_CMD,GPPUA_CMD,0b11111001); //GPPUA:PULLUPS for Voltage translators + LED
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_E.CS_PORT, SPI_GPIOs.P_EXPANDER_E.CS_PIN,INIT_CMD,GPPUB_CMD,0b00001111); //GPPUB:PULLUPS for RXA and RXB lines
	delay_us(1000);
	
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_E.CS_PORT, SPI_GPIOs.P_EXPANDER_E.CS_PIN,INIT_CMD,GPIOA_CMD,SPI_route.MCU_ONLY); //Disable all voltage translators
}

void manual_tx_filter(uint8_t chain,uint8_t filter_selection)
{
	struct Filter_lines *activeFilt;
	
	switch(filter_selection)
	{
		default:
		case 1:
		activeFilt = &Rf_Ctrl_lines._60_t_100M;
		break;
		case 2:
		activeFilt = &Rf_Ctrl_lines._101_t_180M;
		break;
		case 3:
		activeFilt = &Rf_Ctrl_lines._181_t_350M;
		break;
		case 4:
		activeFilt = &Rf_Ctrl_lines._351_t_520M;
		break;
		case 5:
		activeFilt = &Rf_Ctrl_lines._521_t_680M;
		break;
		case 6:
		activeFilt = &Rf_Ctrl_lines._681_t_920M;
		break;
		case 7:
		activeFilt = &Rf_Ctrl_lines._921_t_1500M;
		break;
		case 8:
		activeFilt = &Rf_Ctrl_lines._1501_t_2200M;
		break;
		case 9:
		activeFilt = &Rf_Ctrl_lines._2201_t_3200M;
		break;
		case 10:
		activeFilt = &Rf_Ctrl_lines._3201_t_4000M;
		break;
		case 11:
		activeFilt = &Rf_Ctrl_lines._4001_t_4401M;
		break;
	}
	
	if(chain == 1) //TxA
	{
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_A.CS_PORT, SPI_GPIOs.P_EXPANDER_A.CS_PIN,INIT_CMD,GPIOB_CMD,activeFilt->Sw_PortB); //U3 PortExp_A TXA SW
	}
	
	else //TxB
	{
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_B.CS_PORT,SPI_GPIOs.P_EXPANDER_B.CS_PIN,INIT_CMD,GPIOB_CMD,activeFilt->Sw_PortB); //U3 PortExp_A TXB SW
	}
}

void manual_txLogDet_filter(uint8_t chain,uint8_t filter_selection)
{
	struct Filter_Log_lines *activeLogFilt;
	
	switch(filter_selection)
	{
		case 0:
		activeLogFilt = &Rf_Ctrl_lines._60_t_630M;
		break;
		case 1:
		activeLogFilt = &Rf_Ctrl_lines._631_t_850M;
		break;
		case 2:
		activeLogFilt = &Rf_Ctrl_lines._851_t_1400M;
		break;
		case 3:
		activeLogFilt = &Rf_Ctrl_lines._1401_t_1800M;
		break;
		case 4:
		activeLogFilt = &Rf_Ctrl_lines._1801_t_2400M;
		break;
		case 5:
		activeLogFilt = &Rf_Ctrl_lines._2401_t_3100M;
		break;
		case 6:
		activeLogFilt = &Rf_Ctrl_lines._3101_t_3500M;
		break;
		case 7:
		activeLogFilt  = &Rf_Ctrl_lines._3501_t_3800M;
		break;
		case 8:
		activeLogFilt  = &Rf_Ctrl_lines._3801_t_4300M;
		break;
	}
	
	if(chain == 1) //TxA
	{
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_A.CS_PORT, SPI_GPIOs.P_EXPANDER_A.CS_PIN,INIT_CMD,GPIOA_CMD,activeLogFilt->Log_PortA); //U3 PortExp_A TXA Log
	}
	
	else //TxB
	{
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_B.CS_PORT,SPI_GPIOs.P_EXPANDER_B.CS_PIN,INIT_CMD,GPIOA_CMD,activeLogFilt->Log_PortA); //U3 PortExp_A TXB Log
	}
}

void manual_rx_filter(uint8_t chain,uint8_t filter_selection)
{
	if (chain == 1)
	{
		if (filter_selection == 0)
			Rx_Chains.RxA.Subband = 3;
		else if (filter_selection == 1)
			Rx_Chains.RxA.Subband = 1;
		else if (filter_selection == 2)
			Rx_Chains.RxA.Subband = 4;
	}
	else if (chain == 2)
	{
		if (filter_selection == 0)
			Rx_Chains.RxB.Subband = 3;
		else if (filter_selection == 1)
			Rx_Chains.RxB.Subband = 1;
		else if (filter_selection == 2)
			Rx_Chains.RxB.Subband = 4;
	}
	
	configurePortExpRx(); //Execute changes
}

void activeLeds() {
	// TXA
	if (tx.TxA.health == 1) {
		led(ledSelection.TXA_MAIN, tx.TxA.health);
	} else if (tx.TxA.health == 2 && tx.TxA.isItOn == 1 && tx.TxA.Output == 1) {
		led(ledSelection.TXA_MAIN, tx.TxA.health);
	} else {
		led(ledSelection.TXA_MAIN, 0);
	}

	// TXB
	if (tx.TxB.health == 1) {
		led(ledSelection.TXB_MAIN, tx.TxB.health);
	} else if (tx.TxB.health == 2 && tx.TxB.isItOn == 1 && tx.TxB.Output == 1) {
		led(ledSelection.TXB_MAIN, tx.TxB.health);
	} else {
		led(ledSelection.TXB_MAIN, 0);
	}

	// RXA
	if (Rx_Chains.RxA.health == 1) {
		if (Rx_Chains.RxA.Input == 1) {
			led(ledSelection.RXA_AUX, Rx_Chains.RxA.health);
			led(ledSelection.RXA_MAIN, 0);
		} else if (Rx_Chains.RxA.Input == 2) {
			led(ledSelection.RXA_AUX, 0);
			led(ledSelection.RXA_MAIN, Rx_Chains.RxA.health);
		} else {
			led(ledSelection.RXA_AUX, 0);
			led(ledSelection.RXA_MAIN, 0);
		}
	} else if (Rx_Chains.RxA.health == 2 && Rx_Chains.RxA.isItOn == 1) {
		if (Rx_Chains.RxA.Input == 1) {
			led(ledSelection.RXA_AUX, Rx_Chains.RxA.health);
			led(ledSelection.RXA_MAIN, 0);
		} else if (Rx_Chains.RxA.Input == 2) {
			led(ledSelection.RXA_AUX, 0);
			led(ledSelection.RXA_MAIN, Rx_Chains.RxA.health);
		} else {
			led(ledSelection.RXA_AUX, 0);
			led(ledSelection.RXA_MAIN, 0);
		}
	} else {
		led(ledSelection.RXA_AUX, 0);
		led(ledSelection.RXA_MAIN, 0);
	}

	// RXB
	if (Rx_Chains.RxB.health == 1) {
		if (Rx_Chains.RxB.Input == 1) {
			led(ledSelection.RXB_AUX, Rx_Chains.RxB.health);
			led(ledSelection.RXB_MAIN, 0);
		} else if (Rx_Chains.RxB.Input == 2) {
			led(ledSelection.RXB_AUX, 0);
			led(ledSelection.RXB_MAIN, Rx_Chains.RxB.health);
		} else {
			led(ledSelection.RXB_AUX, 0);
			led(ledSelection.RXB_MAIN, 0);
		}
		} else if (Rx_Chains.RxB.health == 2 && Rx_Chains.RxB.isItOn == 1) {
		if (Rx_Chains.RxB.Input == 1) {
			led(ledSelection.RXB_AUX, Rx_Chains.RxB.health);
			led(ledSelection.RXB_MAIN, 0);
		} else if (Rx_Chains.RxB.Input == 2) {
			led(ledSelection.RXB_AUX, 0);
			led(ledSelection.RXB_MAIN, Rx_Chains.RxB.health);
		} else {
			led(ledSelection.RXB_AUX, 0);
			led(ledSelection.RXB_MAIN, 0);
		}
	} else {
		led(ledSelection.RXB_AUX, 0);
		led(ledSelection.RXB_MAIN, 0);
	}
}

void led(uint8_t lednumber, uint8_t state)
{
	uint8_t new_led_port_a = led_port_a;  // Shadow copy
	uint8_t new_led_port_b = led_port_b;

	switch (lednumber) {
		// LEDs mapped to PORTB
		case 0x00: // LED0 RXA_AUX
		new_led_port_b &= ~((1 << 7) | (1 << 6));
		if (state & 0x01) new_led_port_b |= (1 << 7);
		if (state & 0x02) new_led_port_b |= (1 << 6);
		break;

		case 0x01:  // LED1 RXA_MAIN
		new_led_port_b &= ~((1 << 5) | (1 << 4));
		if (state & 0x01) new_led_port_b |= (1 << 5);
		if (state & 0x02) new_led_port_b |= (1 << 4);
		break;

		case 0x02:  // LED2 TXA_MAIN
		new_led_port_b &= ~((1 << 3) | (1 << 2));
		if (state & 0x01) new_led_port_b |= (1 << 3);
		if (state & 0x02) new_led_port_b |= (1 << 2);
		break;

		case 0x04:  // LED3 RXB_AUX
		new_led_port_b &= ~((1 << 1) | (1 << 0));
		if (state & 0x01) new_led_port_b |= (1 << 1);
		if (state & 0x02) new_led_port_b |= (1 << 0);
		break;
		
		// LEDs mapped to PORTA
		case 0x03:  // LED4 RXB_MAIN
		new_led_port_a &= ~((1 << 7) | (1 << 6));
		if (state & 0x01) new_led_port_a |= (1 << 7);
		if (state & 0x02) new_led_port_a |= (1 << 6);
		break;

		case 0x05:  // LED5 TXB_MAIN
		new_led_port_a &= ~((1 << 5) | (1 << 4));
		if (state & 0x01) new_led_port_a |= (1 << 5);
		if (state & 0x02) new_led_port_a |= (1 << 4);
		break;

		case 0x06:  // LED6 REF
		new_led_port_a &= ~((1 << 3) | (1 << 2));
		if (state & 0x01) new_led_port_a |= (1 << 3);
		if (state & 0x02) new_led_port_a |= (1 << 2);
		break;

		case 0x07:  // LED7 HEALTH
		new_led_port_a &= ~((1 << 1) | (1 << 0));
		if (state & 0x01) new_led_port_a |= (1 << 1);
		if (state & 0x02) new_led_port_a |= (1 << 0);
		break;
		
		case 0xFF:  // ALL LEDs
		new_led_port_a = 0x00;
		new_led_port_b = 0x00;
		
		if (state & 0x01) {
			new_led_port_a |= ((1 << 7) | (1 << 5) | (1 << 3) | (1 << 1)); // Red LEDs
			new_led_port_b |= ((1 << 7) | (1 << 5) | (1 << 3) | (1 << 1));
		}
		if (state & 0x02) {
			new_led_port_a |= ((1 << 6) | (1 << 4) | (1 << 2) | (1 << 0)); // Green LEDs
			new_led_port_b |= ((1 << 6) | (1 << 4) | (1 << 2) | (1 << 0));
		}
		break;

		default:
		return; // Invalid LED number
	}

	// === Only update SPI if something changed ===
	if (new_led_port_a != led_port_a) {
		led_port_a = new_led_port_a;
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_D.CS_PORT, SPI_GPIOs.P_EXPANDER_D.CS_PIN,
			INIT_CMD, GPIOA_CMD, led_port_a);
	}
	
	if(new_led_port_a != led_port_a && new_led_port_b != led_port_b) delay_us(100);
	
	if (new_led_port_b != led_port_b) {
		led_port_b = new_led_port_b;
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_D.CS_PORT, SPI_GPIOs.P_EXPANDER_D.CS_PIN,
			INIT_CMD, GPIOB_CMD, led_port_b);
	}
}
