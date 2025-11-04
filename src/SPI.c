#include <SPI.h>

void setup_spi() {
	/*============= CONFIGURE SPI ====================*/
	// Set SPI pins for Master mode, MOSI, SCK as outputs, MISO as input
	DDRB |= (1 << DDB2) | (1 << DDB1); // MOSI, SCK as outputs
	DDRB &= ~(1 << DDB3);              // MISO as input

	SPCR = (1 << SPE) | (1 << MSTR);   // Master mode, no prescaler (SPR1=0, SPR0=0)
	SPSR = (1 << SPI2X);               // Double SPI speed: F_CPU / 2 = 8 MHz
}

void setup_slow_spi() { //EEPROM requires slower SPI. Can work with 4MHz but safely set 500KHz
    // Enable SPI, Master mode, set clock prescaler to 32 (SPR1=1, SPR0=0, SPI2X=1)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
    SPSR = (1 << SPI2X);
}



uint8_t send_spi(uint8_t data) {
	SPDR = data;  // Load data into SPI data register
	while (!(SPSR & (1 << SPIF))) {
		// Wait for transmission to complete
	}
	return SPDR;
}

void SPI_send16_LSB_First(volatile uint8_t *port, uint8_t pin, uint16_t data) {
	uint8_t high_byte = (data >> 8) & 0xFF;  // Extract the high byte
	uint8_t low_byte = data & 0xFF;          // Extract the low byte
	
	SPCR |= (1 << DORD);			// Change SPI mode LSB first
	*port &= ~(1 << pin);
	delay_us(50);
	send_spi(low_byte);
	send_spi(high_byte);
	delay_us(50);
	*port |= (1 << pin); // Set pin high
	SPCR&= ~(1 << DORD);			// Change SPI mode

}

void send_package2x8(volatile uint8_t *port, uint8_t pin,uint8_t data,uint8_t data2) {
	*port &= ~(1 << pin);
	send_spi(data);
	send_spi(data2);
	*port |= (1 << pin); // Set pin high
}

void SPI_send8(volatile uint8_t *port, uint8_t pin,uint8_t data) {
	*port &= ~(1 << pin);
	send_spi(data);
	*port |= (1 << pin); // Set pin high
}

void SPI_send16(volatile uint8_t *port, uint8_t pin, uint16_t data) {
	uint8_t high_byte = (data >> 8) & 0xFF;  // Extract the high byte
	uint8_t low_byte = data & 0xFF;          // Extract the low byte
	
	*port &= ~(1 << pin);
	delay_us(3);
	send_spi(high_byte);
	send_spi(low_byte);
	delay_us(3);
	*port |= (1 << pin); // Set pin high
}

void SPI_send32(volatile uint8_t *port, uint8_t pin,uint32_t data) {
	*port &= ~(1 << pin);
	delay_us(10);
	send_spi((data >> 24) & 0xFF); // Send MSB first
	send_spi((data >> 16) & 0xFF);
	send_spi((data >> 8) & 0xFF);
	send_spi(data & 0xFF); // Send LSB last
	*port |= (1 << pin); // Set pin high
	delay_us(10);
}



/*
* @brief Transmit 4 bytes of data on SPI interface
*
* spi_msg: 32bit: 4 byte array to transmit
*
*/
long_msg_t long_wr_spi(long_msg_t *spi_msg)
{
	volatile long_msg_t rx_msg;

	/* loop through 4 bytes of cmd and transmit them */
	for (int8_t i = 3; i >= 0; i--)
	{
		rx_msg.msg_arr[i] = writeread_spi(spi_msg->msg_arr[i]);
	}

	return rx_msg;
}


/*
* @brief Transmit 3 bytes of data on SPI interface
*
* spi_msg: 32bit: 4 byte array to transmit
*
*/
long_msg_t three_byte_wr_spi(long_msg_t *spi_msg)
{
	volatile long_msg_t rx_msg;

	/* loop through lowest 3 bytes of cmd and transmit them */
	for (int8_t i = 2; i >= 0; i--)
	{
		rx_msg.msg_arr[i] = writeread_spi(spi_msg->msg_arr[i]);
	}

	return rx_msg;
}


/*
* @brief Transmit 2 bytes of data on SPI interface
*
* spi_msg: 16bit: 2 byte array to transmit
*
*/
short_msg_t short_wr_spi(short_msg_t *spi_msg)
{
	volatile short_msg_t rx_msg;

	/* loop through 2 bytes of cmd and transmit them */
	rx_msg.shrt_arr[1] = writeread_spi(spi_msg->shrt_arr[1]);
	rx_msg.shrt_arr[0] = writeread_spi(spi_msg->shrt_arr[0]);
	
	return rx_msg;
}



/*
* @brief Transmit 1 byte of data on SPI interface
*                 Send a byte to the slave and simultaneously read a byte back from the slave
* @param spi_msg 1 byte data to send
*/
uint8_t writeread_spi(uint8_t spi_msg)
{
	/* set next byte on interface */
	SPDR = spi_msg; //SPDR = 8-bit

	/* wait until transmitted (check status reg SPSR)
	the SPIF flag is set if transmit ends */
	while(!(SPSR & (1<<SPIF)))
	;
	return SPDR; //Return read SPI value
}