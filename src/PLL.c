/*
* PLL_M.c
*
* Created: 31/03/2025 14:00:48
*  Author: constantinos.pavlide
*/
#include <PLL.h>
#include <Tx.h>

#define PFD_FREQ 25.0L

struct FreqCalTable freqTable = {
	.N   = {71,73,75,77,79,81,83,85,87,89,91,93,95,97,99,101,103},  // Replace with actual 17 values if needed
	.ATT = { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  8,  8},
	.RHE = {65,65,65,65,65,65,65,65,65,65,65,65,65,65,65, 65, 65}
};

void init_pll(void) {
	Rf_PLL._645_PLL1_STW_A.Subband = 1;
	Rf_PLL._645_PLL1_STW_A.FreqMHz = 1000;
	Rf_PLL._645_PLL1_STW_A.PortCS = SPI_GPIOs.PLL_M_STW_1.CS_PORT;
	Rf_PLL._645_PLL1_STW_A.CS = SPI_GPIOs.PLL_M_STW_1.CS_PIN;
	Rf_PLL._645_PLL1_STW_A.PortFlag = PLL_GPIOs.PLL_M_645.PORT;
	Rf_PLL._645_PLL1_STW_A.LockFlag = PLL_GPIOs.PLL_M_645.PIN;
	Rf_PLL._645_PLL1_STW_A.PFD = PFD_FREQ;

	Rf_PLL.VHF_PLL3_STW_TxA.Subband = 1;
	Rf_PLL.VHF_PLL3_STW_TxA.FreqMHz = 1000;
	Rf_PLL.VHF_PLL3_STW_TxA.PortCS = SPI_GPIOs.PLL_M_STW_2.CS_PORT;
	Rf_PLL.VHF_PLL3_STW_TxA.CS = SPI_GPIOs.PLL_M_STW_2.CS_PIN;
	Rf_PLL.VHF_PLL3_STW_TxA.PortFlag = PLL_GPIOs.PLL_M_STW_VHF.PORT;
	Rf_PLL.VHF_PLL3_STW_TxA.LockFlag = PLL_GPIOs.PLL_M_STW_VHF.PIN;
	Rf_PLL.VHF_PLL3_STW_TxA.PFD = PFD_FREQ;

	Rf_PLL.VHF_PLL3_STW_TxB.Subband = 1;
	Rf_PLL.VHF_PLL3_STW_TxB.FreqMHz = 1000;
	Rf_PLL.VHF_PLL3_STW_TxB.PortCS = SPI_GPIOs.PLL_S_STW.CS_PORT;
	Rf_PLL.VHF_PLL3_STW_TxB.CS = SPI_GPIOs.PLL_S_STW.CS_PIN;
	Rf_PLL.VHF_PLL3_STW_TxB.PortFlag = PLL_GPIOs.PLL_S_STW_VHF.PORT;
	Rf_PLL.VHF_PLL3_STW_TxB.LockFlag = PLL_GPIOs.PLL_S_STW_VHF.PIN;
	Rf_PLL.VHF_PLL3_STW_TxB.PFD = PFD_FREQ;

	Rf_PLL._8_12Ghz_PLL2_ADF_TxA.Subband = 1;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxA.FreqMHz = 1000;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxA.PortCS = SPI_GPIOs.PLL_M_ADF.CS_PORT;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxA.CS = SPI_GPIOs.PLL_M_ADF.CS_PIN;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxA.PortFlag = PLL_GPIOs.PLL_M_8_12_ADF.PORT;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxA.LockFlag = PLL_GPIOs.PLL_M_8_12_ADF.PIN;
	
	Rf_PLL._8_12Ghz_PLL2_ADF_TxB.Subband = 1;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxB.FreqMHz = 1000;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxB.PortCS = SPI_GPIOs.PLL_S_ADF.CS_PORT;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxB.CS = SPI_GPIOs.PLL_S_ADF.CS_PIN;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxB.PortFlag = PLL_GPIOs.PLL_S_8_12_ADF.PORT;
	Rf_PLL._8_12Ghz_PLL2_ADF_TxB.LockFlag = PLL_GPIOs.PLL_S_8_12_ADF.PIN;
	
	Rf_PLL.Master1.Port_CS_ATT_QPC = SPI_GPIOs.ATT_QPC.CS_PORT;
	Rf_PLL.Master1.CS_ATT_QPC = SPI_GPIOs.ATT_QPC.CS_PIN;
	Rf_PLL.Master1.Port_CS_REO_AD5270 = SPI_GPIOs.RHEO_AD.CS_PORT;
	Rf_PLL.Master1.CS_REO_AD5270 = SPI_GPIOs.RHEO_AD.CS_PIN;
	
	Rf_PLL.Master1.Port_LockFlag_100 = PLL_GPIOs.PLL_M_100.PORT;
	Rf_PLL.Master1.LockFlag_100 = PLL_GPIOs.PLL_M_100.PIN;
	Rf_PLL.Master1.Port_LockFlag_3500 = PLL_GPIOs.PLL_M_3500.PORT;
	Rf_PLL.Master1.LockFlag_3500 = PLL_GPIOs.PLL_M_3500.PIN;
	Rf_PLL.Master1.Port_LockFlag_3500_Slave = PLL_GPIOs.PLL_S_3500.PORT;
	Rf_PLL.Master1.LockFlag_3500_Slave = PLL_GPIOs.PLL_S_3500.PIN;
	
	Prepare645M();
	setupRheo(); //Setup rheo must be before SetupPortExp
	load_freqTable_from_eeprom();
	//write_freqTable_to_eeprom();
}



void Prepare645M()
{
	led(ledSelection.HEALTH,1);
	if((*(Rf_PLL._645_PLL1_STW_A.PortFlag) & (1 << Rf_PLL._645_PLL1_STW_A.LockFlag)) == 0)	{
		setup_STW(&Rf_PLL._645_PLL1_STW_A,0,0,0,0);
	}
	led(ledSelection.HEALTH,2);
}

void setupRheo()
{
	SPCR |= (1 << CPHA); // Change SPI mode
	delay_us(100);
	SPI_send16(Rf_PLL.Master1.Port_CS_REO_AD5270,Rf_PLL.Master1.CS_REO_AD5270,0x2400); //Wakeup rheostad
	delay_us(100);
	SPI_send16(Rf_PLL.Master1.Port_CS_REO_AD5270,Rf_PLL.Master1.CS_REO_AD5270,0x1C03); //Enables wiper to be moved
	delay_us(100);
	SPI_send16(Rf_PLL.Master1.Port_CS_REO_AD5270,Rf_PLL.Master1.CS_REO_AD5270,0x07FF); //Set wiper to MAX
	delay_us(100);
	SPCR &= ~(1 << CPHA); // Reset SPI mode
}

void write_freqTable_to_eeprom()
{
	struct EEPROMs_IC *eeprom = &EEPROMs_GPIOs.PLLM_ROM;
	uint32_t base_address = 0;	
	EnableSPI_FOR(SPI_route.PLL_M);
	
	// Write ATT array (17 elements)
	for (int i = 0; i < 17; i++) {
		writeEEPROM(eeprom, (uint8_t)freqTable.ATT[i], base_address++);
		delay_ms(EEPROM_DELAY);
	}

	// Write RHE array (17 elements)
	for (int i = 0; i < 17; i++) {
		writeEEPROM(eeprom, (uint8_t)freqTable.RHE[i], base_address++);
		delay_ms(EEPROM_DELAY);
	}
	
	EnableSPI_FOR(SPI_route.MCU_ONLY);
}

void load_freqTable_from_eeprom()
{
	struct EEPROMs_IC *eeprom = &EEPROMs_GPIOs.PLLM_ROM;
	uint32_t base_address = 0;
	EnableSPI_FOR(SPI_route.PLL_M);

	// Read ATT array (17 elements)
	for (int i = 0; i < 17; i++) {
		freqTable.ATT[i] = readEEPROM(eeprom, base_address++);
		delay_ms(1);
	}

	// Read RHE array (17 elements)
	for (int i = 0; i < 17; i++) {
		freqTable.RHE[i] = readEEPROM(eeprom, base_address++);
		delay_ms(1);
	}

	EnableSPI_FOR(SPI_route.MCU_ONLY);
}