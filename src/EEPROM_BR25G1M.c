/*
* EEPROM.c
*
* Created: 30/06/2025 13:50:11
*  Author: constantinos.pavlide
*/

#include <EEPROM_BR25G1M.h>


void init_EEPROMs()
{
	EnableSPI_FOR(SPI_route.MCU_ONLY);
	setupEEPROM(&EEPROMs_GPIOs.MCU_1_ROM);
	EnableSPI_FOR(SPI_route.PLL_M);
	setupEEPROM(&EEPROMs_GPIOs.PLLM_ROM);
	EnableSPI_FOR(SPI_route.PLL_S);
	setupEEPROM(&EEPROMs_GPIOs.PLLS_ROM);
	//EnableSPI_FOR(SPI_route.RXA);
	//setupEEPROM(&EEPROMs_GPIOs.RXA_ROM);
	EnableSPI_FOR(SPI_route.RXB);
	setupEEPROM(&EEPROMs_GPIOs.RXB_ROM);
	EnableSPI_FOR(SPI_route.MCU_ONLY);
}

void setupEEPROM(struct EEPROMs_IC *eeprom)
{
	setup_slow_spi();
	DDRK |= (1 << eeprom->CS_PIN);   // Set port output EEPROM pins are driven high by daughter board
	*(eeprom->CS_PORT) &= ~(1 << eeprom->CS_PIN);
	send_spi(Instr_Enable_Write);
	DDRK &= ~(1 << eeprom->CS_PIN);
	
	delay_us(5);
	
	DDRK |= (1 << eeprom->CS_PIN);   // Set port output EEPROM pins are driven high by daughter board
	*(eeprom->CS_PORT) &= ~(1 << eeprom->CS_PIN);
	send_spi(Instr_Status_Reg);
	send_spi(Data_Status_Reg);
	DDRK &= ~(1 << eeprom->CS_PIN);
	setup_spi();
}

void writeEEPROM(struct EEPROMs_IC *eeprom,uint8_t writeData,uint32_t address)
{
	setup_slow_spi();
	uint32_t addr = address & 0x1FFFF; //Mask rest of it
	
	uint8_t addr_byte1 = (addr >> 16) & 0x01;          // Bit 16 (MSB)
	addr_byte1 = addr_byte1 | 0x00;                    // bit1=0, bit0=MS bit of addr
	uint8_t addr_byte2 = (addr >> 8) & 0xFF;           // bits 15-8
	uint8_t addr_byte3 = addr & 0xFF;                  // bits 7-0 (LSB)
	
	DDRK |= (1 << eeprom->CS_PIN);   // Set port output EEPROM pins are driven high by daughter board
	*(eeprom->CS_PORT) &= ~(1 << eeprom->CS_PIN);
	send_spi(Instr_Enable_Write);
	DDRK &= ~(1 << eeprom->CS_PIN);
	
	DDRK |= (1 << eeprom->CS_PIN);   // Set port output EEPROM pins are driven high by daughter board
	*(eeprom->CS_PORT) &= ~(1 << eeprom->CS_PIN);
	send_spi(Instr_Write_Command); //Write command
	send_spi(addr_byte1);	 //MSB -> Bit1 = DONT CARE. Bit0 is MS bit of adress
	send_spi(addr_byte2);	 // Bit 15 - 8 address
	send_spi(addr_byte3);	 // Bit 0 - 7 address LSBs
	send_spi(writeData);
	delay_us(5);
	DDRK &= ~(1 << eeprom->CS_PIN);
	setup_spi();
}

uint8_t readEEPROM(struct EEPROMs_IC *eeprom,uint32_t address)
{
	setup_slow_spi();
	uint32_t addr = address & 0x1FFFF; //Mask rest of it
	
	uint8_t addr_byte1 = (addr >> 16) & 0x01;          // Bit 16 (MSB)
	addr_byte1 = addr_byte1 | 0x00;                    // bit1=0, bit0=MS bit of addr
	uint8_t addr_byte2 = (addr >> 8) & 0xFF;           // bits 15-8
	uint8_t addr_byte3 = addr & 0xFF;                  // bits 7-0 (LSB)
	
	DDRK |= (1 << eeprom->CS_PIN);   // Set port output EEPROM pins are driven high by daughter board
	*(eeprom->CS_PORT) &= ~(1 << eeprom->CS_PIN);
	send_spi(Instr_Read_Command); //read command
	send_spi(addr_byte1);	//MSB -> Bit1 = DONT CARE. Bit0 is MS bit of adress
	send_spi(addr_byte2);	// Bit 15 - 8 address
	send_spi(addr_byte3);	// Bit 0 - 7 address LSBs
	uint8_t recData = send_spi(0x00); //Dont care
	DDRK &= ~(1 << eeprom->CS_PIN);
	setup_spi();

	return	recData;
}

uint32_t get_calibration_address(uint8_t chain, uint8_t is_tx)
{
	long long freq_mhz_scaled;
	uint16_t index;
	
	// Get frequency based on chain and Tx/Rx selection
	if (is_tx) {
		freq_mhz_scaled = (chain == 1) ? tx.TxA.FreqMHz : tx.TxB.FreqMHz;
	} else {
		freq_mhz_scaled = (chain == 1) ? Rx_Chains.RxA.FreqMHz : Rx_Chains.RxB.FreqMHz;
	}
	
	uint32_t step = is_tx ? STEP_FREQ_MHZ_TX : STEP_FREQ_MHZ_RX; // different step size
	uint32_t freq_mhz = freq_mhz_scaled / SCALE_FACTOR;
	
	if (freq_mhz < START_FREQ_MHZ)
	freq_mhz = START_FREQ_MHZ;
	
	// Compute index to closest calibration slot
	index = (freq_mhz - START_FREQ_MHZ + step / 2) / step;
	if (index >= NUM_CAL_TABLES) index = NUM_CAL_TABLES - 1;
	
	// Interleaved layout within each section:
	// Chain 1: even slots, Chain 2: odd slots
	if (chain == 1)
	return (uint32_t)(index * 2) * TABLE_SIZE_BYTES;
	else if (chain == 2)
	return (uint32_t)(index * 2 + 1) * TABLE_SIZE_BYTES;
	else
	return 0xFFFFFFFF;  // Invalid chain
}