/*
 *EEPROM_BR25G1M.h
 *
 * Created: 30/06/2025 13:50:17
 *  Author: constantinos.pavlide
 */ 


#ifndef EEPROM_BR25G1M_H_
#define EEPROM_BR25G1M_H_

#include <GPIOs.h>
#include <SPI.h>

#define Data_Status_Reg 0b00000000   // Status register,enable WPB pin and disable write whole EEPROM
#define Instr_Status_Reg 0b00000001  // Instruction to write statur reg
#define Instr_Enable_Write 0b00000110  // Instruction to enable write (software)
#define Instr_Disable_Write 0b00000100  // Insruction to enable write

#define Instr_Write_Command 0b00000010  // Instruction to write data
#define Instr_Read_Command 0b00000011   // Instruction to read


void setupEEPROM(struct EEPROMs_IC *eeprom);
void writeEEPROM(struct EEPROMs_IC *eeprom,uint8_t writeData,uint32_t address);
uint8_t readEEPROM(struct EEPROMs_IC *eeprom,uint32_t address);
uint32_t get_calibration_address(uint8_t chain, uint8_t is_tx);
void init_EEPROMs(void);




#endif /* EEPROM_H_ */