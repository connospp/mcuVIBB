/*
* MAX5742.c
*
* Created: 17/06/2025 11:20:54
*  Author: constantinos.pavlide
*/
#include <MAX5742.h>

void send_DAC_package(volatile uint8_t *port, uint8_t pin,uint8_t channel,volatile uint16_t data) {
	
	//char buf[48]; // Enough for -2,147,483,648 plus null terminator
	//snprintf(buf, sizeof(buf), "PIN:%u DAC V:%u channel:%u",pin,data,channel);
	//UART_send_string(buf);
	//UART_send_string("\n");
	
	uint16_t command = 0x0000;        // Start with all zeros
	SPCR |= (1 << CPHA);			// Change SPI mode
	
	command |= (channel << 12);        // Channel selection (bits 11–8)
	command |= (data & 0x0FFF);       // 12-bit DAC data (bits 11–0)

	//uint8_t sreg = SREG; //store ISR state
	//cli();  // Disable ISR while setting DAC
	uint8_t high_byte = (command >> 8) & 0xFF;  // Extract the high byte
	uint8_t low_byte = command & 0xFF;          // Extract the low byte
	
	*port &= ~(1 << pin);
	
	// Send high byte
	send_spi(high_byte);
	
	// Send low byte
	send_spi(low_byte);
	//SREG = sreg; //Enable ISR

	// Disable chip select (SS)
	*port |= (1 << pin);
	SPCR &= ~(1 << CPHA); // Reset SPI mode
}

void wakeupDACs()
{
	send_DAC_package(SPI_GPIOs.DAC_3.CS_PORT,SPI_GPIOs.DAC_3.CS_PIN,0x0F,0b000000010000); //DAC 3
	send_DAC_package(SPI_GPIOs.DAC_2.CS_PORT,SPI_GPIOs.DAC_2.CS_PIN,0x0F,0b000000010000); //DAC 2
	send_DAC_package(SPI_GPIOs.DAC_1.CS_PORT,SPI_GPIOs.DAC_1.CS_PIN,0x0F,0b000000010000); //DAC 1
	
	setupDACTxA();
	setupDACTxB();
	setupDACRxA();
	setupDACRxB();
}

void setupDACTxA()
{
	/*###########TXA DACS #################*/
	send_DAC_package(SPI_GPIOs.DAC_1.CS_PORT,SPI_GPIOs.DAC_1.CS_PIN,0,tx.TxA.currentDACValue[0]);
	send_DAC_package(SPI_GPIOs.DAC_1.CS_PORT,SPI_GPIOs.DAC_1.CS_PIN,1,tx.TxA.currentDACValue[1]);
	send_DAC_package(SPI_GPIOs.DAC_1.CS_PORT,SPI_GPIOs.DAC_1.CS_PIN,2,tx.TxA.currentDACValue[2]);
}

void setupDACTxB()
{ //StartDACValue
	/*###########TXB DACS #################*/
	send_DAC_package(SPI_GPIOs.DAC_1.CS_PORT,SPI_GPIOs.DAC_1.CS_PIN,3,tx.TxB.currentDACValue[0]);
	send_DAC_package(SPI_GPIOs.DAC_2.CS_PORT,SPI_GPIOs.DAC_2.CS_PIN,0,tx.TxB.currentDACValue[1]);
	send_DAC_package(SPI_GPIOs.DAC_2.CS_PORT,SPI_GPIOs.DAC_2.CS_PIN,1,tx.TxB.currentDACValue[2]);
}

void setupDACRxA()
{
	/*###########RXA DACS #################*/
	send_DAC_package(SPI_GPIOs.DAC_3.CS_PORT,SPI_GPIOs.DAC_3.CS_PIN,0,Rx_Chains.RxA.currentDACValue[0]);
	send_DAC_package(SPI_GPIOs.DAC_2.CS_PORT,SPI_GPIOs.DAC_2.CS_PIN,2,Rx_Chains.RxA.currentDACValue[1]);
	send_DAC_package(SPI_GPIOs.DAC_2.CS_PORT,SPI_GPIOs.DAC_2.CS_PIN,3,Rx_Chains.RxA.currentDACValue[2]);
}

void setupDACRxB()
{
	/*###########RXB DACS #################*/
	send_DAC_package(SPI_GPIOs.DAC_3.CS_PORT,SPI_GPIOs.DAC_3.CS_PIN,1, Rx_Chains.RxB.currentDACValue[0]);
	send_DAC_package(SPI_GPIOs.DAC_3.CS_PORT,SPI_GPIOs.DAC_3.CS_PIN,2, Rx_Chains.RxB.currentDACValue[1]);
	send_DAC_package(SPI_GPIOs.DAC_3.CS_PORT,SPI_GPIOs.DAC_3.CS_PIN,3, Rx_Chains.RxB.currentDACValue[2]);
}