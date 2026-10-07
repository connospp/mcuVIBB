#include <UART.h>

// Function to initialize UART for receiving data
void setup_uart() {
	unsigned int ubrr = MY_UBRR;

	// Enable double-speed mode before setting UBRR
	UCSR0A |= (1 << U2X0);

	// Set baud rate
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	// Enable RX, TX and RX interrupt
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	// 8 data bits, 1 stop bit, no parity
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Function to receive a byte of data from UART
uint8_t receive_uart() {
	while (!(UCSR0A & (1 << RXC0))) {
		// Wait for data to be received
	}
	return UDR0;  // Return received data from the UART Data Register
}


void send_uart(uint8_t data) {
	
	while (!(UCSR0A & (1 << UDRE0))) {
		// Wait for the UART data register to be empty
	}
	UDR0 = data;  // Send the data via UART
}

void UART_send_string(const char *str) {
	while (*str) {
		while (!(UCSR0A & (1 << UDRE0))); // Wait for the transmit buffer to be empty
		UDR0 = *str++;                        // Send the character
	}
}

float extractFloat(uint8_t skipChars, volatile char uart_buffer[16])
{
	char number_buffer[12];  // Increased size for negative and decimals (e.g., "-123.456")
	uint8_t num_index = 0;
	uint8_t i = skipChars;

	// Capture optional leading minus
	if (uart_buffer[i] == '-') {
		number_buffer[num_index++] = '-';
		i++;
	}

	// Capture digits and at most one decimal point
	uint8_t dot_seen = 0;
	for (; uart_buffer[i] != '\0' && num_index < sizeof(number_buffer) - 1; i++) {
		if (isdigit(uart_buffer[i])) {
			number_buffer[num_index++] = uart_buffer[i];
		}
		else if (uart_buffer[i] == '.' && !dot_seen) {
			number_buffer[num_index++] = '.';
			dot_seen = 1;
		}
		else {
			break;  // stop at any invalid character
		}
	}

	number_buffer[num_index] = '\0';  // Null-terminate
	return atof(number_buffer);
}

// Reads a line from UART into buf until CR/LF, null-terminated
void read_uart_line(volatile char *buf, uint8_t maxlen)
{
	uint8_t i = 0;
	uint8_t c;

	do {
		c = receive_uart();
		if (c == '\r' || c == '\n') {
			if (i == 0) {
				continue;  // leftover/duplicate terminator, ignore and keep waiting
			}
			break;         // real end of line
		}
		if (i < maxlen - 1) {
			buf[i++] = (char)c;
		}
	} while (1);

	buf[i] = '\0';
}

long long extractFloatToLong(uint8_t skipChars,volatile char uart_buffer[16]) {
	char number_buffer[12] = {0};  // Increased buffer size for safety
	uint8_t num_index = 0;
	long long result = 0;
	uint8_t isNegative = 0;
	
	// Skip the initial characters (e.g., "DAC")
	for (uint8_t i = skipChars; uart_buffer[i] != '\0' && num_index < sizeof(number_buffer) - 1; i++) {
		if (isdigit((unsigned char)uart_buffer[i]) || uart_buffer[i] == '.' || uart_buffer[i] == '-') {
			if (uart_buffer[i] == '-') {
				isNegative = 1;  // Mark negative number
				continue;  // Skip '-' symbol for conversion
			}
			number_buffer[num_index++] = uart_buffer[i];
		}
	}
	number_buffer[num_index] = '\0';  // Null-terminate the string

	// Ensure a valid number was extracted
	if (num_index == 0) {
		return 0;  // No valid number found, return 0
	}

	// Convert the extracted number string to long long manually
	uint8_t decimalFound = 0;
	uint8_t decimalCount = 0;
	long long integerPart = 0;
	long long fractionalPart = 0;

	for (uint8_t i = 0; number_buffer[i] != '\0'; i++) {
		if (number_buffer[i] == '.') {
			decimalFound = 1;
			continue;
		}
		
		if (!decimalFound) {
			integerPart = integerPart * 10 + (number_buffer[i] - '0');
			} else {
			decimalCount++;
			fractionalPart = fractionalPart * 10 + (number_buffer[i] - '0');
		}
	}

	// Scale the fractional part to match the SCALE_FACTOR precision
	while (decimalCount < 5) {
		fractionalPart *= 10;
		decimalCount++;
	}

	// Combine integer and fractional parts
	result = integerPart * SCALE_FACTOR + (fractionalPart * SCALE_FACTOR / 100000);

	// If the number was negative, make the result negative
	if (isNegative) {
		result = -result;
	}

	// Return the scaled value
	return result;
}