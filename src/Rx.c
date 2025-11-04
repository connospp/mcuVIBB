/*
* Rx.c
*
* Created: 19/05/2025 16:24:09
*  Author: constantinos.pavlide
*/
#include <Rx.h>

#define PFD_FREQ 25.0L

void init_rx_chains() {
	Rx_Chains.RxA.Subband = 3;
	Rx_Chains.RxA.FreqMHz = START_RX_FREQ;
	Rx_Chains.RxA.Input = 2;  // 0 = Loopback, 1 = AUX, 2 = Main_Input
	Rx_Chains.RxA.agcEnable = 0;
	Rx_Chains.RxA.isItOn = 0;
	Rx_Chains.RxA.health = 1;
	Rx_Chains.RxA.currentDACValue[0] = START_DAC_VALUE;
	Rx_Chains.RxA.currentDACValue[1] = START_DAC_VALUE;
	Rx_Chains.RxA.currentDACValue[2] = START_DAC_VALUE;
	Rx_Chains.RxA.targetADC = 0x00;
	Rx_Chains.RxA.currentADC = 0x00;
	Rx_Chains.RxA.carrierPower= 0x00;
	Rx_Chains.RxA.DAC_CS[0] = Rx_Chains.RxA.DAC_CS[1] = SPI_GPIOs.DAC_2.CS_PIN;
	Rx_Chains.RxA.DAC_CS[2] = SPI_GPIOs.DAC_3.CS_PIN;
	Rx_Chains.RxA.DAC_PortCS[0] = Rx_Chains.RxA.DAC_PortCS[1] = SPI_GPIOs.DAC_2.CS_PORT;
	Rx_Chains.RxA.DAC_PortCS[2] = SPI_GPIOs.DAC_3.CS_PORT;
	Rx_Chains.RxA.dacChan[0] = 2;
	Rx_Chains.RxA.dacChan[1] = 3;
	Rx_Chains.RxA.dacChan[2] = 0;

	Rx_Chains.RxA.STW_PLL1.PortCS1 = SPI_GPIOs.RXA_STW_1.CS_PORT;
	Rx_Chains.RxA.STW_PLL1.CS_1 = SPI_GPIOs.RXA_STW_1.CS_PIN;
	Rx_Chains.RxA.STW_PLL1.PortFlag1 = PLL_GPIOs.RXA_PLLA.PORT;
	Rx_Chains.RxA.STW_PLL1.LockFlag_1 = PLL_GPIOs.RXA_PLLA.PIN;
	Rx_Chains.RxA.STW_PLL1.PFD = PFD_FREQ;

	Rx_Chains.RxA.STW_PLL2.PortCS1 = SPI_GPIOs.RXA_STW_2.CS_PORT;
	Rx_Chains.RxA.STW_PLL2.CS_1 = SPI_GPIOs.RXA_STW_2.CS_PIN;
	Rx_Chains.RxA.STW_PLL2.PortFlag1 = PLL_GPIOs.RXA_PLLB.PORT;
	Rx_Chains.RxA.STW_PLL2.LockFlag_1 =PLL_GPIOs.RXA_PLLB.PIN;
	Rx_Chains.RxA.STW_PLL2.PFD = PFD_FREQ;

	Rx_Chains.RxB.Subband = 3;
	Rx_Chains.RxB.FreqMHz = START_RX_FREQ;
	Rx_Chains.RxB.Input = 2;  // 0 = Loopback, 1 = AUX, 2 = Main_Input
	Rx_Chains.RxB.agcEnable = 0;
	Rx_Chains.RxB.isItOn = 0;
	Rx_Chains.RxB.health = 1;
	Rx_Chains.RxB.currentDACValue[0] = START_DAC_VALUE;
	Rx_Chains.RxB.currentDACValue[1] = START_DAC_VALUE;
	Rx_Chains.RxB.currentDACValue[2] = START_DAC_VALUE;
	Rx_Chains.RxB.targetADC = 0x00;
	Rx_Chains.RxB.currentADC = 0x00;
	Rx_Chains.RxB.carrierPower= 0x00;
	Rx_Chains.RxB.DAC_CS[0] = Rx_Chains.RxB.DAC_CS[1] = Rx_Chains.RxB.DAC_CS[2] =SPI_GPIOs.DAC_3.CS_PIN;
	Rx_Chains.RxB.DAC_PortCS[0] = Rx_Chains.RxB.DAC_PortCS[1] = Rx_Chains.RxB.DAC_PortCS[2] = SPI_GPIOs.DAC_3.CS_PORT;
	Rx_Chains.RxB.dacChan[0] = 1;
	Rx_Chains.RxB.dacChan[1] = 2;
	Rx_Chains.RxB.dacChan[2] = 3;
	
	Rx_Chains.RxB.STW_PLL1.PortCS1 = SPI_GPIOs.RXB_STW_1.CS_PORT;
	Rx_Chains.RxB.STW_PLL1.CS_1 = SPI_GPIOs.RXB_STW_1.CS_PIN;
	Rx_Chains.RxB.STW_PLL1.PortFlag1 = PLL_GPIOs.RXB_PLLA.PORT;
	Rx_Chains.RxB.STW_PLL1.LockFlag_1 = PLL_GPIOs.RXB_PLLA.PIN;
	Rx_Chains.RxB.STW_PLL1.PFD = PFD_FREQ;

	Rx_Chains.RxB.STW_PLL2.PortCS1 = SPI_GPIOs.RXB_STW_2.CS_PORT;
	Rx_Chains.RxB.STW_PLL2.CS_1 = SPI_GPIOs.RXB_STW_2.CS_PIN;
	Rx_Chains.RxB.STW_PLL2.PortFlag1 = PLL_GPIOs.RXB_PLLB.PORT;
	Rx_Chains.RxB.STW_PLL2.LockFlag_1 = PLL_GPIOs.RXB_PLLB.PIN;
	Rx_Chains.RxB.STW_PLL2.PFD = PFD_FREQ;
}

void change_Rx_Frequency(long long freq,uint8_t Chain)
{
	//struct Rx_PLLs *SelectedChain;
	
	if(Chain == 1)
	{
		Rx_Chains.RxA.FreqMHz = freq;
		EnableSPI_FOR(SPI_route.RXA);
	}
	if(Chain == 2)
	{
		Rx_Chains.RxB.FreqMHz = freq;
		EnableSPI_FOR(SPI_route.RXB);
	}
	
	Calculate_Frequency_Rx(Chain); //Setup frequency
	delay_ms(5);
	EnableSPI_FOR(SPI_route.MCU_ONLY);
	//write_calibration_points_to_eeprom(Chain);
	load_Rxcalibration(Chain);

	configurePortExpRx(); //**********Always execute after all voltage translators are disabled**********//
}

void Calculate_Frequency_Rx(uint8_t Chain)
{
	struct Rx_PLLs *Rx_Ch_;
	uint32_t ST0;//PLL2 freq - 2220MHz SB1/SB2/SB4
	
	if(Chain == 1 )
	{
		Rx_Ch_ = &Rx_Chains.RxA;
	}
	else if (Chain == 2)
	{
		Rx_Ch_ = &Rx_Chains.RxB;
	}
	else return;
	
	
	if(Rx_Ch_->FreqMHz < 60*SCALE_FACTOR)	return;
	else if (Rx_Ch_->FreqMHz < 1021*SCALE_FACTOR) // Sub-band 1
	{
		Rx_Ch_->Subband = 1;
		ST0 = 0x03F000CE; //2060MHz
	}
	else if (Rx_Ch_->FreqMHz < 2001*SCALE_FACTOR) // Sub-band 2
	{
		Rx_Ch_->Subband = 2;
		ST0 = 0x03F000DE; //2220MHz

	}
	else if (Rx_Ch_->FreqMHz <2301*SCALE_FACTOR) // Sub-band 3
	{
		Rx_Ch_->Subband = 3;
		ST0 = 0x03F000FC; //2520MHz

	}
	else if (Rx_Ch_->FreqMHz < 6661*SCALE_FACTOR) // Sub-band 4
	{
		Rx_Ch_->Subband = 4;
		ST0 = 0x03F000DE; //2220MHz
	}
	else return;

	Calculate_STW_Rx(Rx_Ch_);  //PLL1
	setup_STW_Rx(&Rx_Ch_->STW_PLL2, ST0, 0, 0, 0); //Pll2
}

void configurePortExpRx()
{
	static uint8_t GPA_C = 0b01010011; //Default value is for Main Input and Mixer disabled (GPA_C Controls input method and mixer for RXA) Tx 2350-2500 and 2550 Always high (bit 4 and 6)
	static uint8_t GPB_C = 0b01010011; //Default value is for Main Input and Mixer disabled	(GPB_C Controls input method and mixer for RXB) Tx 2350-2500 and 2550 Always high (bit 4 and 6)

	static uint8_t GPB_E = 0b00000011; //Default value is for sub band 2 (GPB_E Controls Subband filter for RXA (bits 2+3) and RXB (0+1)
	
	
	/*============RXA BELOW============*/
	if(Rx_Chains.RxA.Subband == 1 || Rx_Chains.RxA.Subband == 2) // If subband 1 or 2
	{
		GPB_E |= (1 << 3); //Enable LPF_HPF
		GPB_E &= ~(1 << 2); //Disable 2150_2450
		
		GPA_C &= ~(1 << 3); //Disable Mixer
	}
	else if (Rx_Chains.RxA.Subband == 3) // if subband 3
	{
		GPB_E &= ~(1 << 3); //Disable LPF_HPF
		GPB_E |= (1 << 2); //Enable 2150_2450
		
		GPA_C &= ~(1 << 3); //Disable Mixer
	}
	
	else if (Rx_Chains.RxA.Subband == 4) // if subband 4
	{
		GPB_E &= ~(1 << 3); //Disable LPF_HPF
		GPB_E &= ~(1 << 2); //Disable 2150_2450
		
		GPA_C |= (1 << 3); //Enable Mixer LPF_HPF
	}
	
	
	/*============RXB BELOW============*/
	if(Rx_Chains.RxB.Subband == 1 || Rx_Chains.RxB.Subband == 2)
	{
		GPB_E |= (1 << 0); //Enable LPF_HPF
		GPB_E &= ~(1 << 1); //Disable 2150_2450
		
		GPB_C &= ~(1 << 3); //Disable Mixer LPF_HPF
	}
	else if (Rx_Chains.RxB.Subband == 3)
	{
		GPB_E &= ~(1 << 0); //Disable LPF_HPF
		GPB_E |= (1 << 1); //Enable 2150_2450
		
		GPB_C &= ~(1 << 3); //Disable Mixer LPF_HPF
	}
	else if (Rx_Chains.RxB.Subband == 4)
	{
		GPB_E &= ~(1 << 0); //Disable LPF_HPF
		GPB_E &= ~(1 << 1); //Disable 2150_2450
		
		GPB_C |= (1 << 3); //Enable Mixer LPF_HPF
	}
	/*======================================================*/
	
	/* =======================U14 Port Expander C Inputs ============================== */
	/*========================RXA========================*/
	if (Rx_Chains.RxA.Input == 0) // 0 = Loopback,
	{
		GPA_C &= ~(1 << 0); //Clear  MAIN AUX LOOP SW 1 RXA
		GPA_C &= ~(1 << 1); //Clear  MAIN AUX LOOP SW 2 RXA
		GPA_C &= ~(1 << 2); //Clear  MAIN AUX LOOP SW 3 RXA
	}
	else if (Rx_Chains.RxA.Input == 1)//  1 = AUX,
	{
		GPA_C &= ~(1 << 0); //Clear  MAIN AUX LOOP SW 1 RXA
		GPA_C &= ~(1 << 1); //Clear  MAIN AUX LOOP SW 2 RXA
		GPA_C |= (1 << 2);  //Set  MAIN AUX LOOP SW 3 RXA
	}
	else if (Rx_Chains.RxA.Input == 2) // 2 = Main_Input
	{
		GPA_C |= (1 << 0);  //Set  MAIN AUX LOOP SW 1 RXA
		GPA_C |= (1 << 1);  //Set  MAIN AUX LOOP SW 2 RXA
		GPA_C &= ~(1 << 2); //Clear  MAIN AUX LOOP SW 3 RXA
	}
	/*========================RXB========================*/
	if (Rx_Chains.RxB.Input == 0) // 0 = Loopback, 1 = AUX, 2 = Main_Input
	{
		GPB_C &= ~(1 << 0); //Clear  MAIN AUX LOOP SW 1 RXA
		GPB_C &= ~(1 << 1); //Clear  MAIN AUX LOOP SW 2 RXA
		GPB_C &= ~(1 << 2); //Clear  MAIN AUX LOOP SW 3 RXA
	}
	else if (Rx_Chains.RxB.Input == 1)
	{
		GPB_C &= ~(1 << 0); //Clear  MAIN AUX LOOP SW 1 RXA
		GPB_C &= ~(1 << 1); //Clear  MAIN AUX LOOP SW 2 RXA
		GPB_C |= (1 << 2);  //Set  MAIN AUX LOOP SW 3 RXA
	}
	else if (Rx_Chains.RxB.Input == 2)
	{
		GPB_C |= (1 << 0);	//Set  MAIN AUX LOOP SW 1 RXA
		GPB_C |= (1 << 1);	//Set  MAIN AUX LOOP SW 2 RXA
		GPB_C &= ~(1 << 2); //Clear  MAIN AUX LOOP SW 3 RXA
	}

	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_C.CS_PORT, SPI_GPIOs.P_EXPANDER_C.CS_PIN,INIT_CMD,GPIOA_CMD,GPA_C); //U14 PortExp_C RXA/TXA GPA
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_C.CS_PORT, SPI_GPIOs.P_EXPANDER_C.CS_PIN,INIT_CMD,GPIOB_CMD,GPB_C); //U14 PortExp_C RXB/TXB GPB
	delay_us(1000);
	setupPortExpPorts(SPI_GPIOs.P_EXPANDER_E.CS_PORT, SPI_GPIOs.P_EXPANDER_E.CS_PIN,INIT_CMD,GPIOB_CMD,GPB_E); //U15 PortExp_E
}