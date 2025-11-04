/*
* STW81200.c
*
* Created: 31/03/2025 15:05:30
*  Author: constantinos.pavlide
*/
#include <STW81200.h>


void setup_STW(t_STW *activePLL,uint16_t ST0,uint32_t ST1,uint32_t ST2,uint8_t ST6)
{
	uint8_t lockFlagMask = (1 << (activePLL->LockFlag));  // Fix bit masking
	uint8_t attempts = 0;
	
	do	{
		SPI_send32(activePLL->PortCS,activePLL->CS ,0x03FFFFFFF); // PLL1 ST0 RESET
		attempts++;
		delay_ms(FLAG_RESPONSE_WAIT_ms);
		if (attempts >= 10) {
			UART_send_string("ERROR: PLL failed to reset Tx STW \r\n");
			return;
		}
	}while((*activePLL->PortFlag & lockFlagMask) != 0);
	
	delay_us(10000);
	attempts = 0;	
	
	uint32_t ST0_temp = 0x03F00102;
	uint32_t ST6_temp = 0x300004FA;
	//uint32_t ST6_temp = 0x300004FA
	uint32_t ST1_temp = 0x0C600000;
	uint32_t ST2_temp = 0x15000002;
	uint32_t ST3_temp = 0x1C000005;
	uint32_t ST4_temp = 0x20870303;
	uint32_t ST5_temp = 0x28000770;
	
	if(activePLL == (void*)&Rf_PLL.VHF_PLL3_STW_TxA || activePLL == (void*)&Rf_PLL.VHF_PLL3_STW_TxB) //PLL Master 3G constants
	{
		ST0_temp = (0x03F00000| (ST0 & 0x0000FFFF));
		ST1_temp = (0x0C000000 | (ST1 & 0x00FFFFFF));
		ST2_temp = (0x15000000 | (ST2 & 0x001FFFFF)); // Overwrite last 21 bits of ST2
		ST3_temp = 0x1C000004;
		ST6_temp = 0x340004FA;
		//ST6_temp = (0x300004FA & 0xFF9FFFFF) | ((ST6 << 22) & 0x007C0000);
	}
	else if(activePLL == (void*)&Tx_STW.STW_TxA || activePLL == (void*)&Tx_STW.STW_TxB)  //Log detector constants
	{
		ST0_temp = (0x03F00000| (ST0 & 0x0000FFFF));
		ST1_temp = (0x0C000000 | (ST1 & 0x00FFFFFF));
		ST3_temp = 0x1C000028;
		ST4_temp = 0x20870301;
		ST5_temp = 0x28001FE0;
	}
	
	do{
		SPI_send32(activePLL->PortCS,activePLL->CS,0x48000000); // PLL1 ST9
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,ST0_temp); // PLL1 ST0 // Overwrite last 16 bits of ST0
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x44000002); // PLL1 ST8
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x39800002); // PLL1 ST7
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,ST6_temp); // PLL1 ST6
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,ST5_temp); // PLL1 ST5
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,ST4_temp); // PLL1 ST4
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,ST3_temp); // PLL1 ST3
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,ST2_temp); // PLL1 ST2 // Overwrite last 24 bits of ST2
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,ST1_temp); // PLL1 ST1 // Overwrite last 24 bits of ST1
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,ST0_temp); // PLL1 ST0  // Overwrite last 16 bits of ST0
		delay_ms(FLAG_RESPONSE_WAIT_ms);
		attempts++;
		
		if (attempts >= 10) {
			UART_send_string("ERROR: PLL failed to lock (lock flag not set) Tx STW\r\n");
			return;
		}
				
	}while((*activePLL->PortFlag & lockFlagMask) == 0);

}

void setup_STW_Rx(struct Rx_status *activePLL,uint16_t ST0,uint32_t ST1,uint32_t ST2,uint8_t ST6)
{
	uint8_t lockFlagMask = (1 << (activePLL->LockFlag_1));  // Fix bit masking
	uint8_t attempts = 0;
	
	do	{
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,0x03FFFFFFF); // PLL1 ST0 RESET
		delay_ms(FLAG_RESPONSE_WAIT_ms);
		if (attempts >= 10) {
			UART_send_string("ERROR: PLL failed to reset Rx STW \r\n");
			return;
		}
	}while((*activePLL->PortFlag1 & lockFlagMask) != 0);
	
	//delay_ms(10);
	attempts = 0;		
	
	uint32_t ST0_temp = 0x03F00000 | ST0;
	uint32_t ST1_temp = 0x0C200000;
	uint32_t ST2_temp = 0x15000002;
	uint32_t ST3_temp = 0x1C000005;
	uint32_t ST5_temp = 0x280003F0;
	uint32_t ST6_temp = 0x300004FA;
	
	if (activePLL == &Rx_Chains.RxA.STW_PLL1 || activePLL == &Rx_Chains.RxB.STW_PLL1)
	{
		ST0_temp = (0x03F00000| (ST0 & 0x0000FFFF));
		ST1_temp = (0x0C000000 | (ST1 & 0x00FFFFFF));
		ST2_temp = (0x15000000 | (ST2 & 0x001FFFFF)); // Overwrite last 21 bits of ST2
		ST3_temp = 0x1C000004;
		ST5_temp = 0x280007F0;
		//ST6_temp = (0x300004FA & 0xFF9FFFFF) | ((ST6 << 22) & 0x007C0000);
		ST6_temp = 0x340004FA;
	}	
	
	do{
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,0x48000000); // PLL1 ST9
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,ST0_temp); // PLL1 ST0 // Overwrite last 16 bits of ST0
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,0x44000003); // PLL1 ST8
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,0x39800002); // PLL1 ST7
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,ST6_temp); // PLL1 ST6
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,ST5_temp); // PLL1 ST5
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,0x20070303); // PLL1 ST4
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,ST3_temp); // PLL1 ST3
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,ST2_temp); // PLL1 ST2 // Overwrite last 24 bits of ST2
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,ST1_temp); // PLL1 ST1 // Overwrite last 24 bits of ST1
		delay_us(PLL_DELAY_US);
		SPI_send32(activePLL->PortCS1,activePLL->CS_1 ,ST0_temp); // PLL1 ST0  // Overwrite last 16 bits of ST0
		delay_ms(FLAG_RESPONSE_WAIT_ms);
		
		attempts++;
		
		if (attempts >= 10) {
			UART_send_string("ERROR: PLL failed to lock (lock flag not set) Rx STW\r\n");
			return;
		}
		
	}while((*activePLL->PortFlag1 & lockFlagMask) == 0);
}

uint8_t setVCODivisionBits(uint16_t PLL_N)
{
	uint8_t bits = 0;  // Variable to hold the 3-bit value
	
	// Map the value of A to its corresponding 3-bit value
	switch(PLL_N)
	{
		case 1:
		bits = 0;  // 000
		break;
		case 2:
		bits = 1;  // 001
		break;
		case 4:
		bits = 2;  // 010
		break;
		case 8:
		bits = 3;  // 011
		break;
		case 16:
		bits = 4;  // 100
		break;
		case 32:
		bits = 5;  // 101
		break;
		case 64:
		bits = 6;  // 110
		break;
		//default:
		//return;
	}
	return bits;
}

void Calculate_STW(t_STW *activePLL) //This function is based on chapter 3.10 of the DDD_RX_VIBB
{
	uint16_t Clock_divider = 1;
	
	// Calculate PLL1 Frequency based on Subband, using integer math with SCALE_FACTOR for precision
	long long newPLL1Freq = activePLL->FreqMHz;
	//long long temp = (long long int)VCO_MIN * SCALE_FACTOR;
	// Calculate new VCO (scaled by SCALE_FACTOR)
	long long newVCO;
	do {
		newVCO = Clock_divider * newPLL1Freq; // Multiply in integer space
		Clock_divider *= 2;
	} while (newVCO < (long long int)VCO_MIN * SCALE_FACTOR || newVCO >  (long long int)VCO_MAX * SCALE_FACTOR); // Compare with scaled min/max VCO
	
	Clock_divider /= 2; // Divider number for ST1
	
	// Calculate DIV_N_PLL with fixed-point (scaled by SCALE_FACTOR)
	long long DIV_N_PLL = (long long)round(newVCO / activePLL->PFD);
	uint32_t newST0 = (uint32_t)(DIV_N_PLL/SCALE_FACTOR); // Integer part for ST0
	
	// Handle fractional part (scaled by SCALE_FACTOR)
	uint32_t int_FRAC_N_PLL1 = DIV_N_PLL - (newST0* SCALE_FACTOR);   // Fractional part as a long double
	
	// Prepare ST1/ST2 fractional
	uint32_t newST1 = 0;
	uint32_t newST2 = 2; // Minimum allowed in case there is no fractional
	uint32_t ST1_DIV_SEL = setVCODivisionBits(Clock_divider);
	
	// Handle fractional part logic
	if (int_FRAC_N_PLL1 != 0) {
		// Truncate fractional part to 3 decimals for ST1
		uint32_t den = SCALE_FACTOR;
		uint32_t multiplier = 1;
		
		// Find the largest multiplier within range
		while ((int_FRAC_N_PLL1 * (multiplier + 1) <= 2097151) && (den * (multiplier + 1) <= 2097151)) {
			multiplier++;
		}
		while ((int_FRAC_N_PLL1 * multiplier > 2097151) || (den * multiplier > 2097151)) {
			multiplier--;
		}
		int_FRAC_N_PLL1 = int_FRAC_N_PLL1 * multiplier; // Fractional number for ST1
		int_FRAC_N_PLL1 = int_FRAC_N_PLL1 & 0x1FFFFF;  // Mask to 21 bits
		
		den = den * multiplier; // Update denominator for ST2
		newST2 = den & 0x1FFFFF;  // Mask to 21 bits
	}
	
	// Finalize ST1 calculation
	newST1 = (int_FRAC_N_PLL1 & 0x1FFFFF) | (ST1_DIV_SEL << 21); // Prepare ST1
	
	uint8_t newSt6 = (int_FRAC_N_PLL1 != 0) ? (1 << 0 | 1 << 1 | 1 << 4) : 0; // Enable fractional mode bits in ST6
	
	// Call setupPLL1 function (commented out in the original)
	setup_STW(activePLL, newST0, newST1, newST2, newSt6);
}

void Calculate_STW_Rx(struct Rx_PLLs *activeChain) //This function is based on chapter 3.10 of the DDD_RX_VIBB
{
	struct Rx_status *activePll = &activeChain->STW_PLL1;
	
	uint32_t IntFreq = 2140 * SCALE_FACTOR; // Intermediate frequency for SB 1,2,4
	uint16_t Clock_divider = 1;
	
	// Calculate PLL1 Frequency based on Subband, using integer math with SCALE_FACTOR for precision
	long long newPLL1Freq;
	if (activeChain->Subband == 1) {
		newPLL1Freq = (IntFreq - activeChain->FreqMHz) ; // Multiply by SCALE_FACTOR
		} else if (activeChain->Subband == 4) {
		newPLL1Freq = ((IntFreq + activeChain->FreqMHz)) / 2; // Divide by 2 with scaling
		} else {
		if (activeChain->Subband == 3) { // Subband 3 has a different IF
			IntFreq = 2440* SCALE_FACTOR;
		}
		newPLL1Freq = (IntFreq + activeChain->FreqMHz);
	}
	
	// Calculate new VCO (scaled by SCALE_FACTOR)
	long long newVCO;
	do {
		newVCO = Clock_divider * newPLL1Freq; // Multiply in integer space
		Clock_divider *= 2;
	} while (newVCO < (long long int)VCO_MIN * SCALE_FACTOR || newVCO >  (long long int)VCO_MAX * SCALE_FACTOR); // Compare with scaled min/max VCO
	
	Clock_divider /= 2; // Divider number for ST1
	
	// Calculate DIV_N_PLL with fixed-point (scaled by SCALE_FACTOR)
	//long long DIV_N_PLL = (long long)round(newVCO / activePll->PFD);
	long long DIV_N_PLL = (newVCO + (long long)(activePll->PFD / 2)) / (long long)activePll->PFD;
	uint32_t newST0 = (uint32_t)(DIV_N_PLL/SCALE_FACTOR); // Integer part for ST0
	
	// Handle fractional part (scaled by SCALE_FACTOR)
	uint32_t int_FRAC_N_PLL1 = DIV_N_PLL - (newST0* SCALE_FACTOR);   // Fractional part as a long double
	
	// Prepare ST1/ST2 fractional
	uint32_t newST1 = 0;
	uint32_t newST2 = 2; // Minimum allowed in case there is no fractional
	uint32_t ST1_DIV_SEL = setVCODivisionBits(Clock_divider);
	
	// Handle fractional part logic
	if (int_FRAC_N_PLL1 != 0) {
		// Truncate fractional part to 3 decimals for ST1
		uint32_t den = SCALE_FACTOR;
		uint32_t multiplier = 1;
		
		// Find the largest multiplier within range
		while ((int_FRAC_N_PLL1 * (multiplier + 1) <= 2097151) && (den * (multiplier + 1) <= 2097151)) {
			multiplier++;
		}
		while ((int_FRAC_N_PLL1 * multiplier > 2097151) || (den * multiplier > 2097151)) {
			multiplier--;
		}
		int_FRAC_N_PLL1 = int_FRAC_N_PLL1 * multiplier; // Fractional number for ST1
		int_FRAC_N_PLL1 = int_FRAC_N_PLL1 & 0x1FFFFF;  // Mask to 21 bits
		
		den = den * multiplier; // Update denominator for ST2
		newST2 = den & 0x1FFFFF;  // Mask to 21 bits
	}
	
	// Finalize ST1 calculation
	newST1 = (int_FRAC_N_PLL1 & 0x1FFFFF) | (ST1_DIV_SEL << 21); // Prepare ST1
	
	uint8_t newSt6 = (int_FRAC_N_PLL1 != 0) ? (1 << 0 | 1 << 1 | 1 << 4) : 0; // Enable fractional mode bits in ST6
	
	//Call setupPLL1 function (commented out in the original)
	setup_STW_Rx(activePll, newST0, newST1, newST2, newSt6);
}

