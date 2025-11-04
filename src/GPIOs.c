/*
* GPIOs.c
*
* Created: 17/06/2025 13:56:41
*  Author: constantinos.pavlide
*/
#include <GPIOs.h>

struct ALL_GPIOs ETH_GPIOs = {
	.W5500.CS_PIN = PB0,
	.W5500.CS_PORT = &PORTB,
	.W5500.RESET_PIN = PE6,
	.W5500.RESET_PORT = &PORTE,
	.W5500.RDY_PIN = PH0,
	.W5500.RDY_PORT = &PINH,
	.W5500.INT_PIN = PE7,
	.W5500.INT_PORT = &PINE
};

struct ALL_GPIOs EEPROMs_GPIOs = {
	.TXA_ROM = {PK1,&PORTK,PD4,&PORTD},
	.TXB_ROM = {PK2,&PORTK,PD5,&PORTD},
	.RXA_ROM = {PK3,&PORTK,PD6,&PORTD},
	.RXB_ROM = {PK4,&PORTK,PD7,&PORTD},
	.PLLM_ROM = {PK5,&PORTK,PG5,&PORTG},
	.PLLS_ROM = {PK6,&PORTK,PG4,&PORTG},
	.MCU_1_ROM = {PK7,&PORTK,PG3,&PORTG},
	//.MCU_2_ROM = {PA0,&PORTA,}
};

struct ALL_GPIOs REST = {
	.PSU_CONTROL = {PL7,&PORTL},
	.POWER_SWITCH_SENSE = {PE5,&PINE},
	.CNVST_ADC_TX = {PD0,&PORTD},
	.CNVST_ADC_RX = {PD1,&PORTD},
	.EOC_ADC_TX = {PD2,&PIND},
	.EOC_ADC_RX = {PD3,&PIND},
	.PSU_TTL1 = {PG2,&PING},
	.PSU_TTL2 = {PG1,&PING},
	.PSU_SMB_ALERT = {PG0,&PING},
	.ETH_RESET = {PK0,&PINK},
	.LED1 = {PE2,&PORTE},
	.TXA_LOOP_SW = {PJ0,&PORTJ},
	.TXB_LOOP_SW = {PJ4,&PORTJ},
	.MAIN_VOLT_TRANSL = {PA1,&PORTA}
};

struct ALL_GPIOs PLL_GPIOs = {
	.RXA_PLLA = {PJ2,&PINJ},
	.RXA_PLLB = {PJ3,&PINJ},
	.RXB_PLLA = {PJ6,&PINJ},
	.RXB_PLLB = {PJ7,&PINJ},
	.TXA_LOGDET = {PJ1,&PINJ},
	.TXB_LOGDET = {PJ5,&PINJ},
	.PLL_M_645 = {PC7,&PINC},
	.PLL_M_100 = {PC6,&PINC},
	.PLL_M_3500 = {PC5,&PINC},
	.PLL_M_STW_VHF = {PC4,&PINC},
	.PLL_M_8_12_ADF = {PC3,&PINC},
	.PLL_S_STW_VHF = {PC2,&PINC},
	.PLL_S_3500 = {PC1,&PINC},
	.PLL_S_8_12_ADF = {PC0,&PINC}
};

struct ALL_GPIOs SPI_GPIOs = {
	.ADC_TX = {PH7,&PORTH},
	.ADC_RX = {PH6,&PORTH},
	.DAC_1 = {PH5,&PORTH},
	.DAC_2 = {PH4,&PORTH},
	.DAC_3 = {PH3,&PORTH},
	.P_EXPANDER_A = {PF0,&PORTF},
	.P_EXPANDER_B = {PF1,&PORTF},
	.P_EXPANDER_C = {PF2,&PORTF},
	.P_EXPANDER_D = {PF3,&PORTF},
	.P_EXPANDER_E = {PE4,&PORTE},
	.TXA_STW = {PH1,&PORTH},
	.TXB_STW = {PH2,&PORTH},
	.RXA_STW_1 = {PB4,&PORTB},
	.RXA_STW_2 = {PB5,&PORTB},
	.RXB_STW_1 = {PB6,&PORTB},
	.RXB_STW_2 = {PB7,&PORTB},
	.PLL_S_STW = {PL0,&PORTL},
	.PLL_M_STW_1 = {PL2,&PORTL},
	.PLL_M_STW_2 = {PL3,&PORTL},
	.PLL_S_ADF = {PL1,&PORTL},
	.PLL_M_ADF = {PL4,&PORTL},
	.ATT_QPC = {PL5,&PORTL},
	.RHEO_AD = {PL6,&PORTL},
	.TC72_MCU = {PA6,&PORTA},
	.TC72_TXA = {PA2,&PORTA},
	.TC72_TXB = {PA3,&PORTA},
	.TC72_RXA = {PA4,&PORTA},
	.TC72_RXB = {PA5,&PORTA},
	.TC72_EXTRA = {PA7,&PORTA}
};

void power_On_routine()
{
	DDRL  |= 0xE0;     // Set PL7 to output (Power control) and Attenuator CS
	*SPI_GPIOs.RHEO_AD.CS_PORT |= (1 << SPI_GPIOs.RHEO_AD.CS_PIN); //Keep rheostat CS high on put during power on
	*SPI_GPIOs.ATT_QPC.CS_PORT  &= ~(1 << SPI_GPIOs.ATT_QPC.CS_PIN); //Keep Attenuator pin low during power on
	*REST.PSU_CONTROL.PORT |= (1 << REST.PSU_CONTROL.PIN); // Power ON unit
}
	

void setup_GIO_directions()
{
	/*============= CONFIGURE PORT DIRECTIONS =======================*/
	DDRA = 0xFF;    // Set PA0-PA7 as outputs
	DDRB = 0xF7;    // All but miso output
	DDRC = 0x00;	// All flags in
	DDRD = 0xF3;    // Set PD0, PD1, PD4, PD5, PD6, PD7 as outputs, PD2, PD3 as inputs
	DDRE = 0x5E;	// Set PE0, PE5, PE7 as input, PE1, PE2, PE3, PE4, PE6 as outputs
	DDRF = 0x4F;    // Set PortF Pins 0-3 as outputs
	DDRG = 0xF8;    // Set PG3, PG4, PG5 as outputs
	DDRH = 0xFE;    // Set PH0 as input and PH1-PH6 as outputs
	DDRJ = 0x11;    // Set bits PJ4 PJ0 output
	DDRK = 0xFE;	// PK1-7 chip selects for EEPROM we set them to output
	DDRL = 0xFF;    // Set all as outputs
}

void setup_Output_state()
{
	/*============= CONFIGURE OUTPUT PORT STATES ====================*/
	// Set output states directly after configuration
	PORTA = 0x03;    // Set PA0-PA1 high, rest low=TC72 CS
	PORTB = 0xFF;    // Set PB0-PB7 high
	PORTC = 0x00;    // All inputs start low
	PORTD = 0xF3;
	PORTE = 0xF6;    // Set PE2, PE4, PE6 high
	PORTF = 0xFF;    // Set PF0–PF7 high
	PORTG = 0x38;    // Set PG3–PG5 high (bits 3, 4, 5)
	PORTH = 0xFE;    // Set PH1-PH7 high
	PORTJ = 0x00;    // Set all outputs LOW (inputs unaffected unless pull-ups are enabled)
	PORTK = 0xFE;
	PORTL = 0xFF;    // Set PL0-PL7 high
}

void configure_ports_low() //Clears all ports to avoid leakage 5V/3V3
{
	// Clear output pins only (input pins left unchanged)
	PORTL &= ~(DDRL & ~(1 << REST.PSU_CONTROL.PIN));  // Leave PL7 high (power enable)
	PORTF &= ~DDRF;
	PORTE &= ~DDRE;
	PORTG &= ~DDRG;
	PORTH &= ~DDRH;
	PORTB &= ~DDRB;
	PORTD &= ~DDRD;
	PORTA &= ~DDRA;

	// Make SPI pins PB0–PB2 inputs to prevent SDB 5V leakage
	DDRB &= ~0x07;
	
	EIMSK &= ~(1 << INT2); //Disable Interrupt routines
	EIMSK &= ~(1 << INT3); //Disable Interrupt routines

}