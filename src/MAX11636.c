/*
* MAX11636.c
*
* Created: 17/06/2025 11:20:28
*  Author: constantinos.pavlide
*/
#include <MAX11636.h>

void initADCs()
{
	setupADCTx();
	setupADCRx();
}

void setupADCTx()
{
	uint8_t RESET = MAX11636_RESET_CMD;    // Reset values
	uint8_t SETUP = MAX11636_SETUP_CMD_TX;    // MSB 01 Setup command . 00 clock timed. 10 Ref . 10 Unipolar conf follows LSB
	uint8_t CONV_REG = MAX11636_CONV_REG_CMD_TX; // MSB 1 Conversion. 0 not care. 000 Get conversion CH1. 00 Convert from Ch0 up to CH of previous byte (Ch1 here). 0 Not care  LSB
	uint8_t AVERAGE = MAX11636_AVERAGE_CMD;  // MSB 001 fixed, 111 max averaging, 00 scanning not used for this clock LSB
	
	SPI_send8(SPI_GPIOs.ADC_TX.CS_PORT,SPI_GPIOs.ADC_TX.CS_PIN,RESET); //Setup ADC configuration
	delay_us(100);
	SPI_send8(SPI_GPIOs.ADC_TX.CS_PORT,SPI_GPIOs.ADC_TX.CS_PIN,SETUP);
	delay_us(100);
	SPI_send8(SPI_GPIOs.ADC_TX.CS_PORT,SPI_GPIOs.ADC_TX.CS_PIN,AVERAGE);
	delay_us(100);
	SPI_send8(SPI_GPIOs.ADC_TX.CS_PORT,SPI_GPIOs.ADC_TX.CS_PIN,CONV_REG);
	delay_us(100);
}

void requestNewSample(bool isTx)
{
	if(isTx)
	{
		*REST.CNVST_ADC_TX.PORT &= ~(1 << REST.CNVST_ADC_TX.PIN);  // Set CNVST Low
		delay_us(1);
		*REST.CNVST_ADC_TX.PORT |= (1 << REST.CNVST_ADC_TX.PIN);   // Set CNVST High
	}
	else
	{
		*REST.CNVST_ADC_RX.PORT &= ~(1 << REST.CNVST_ADC_RX.PIN);  // Set CNVST Low
		delay_us(1);
		*REST.CNVST_ADC_RX.PORT |= (1 << REST.CNVST_ADC_RX.PIN);   // Set CNVST High
	}
}

void setupADCRx()
{
	uint8_t RESET = MAX11636_RESET_CMD;    // Reset values
	uint8_t SETUP = MAX11636_SETUP_CMD_RX;    // MSB 01 Setup command . 00 clock timed. 10 Ref . 10 Unipolar conf follows LSB
	uint8_t UNIPOLAR = MAX11636_UNIPOLAR_RX;
	uint8_t CONV_REG = MAX11636_CONV_REG_CMD_RX; // MSB 1 Conversion. 0 not care. 000 Get conversion CH1. 00 Convert from Ch0 up to CH of previous byte (Ch1 here). 0 Not care  LSB
	uint8_t AVERAGE = MAX11636_AVERAGE_CMD;  // MSB 001 fixed, 111 max averaging, 00 scanning not used for this clock LSB
	
	SPI_send8(SPI_GPIOs.ADC_RX.CS_PORT,SPI_GPIOs.ADC_RX.CS_PIN,RESET); //Setup ADC configuration
	delay_us(100);
	send_package2x8(SPI_GPIOs.ADC_RX.CS_PORT,SPI_GPIOs.ADC_RX.CS_PIN,SETUP,UNIPOLAR);
	delay_us(100);
	SPI_send8(SPI_GPIOs.ADC_RX.CS_PORT,SPI_GPIOs.ADC_RX.CS_PIN,AVERAGE);
	delay_us(100);
	SPI_send8(SPI_GPIOs.ADC_RX.CS_PORT,SPI_GPIOs.ADC_RX.CS_PIN,CONV_REG);
	delay_us(100);
}

void readRXPower()
{
	uint8_t msb, lsb;
	//uint8_t sreg = SREG;
	//cli();  // Disable interrupts
	*SPI_GPIOs.ADC_RX.CS_PORT &= ~(1 << SPI_GPIOs.ADC_RX.CS_PIN);  // CS Low
	delay_us(1);
	
	// Read 5 times and store all readings
	uint16_t readings[5];
	for(uint8_t i = 0; i < 5; i++)
	{
		msb = send_spi(0x00);
		lsb = send_spi(0x00);
		readings[i] = ((uint16_t)(msb) << 8) & 0x0F00;  // Keep bits 11:8
		readings[i] |= lsb;                              // Add bits 7:0
	}
	
	delay_us(1);
	*SPI_GPIOs.ADC_RX.CS_PORT |= (1 << SPI_GPIOs.ADC_RX.CS_PIN);  // CS High
	
	//SREG = sreg;  // Restore interrupt state
	
	// Find the last two non-zero readings
	uint16_t last_two[2] = {0, 0};
	uint8_t found = 0;
	
	// Search backwards through the readings
	for(int8_t i = 4; i >= 0 && found < 2; i--)
	{
		if(readings[i] != 0)
		{
			last_two[1 - found] = readings[i];  // Fill from back: last_two[1], then last_two[0]
			found++;
		}
	}
	
	// Assign the last two non-zero readings
	if(found >= 2)
	{
		Rx_Chains.RxA.currentADC = last_two[0];  // Second-to-last non-zero
		Rx_Chains.RxB.currentADC = last_two[1];  // Last non-zero
	}
	else
	{
		// Fallback if we don't find 2 non-zero readings
		Rx_Chains.RxA.currentADC = readings[0];
		Rx_Chains.RxB.currentADC = readings[1];
	}
	
	// Clear FIFO after reading to avoid stale data in next read.
	// SPI_send8(SPI_GPIOs.ADC_RX.CS_PORT,SPI_GPIOs.ADC_RX.CS_PIN,MAX11636_CLR_FIFO_CMD);

	//if(Rx_Chains.RxB.currentADC == 0 || Rx_Chains.RxA.currentADC == 0)
	//{
	//char buf[48]; // Enough for -2,147,483,648 plus null terminator
	//snprintf(buf, sizeof(buf), "Rx: ADC1:%u / ADC2:%u \n",Rx_Chains.RxB.currentADC,Rx_Chains.RxA.currentADC);
	//UART_send_string(buf);
	//}
}

void readTXPower()
{
	uint8_t msb, lsb;
	//uint8_t sreg = SREG;
	//cli();  // Disable interrupts
	
	*SPI_GPIOs.ADC_TX.CS_PORT &= ~(1 << SPI_GPIOs.ADC_TX.CS_PIN);  // CS Low
	
	// Read 5 times and store all readings
	uint16_t readings[5];
	for(uint8_t i = 0; i < 5; i++)
	{
		msb = send_spi(0x00);
		lsb = send_spi(0x00);
		readings[i] = (((uint16_t)(msb) << 8) & 0x0F00) | lsb;
	}
	
	*SPI_GPIOs.ADC_TX.CS_PORT |= (1 << SPI_GPIOs.ADC_TX.CS_PIN);  // CS High
	
	//SREG = sreg;  // Restore interrupt state
	
	// Find the last two non-zero readings
	uint16_t last_two[2] = {0, 0};
	uint8_t found = 0;
	
	// Search backwards through the readings
	for(int8_t i = 4; i >= 0 && found < 2; i--)
	{
		if(readings[i] != 0)
		{
			last_two[1 - found] = readings[i];  // Fill from back: last_two[1], then last_two[0]
			found++;
		}
	}
	
	// Assign the last two non-zero readings
	if(found >= 2)
	{
		tx.TxA.currentADC = last_two[0];  // Second-to-last non-zero
		tx.TxB.currentADC = last_two[1];  // Last non-zero
	}
	else
	{
		// Fallback if we don't find 2 non-zero readings
		tx.TxA.currentADC = readings[0];
		tx.TxB.currentADC = readings[1];
	}
}


/**
* @brief If previous ADC readings were faulty, will reflect in health.
*
* @return uint8_t
*/
uint8_t check_adc_health()
{
	//uint16_t readingRx = 0x00;
	uint8_t response = 0;

	if (tx.TxA.FaultyChain < 1 && tx.TxB.FaultyChain < 1) //IF ADC reading lower than value. Possible issue with ADC or Log detector
	{
		response |= (1 << 0); 
	}
	
	//*SPI_GPIOs.ADC_RX.CS_PORT &= ~(1 << SPI_GPIOs.ADC_RX.CS_PIN);  // Set PH7 (CS) Low
	//delay_us(1);
	//readingRx = send_spi(0x00) << 8;
	//readingRx &= 0b0000111100000000;
	//readingRx |= send_spi(0x00);
	//send_spi(0x00); //read and discard channel 2
	//send_spi(0x00); //read and discard channel 2
	//delay_us(1);
	//*SPI_GPIOs.ADC_RX.CS_PORT |= (1 << SPI_GPIOs.ADC_RX.CS_PIN);  // Set PH7 (CS) high
	
	response |= (1 << 1);  // Any value is considered passed. We cannot check the RX ADC because of limits 1V-2V
	
	return response;
}