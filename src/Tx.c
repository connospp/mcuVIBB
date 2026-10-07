/*
* Tx.c
*
* Created: 31/03/2025 14:00:26
*  Author: constantinos.pavlide
*/

#include <Tx.h>

#define PFD_FREQ 1.0L

struct Tx_chains tx = {
	.TxA = {
		.FreqMHz=START_TX_FREQ,
		.Output = 1,   // 0 = Loopback, 1 = Main_Input
		.isItOn = 0,
		.health = 2,
		.targetADC = 0x00,
		.minAllowedADC = 0x00,
		.carrierPower = -100,
		.agcEnable = 0,
		.currentDACValue={START_TX_DAC_VALUE,START_TX_DAC_VALUE,START_TX_DAC_VALUE},
		.failedADCattempts =0, // Counter that counts how many times ADC values were found to be under min allowed
		.dacChan={0,1,2},
		.FaultyChain = 0, // 0-> OK 1-> FAULT
		.uncalibratedFreq = 0
	},
	
	.TxB = {
		.FreqMHz=START_TX_FREQ,
		.Output = 1,
		.isItOn = 0,
		.health = 2,
		.targetADC = 0x00,
		.minAllowedADC = 0x00,
		.carrierPower = -200,
		.agcEnable = 0,
		.currentDACValue={START_TX_DAC_VALUE,START_TX_DAC_VALUE,START_TX_DAC_VALUE},
		.failedADCattempts =0, // Counter that counts how many times ADC values were found to be under min allowed
		.dacChan={3,0,1},
		.uncalibratedFreq = 0
	}
};

struct Tx_Ctrl Rf_Ctrl_lines = {
	._60_t_100M.Sw_PortB =  0xD0,
	._60_t_630M.Log_PortA = 0xD0,
	
	//._101_t_180M.Sw_PortB = 0x1A,
	._101_t_180M.Sw_PortB = 0x58,
	._631_t_850M.Log_PortA = 0x58,

	//._181_t_350M.Sw_PortB = 0x29,
	._181_t_350M.Sw_PortB = 0x94,
	._851_t_1400M.Log_PortA = 0x94,
	
	//._351_t_520M.Sw_PortB = 0x38, OK
	._351_t_520M.Sw_PortB = 0x1C,
	._1401_t_1800M.Log_PortA = 0x1C,
	
	//._521_t_680M.Sw_PortB = 0x47, OK
	._521_t_680M.Sw_PortB = 0xE2,
	._1801_t_2400M.Log_PortA = 0xE2,//0b11100010
	
	//._681_t_920M.Sw_PortB = 0x56, OK
	._681_t_920M.Sw_PortB = 0x6A,
	._2401_t_3100M.Log_PortA = 0x6A,
	
	//._921_t_1500M.Sw_PortB = 0x65, OK
	._921_t_1500M.Sw_PortB = 0xA6,
	._3101_t_3500M.Log_PortA = 0xA6,
	
	//._1501_t_2200M.Sw_PortB = 0x74, OK
	._1501_t_2200M.Sw_PortB = 0x2E,
	._3501_t_3800M.Log_PortA = 0x2E,
	
	//._2201_t_3200M.Sw_PortB = 0x83,
	._2201_t_3200M.Sw_PortB = 0xC1,
	._3801_t_4300M.Log_PortA = 0xC1,
	
	//._3201_t_4000M.Sw_PortB = 0x92,
	._3201_t_4000M.Sw_PortB = 0x49,
	//._3201_t_4000M.Log_PortA = 0x00,
	
	//._4001_t_4401M.Sw_PortB = 0xA1,
	._4001_t_4401M.Sw_PortB = 0x85,
	//._4001_t_4401M.Log_PortA = 0x00
};

void init_tx_chains() {
	
	Tx_STW.STW_TxA.FreqMHz = 84150000;
	Tx_STW.STW_TxA.PortCS = SPI_GPIOs.TXA_STW.CS_PORT;
	Tx_STW.STW_TxA.CS = SPI_GPIOs.TXA_STW.CS_PIN;
	Tx_STW.STW_TxA.PortFlag = PLL_GPIOs.TXA_LOGDET.PORT;
	Tx_STW.STW_TxA.LockFlag = PLL_GPIOs.TXA_LOGDET.PIN;
	Tx_STW.STW_TxA.PFD = PFD_FREQ;
	tx.TxA.DAC_CS[0] = tx.TxA.DAC_CS[1] = tx.TxA.DAC_CS[2] = SPI_GPIOs.DAC_1.CS_PIN;
	tx.TxA.DAC_PortCS[0] =  tx.TxA.DAC_PortCS[1] = tx.TxA.DAC_PortCS[2] = SPI_GPIOs.DAC_1.CS_PORT;
	
	Tx_STW.STW_TxB.FreqMHz = 84150000;
	Tx_STW.STW_TxB.PortCS = SPI_GPIOs.TXB_STW.CS_PORT;
	Tx_STW.STW_TxB.CS = SPI_GPIOs.TXB_STW.CS_PIN;
	Tx_STW.STW_TxB.PortFlag = PLL_GPIOs.TXB_LOGDET.PORT;
	Tx_STW.STW_TxB.LockFlag = PLL_GPIOs.TXB_LOGDET.PIN;
	Tx_STW.STW_TxB.PFD = PFD_FREQ;
	tx.TxB.DAC_CS[0]  = SPI_GPIOs.DAC_1.CS_PIN;
	tx.TxB.DAC_CS[1] = tx.TxB.DAC_CS[2] = SPI_GPIOs.DAC_2.CS_PIN;
	tx.TxB.DAC_PortCS[0] = SPI_GPIOs.DAC_1.CS_PORT;
	tx.TxB.DAC_PortCS[1] = tx.TxB.DAC_PortCS[2]  = SPI_GPIOs.DAC_2.CS_PORT;
}

void change_Tx_Frequency(long long freq,uint8_t Chain)
{
	if(Chain == 1)
	{
		tx.TxA.FreqMHz = freq;
		EnableSPI_FOR(SPI_route.PLL_M);
	}
	if(Chain == 2)
	{
		tx.TxB.FreqMHz = freq;
		EnableSPI_FOR(SPI_route.PLL_S);
	}
	
	Calculate_Frequency_Tx(Chain); //Setup frequency
	delay_ms(50);
	EnableSPI_FOR(SPI_route.TX);
	Calculate_Frequency_LogDet(Chain);
	delay_ms(50);
	EnableSPI_FOR(SPI_route.MCU_ONLY);
	
	load_Txcalibration_table(Chain);
	setFilters(Chain); //Filter path for Tx
	//write_calibration_table_to_eeprom(Chain);
}

void Calculate_Frequency_LogDet(uint8_t Chain)
{
	t_STW *Tx_LogDet;
	long long freq;
	
	if(Chain == 1)
	{
		Tx_LogDet = &Tx_STW.STW_TxA;
		freq = tx.TxA.FreqMHz;
	}
	else if (Chain == 2)
	{
		Tx_LogDet = &Tx_STW.STW_TxB;
		freq = tx.TxB.FreqMHz;
	}
	else return;
	
	if(freq < 3100*SCALE_FACTOR )
	{
		Tx_LogDet->FreqMHz = freq + (FIF_LOG_FREQ*SCALE_FACTOR);
	}
	else
	{
		Tx_LogDet->FreqMHz = freq - (FIF_LOG_FREQ*SCALE_FACTOR);
	}
	
	Calculate_STW(Tx_LogDet); //Setup and Lock PLLC
}

void Calculate_Frequency_Tx(uint8_t Chain)
{
	struct Tx_status *SelectedChain;
	t_ADF5352 *_8_12_PLL;
	t_STW *_3G_PLL;
	uint8_t LockFlag;
	volatile uint8_t *LockFlagPort;
	
	if(Chain == 1 )
	{
		SelectedChain = &tx.TxA;
		_8_12_PLL = &Rf_PLL._8_12Ghz_PLL2_ADF_TxA;
		_3G_PLL = &Rf_PLL.VHF_PLL3_STW_TxA;
		LockFlag = Rf_PLL.Master1.LockFlag_3500;
		LockFlagPort = Rf_PLL.Master1.Port_LockFlag_3500;
	}
	else if (Chain == 2)
	{
		SelectedChain = &tx.TxB;
		_8_12_PLL = &Rf_PLL._8_12Ghz_PLL2_ADF_TxB;
		_3G_PLL = &Rf_PLL.VHF_PLL3_STW_TxB;
		LockFlag = Rf_PLL.Master1.LockFlag_3500_Slave;
		LockFlagPort = Rf_PLL.Master1.Port_LockFlag_3500_Slave;
	}
	else return;
	
	long long freq = SelectedChain->FreqMHz;
	float demicalFreq = (float)freq / SCALE_FACTOR;
	_8_12_PLL->FreqMHz = demicalFreq;
	uint16_t ADF_N = setup_ADF(_8_12_PLL);
	long long ADF_Freq = 6250000*2*ADF_N; // Setup and Lock PLLB
	long long IF_Z75 = ADF_Freq - freq;
	long long IF_Z58 = IF_Z75 - BEAT_FREQ * SCALE_FACTOR;
	long long F_VCO = IF_Z58 + Z11_CENTER_FREQ * SCALE_FACTOR;
	_3G_PLL->FreqMHz =  F_VCO - LOW_NOISE_SOURCE* SCALE_FACTOR;
	//_3G_PLL->FreqMHz = (long long)((F_VCO - (float)LOW_NOISE_SOURCE) * (double)SCALE_FACTOR);
	
	if(_3G_PLL->FreqMHz < 165*SCALE_FACTOR || _3G_PLL->FreqMHz > 415*SCALE_FACTOR) return; //Invalid frequencies
	
	uint8_t attempts = 0;
	do{
		Calculate_STW(_3G_PLL); //Setup and Lock PLLC
		
		attempts++;
		delay_ms(FLAG_RESPONSE_WAIT_ms);
		if (attempts >= 10) {
			UART_send_string("ERROR: STW 3.5G PLL failed to lock\r\n");
			break;
		}
	}while((*LockFlagPort & (1 << (LockFlag))) == 0); //Check second flag
	
	setRheoAt(ADF_N);
}

void setRheoAt(uint16_t ADF_N)
{
	uint16_t Rheo = 0;
	float Att = 0;
	int found = 0;

	for (int i = 0; i < CAL_TABLE_SIZE; i++) {
		if (freqTable.N[i] == ADF_N) {
			Att = freqTable.ATT[i];
			Rheo = freqTable.RHE[i];
			found = 1;
			break;
		}
	}

	if (!found) {
		return; // ADF_N not found in table
	}

	uint8_t atten_value = ((uint8_t)(Att * 4)) & 0x7F;
	uint16_t reg_value = (uint16_t)atten_value;
	SPI_send16_LSB_First(Rf_PLL.Master1.Port_CS_ATT_QPC, Rf_PLL.Master1.CS_ATT_QPC, reg_value);

	uint16_t percent_value = (((uint32_t)Rheo * 1023) / 100) & 0x03FF;
	reg_value = (0x01 << 10) | percent_value;

	SPCR |= (1 << CPHA);
	delay_us(10);
	SPI_send16(Rf_PLL.Master1.Port_CS_REO_AD5270, Rf_PLL.Master1.CS_REO_AD5270, reg_value);
	delay_us(10);
	SPCR &= ~(1 << CPHA);
}

void setTxPath(uint8_t path, uint8_t chain) //0 Loop, 1 Main AGC not turned ON automatically. Must be done manually
{
	if(chain == 1)
	{
		tx.TxA.Output = path; // Save current state
		if(path == 0)
		{
			tx.TxA.currentDACValue[0] = tx.TxA.currentDACValue[1] = tx.TxA.currentDACValue[2] = DAC_LOOP_TX; // NO AGC on loopback so we set power to fix value
			setupDACTxA();
			
			tx.TxA.agcEnable = 0;//LogDetector is located on the Main output. Hence no effect on internal loop and causes to amplify leakage
			//and causes a big spike on the output when returning to main loop
			*REST.TXA_LOOP_SW.PORT |= (1 << REST.TXA_LOOP_SW.PIN);
			return;
		}
		tx.TxA.currentDACValue[0] = tx.TxA.currentDACValue[1] = tx.TxA.currentDACValue[2] = DAC_RESET_VALUE; // Before enabling AGC, Reset DAC value to avoid tripping log det fault
		setupDACTxA();
		
		tx.TxA.agcEnable = 1;
		*REST.TXA_LOOP_SW.PORT &= ~(1 << REST.TXA_LOOP_SW.PIN);
	}
	else
	{
		tx.TxB.Output = path;
		if(path == 0)
		{
			tx.TxB.currentDACValue[0] = tx.TxB.currentDACValue[1] = tx.TxB.currentDACValue[2] = DAC_LOOP_TX; // NO AGC on loopback so we set power to fix value
			setupDACTxB();
			
			tx.TxB.agcEnable = 0;
			*REST.TXB_LOOP_SW.PORT |= (1 << REST.TXB_LOOP_SW.PIN);
			return;
		}
		tx.TxB.currentDACValue[0] = tx.TxB.currentDACValue[1] = tx.TxB.currentDACValue[2] = DAC_RESET_VALUE; // Before enabling AGC, Reset DAC value to avoid tripping log det fault
		setupDACTxB();
		
		tx.TxB.agcEnable = 1;
		*REST.TXB_LOOP_SW.PORT &= ~(1 << REST.TXB_LOOP_SW.PIN);
	}
}


void setFilters(uint8_t Chain)
{
	/* FIRST PART SETS PATH FILTER */
	struct Filter_lines *activeFilt;
	struct Filter_Log_lines *activeLogFilt;
	//struct Tx_status *SelectedChain;
	long long freq;
	
	if(Chain == 1)
	{
		freq = tx.TxA.FreqMHz;
	}
	else // (Chain == 2)
	{
		freq = tx.TxB.FreqMHz;
	}

	
	if(freq > 60 * SCALE_FACTOR && freq <= 100 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._60_t_100M;
	}
	else if(freq <= 180 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._101_t_180M;
	}
	else if (freq <= 350 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._181_t_350M;
	}
	else if (freq <= 520 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._351_t_520M;
	}
	else if (freq <= 680 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._521_t_680M;
	}
	else if (freq <= 920 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._681_t_920M;
	}
	else if (freq <= 1500 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._921_t_1500M;
	}
	else if (freq <= 2200 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._1501_t_2200M;
	}
	else if (freq <= 3200 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._2201_t_3200M;
	}
	else if (freq <= 4000 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._3201_t_4000M;
	}
	else // (freq <= 4400 * SCALE_FACTOR)
	{
		activeFilt = &Rf_Ctrl_lines._4001_t_4401M;
	}
	
	/* SECOND PART SETS LOG DETECTOR FILTER */
	if(freq > 60 * SCALE_FACTOR && freq <= 630 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._60_t_630M;
	}
	else if(freq <= 850 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._631_t_850M;
	}
	else if (freq <= 1400 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._851_t_1400M;
	}
	else if (freq <= 1800 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._1401_t_1800M;
	}
	else if (freq <= 2400 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._1801_t_2400M;
	}
	else if (freq <= 3100 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._2401_t_3100M;
	}
	else if (freq <= 3500 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._3101_t_3500M;
	}
	else if (freq <= 3800 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._3501_t_3800M;
	}
	else // (freq <= 4300 * SCALE_FACTOR)
	{
		activeLogFilt = &Rf_Ctrl_lines._3801_t_4300M;
	}
	
	if(Chain == 1) //TxA
	{
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_A.CS_PORT, SPI_GPIOs.P_EXPANDER_A.CS_PIN,INIT_CMD,GPIOA_CMD,activeLogFilt->Log_PortA); //U3 PortExp_A TXA Log
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_A.CS_PORT, SPI_GPIOs.P_EXPANDER_A.CS_PIN,INIT_CMD,GPIOB_CMD,activeFilt->Sw_PortB); //U3 PortExp_A TXA SW
	}
	
	else if(Chain == 2) //TxB
	{
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_B.CS_PORT,SPI_GPIOs.P_EXPANDER_B.CS_PIN,INIT_CMD,GPIOA_CMD,activeLogFilt->Log_PortA); //U3 PortExp_A TXB Log
		setupPortExpPorts(SPI_GPIOs.P_EXPANDER_B.CS_PORT,SPI_GPIOs.P_EXPANDER_B.CS_PIN,INIT_CMD,GPIOB_CMD,activeFilt->Sw_PortB); //U3 PortExp_A TXB SW
	}
}
