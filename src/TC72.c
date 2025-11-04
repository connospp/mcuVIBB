/*
* TC72.c
*
* Created: 1-6-2021 12:31:10
*  Author: arne.de_brabanter
*/
#include "SPI.h"
#include "common.h"
#include "UART.h"
#include "TC72.h"
#include "MCP23S18.h"

/*
* @brief Checks the ID of the temperature sensor TC72.
*
* @return Return 1: ID matches => peripheral communication OK
*         Return 0: ID doesn't match => peripheral communication NOK
*/
uint8_t readID_TC72()
{
	uint8_t response = 0x00;
	
	response = TC72_spi(TC72_REG_ID);
	
	if (response == TC72_ID)
	{
		//char print[32];     //declaration of String to print
		//sprintf(print, "TC72 detected!\r\n"); //print TC72 = detected
		//log_msg(print, 16); //send uart message
		return 1;
	}
	//char print[32];     //declaration of String to print
	//sprintf(print,"ID TC72 not found!\r\n"); //print which adc = not found
	//log_msg(print, 20); //send uart message
	return 0;
}

/*
* @brief Initialize TC72 temperature sensor. Set continuous conversion mode.
*
*/
void TC72_init()
{
	SPCR |= (1 << CPOL) | (1<<CPHA);
	*SPI_GPIOs.TC72_MCU.CS_PORT |= (1 << SPI_GPIOs.TC72_MCU.CS_PIN); // Set TC72 high
	delay_us(3);
	send_spi(0x80); // Write command (MSB high)
	send_spi(0x04); // Write data
	delay_us(3);
	*SPI_GPIOs.TC72_MCU.CS_PORT &= ~(1 << SPI_GPIOs.TC72_MCU.CS_PIN); // Set TC72 low
	SPCR &= ~(1<<CPHA);
	SPCR &= ~(1<<CPOL);
}

uint8_t TC72_spi(uint8_t reg) {
	uint8_t value;	
	EnableSPI_FOR(SPI_route.TC72);
	SPCR |= (1 << CPOL) | (1<<CPHA);
	*SPI_GPIOs.TC72_MCU.CS_PORT |= (1 << SPI_GPIOs.TC72_MCU.CS_PIN); // Set TC72 high
	send_spi(reg); // Read command (MSB low)
	value = send_spi(0x00); // Read data
	delay_us(3);
	*SPI_GPIOs.TC72_MCU.CS_PORT &= ~(1 << SPI_GPIOs.TC72_MCU.CS_PIN); // Set TC72 low
	SPCR &= ~(1<<CPHA);
	SPCR &= ~(1<<CPOL);
	EnableSPI_FOR(SPI_route.MCU_ONLY);
	return value;
	
}

// Read integer temperature (MSB)
int8_t TC72_read_temperature_integer(int8_t cmd) {
	return (int8_t)TC72_spi(cmd); // Read MSB register (integer part)
}

// Read fractional temperature (LSB)
float  TC72_read_temperature_fractional() {
	uint8_t lsb = TC72_spi(0x01); // Read LSB register
	//return lsb;
	return (lsb >> 6) * 0.25; // Fractional part in 0.25�C increments
}

/*
* @brief Read out the temperature of TC72 in hex.
*
* @return hex temperature in 2-complement.
*/
float TC72_read_float()
{
	//char buffer[20];         // Buffer to hold the string representation
	int8_t integer_temp;
	float fractional_temp;
	float temperature = 0;
	
	integer_temp = TC72_read_temperature_integer(0x02); // Integer temperature
	fractional_temp = TC72_read_temperature_fractional(); // Fractional temperature
	temperature = integer_temp + fractional_temp;
	//sprintf(buffer, "%.2f", temperature); // If not working see this https://startingelectronics.org/articles/atmel-AVR-8-bit/print-float-atmel-studio-7/
	//UART_send_string(buffer); //Output channel one
	//send_uart('\n');
	
	return temperature;	
}

void TC72_read()
{
	temperature.shrt_arr[1] = TC72_read_temperature_integer(0x02); // Integer temperature
	temperature.shrt_arr[0] = TC72_read_temperature_integer(0x01); // Fractional temperature
	
}
