/*
 * UART.h
 *
 * Created: 31/03/2025 14:48:37
 *  Author: constantinos.pavlide
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>  // This includes the WDTCR register and other necessary AVR definitions
#include <ctype.h>
#include <common.h>

//#define SCALE_FACTOR 100000
#define UART_BAUD 115200
#define MY_UBRR ((F_CPU / (8UL * UART_BAUD)) - 1)  // Double-speed
#define UART_BUFFER_SIZE 99

#ifndef UART_H_
#define UART_H_

void setup_uart(void);
uint8_t receive_uart(void);
void receive_string_uart(char *buffer, uint8_t max_length);
void send_uart(uint8_t data);
void UART_send_string(const char *str);
float extractFloat(uint8_t skipChars,volatile char uart_buffer[16]);
long long extractFloatToLong(uint8_t skipChars,volatile char uart_buffer[16]);
void read_uart_line(volatile char *buf, uint8_t maxlen);

#endif /* UART_H_ */