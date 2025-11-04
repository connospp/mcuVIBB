/*
 * tc72.h
 *
 * Created: 1-6-2021 12:31:35
 *  Author: arne.de_brabanter
 */ 


#ifndef TC72_H_
#define TC72_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* PROTOTYPES */
uint8_t readID_TC72(void);
void TC72_init(void);
float TC72_read_float(void);
void TC72_read(void);
uint8_t TC72_spi(uint8_t reg);
int8_t TC72_read_temperature_integer(int8_t);
float  TC72_read_temperature_fractional(void);

/* TC72 REGISTERS*/
#define TC72_REG_CONTROL_R		0x00
#define TC72_REG_CONTROL_W		0x80
#define TC72_REG_TEMP_LSB       0x01
#define TC72_REG_TEMP_MSB       0x02
#define TC72_REG_ID             0x03 

/* TC72 CONTROL REGISTER bit designations*/
#define TC72_OS		4  /* One-Shot enable */
#define TC72_SHDN	0  /* shutdown disable */

/* ID */
#define TC72_ID         0x54




#endif /* TC72_H_ */