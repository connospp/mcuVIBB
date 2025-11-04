/*
 * ADF5356.c
 *
 * Created: 31/03/2025 15:29:42
 *  Author: constantinos.pavlide
 */ 
#include <ADF5356.h>

uint16_t setup_ADF(t_ADF5352 *activePLL)
{
	uint8_t lockFlagMask = (1 << (activePLL->LockFlag));  // Fix bit masking	
	uint8_t attempts = 0;
	
	do	{
		SPI_send32(activePLL->PortCS,activePLL->CS,0x20300000 ); // R0 RESET
		delay_ms(FLAG_RESPONSE_WAIT_ms);
		
		attempts++;
		if (attempts >= 10) {
			UART_send_string("ERROR: PLL failed to lock (lock flag not set) ADF \r\n");
			return 0;
		}
		
	} while((*activePLL->PortFlag & lockFlagMask) != 0);
	
	// delay_ms(10);
	attempts = 0;

	
	uint16_t Ncounter = (activePLL->FreqMHz < 310) ? 71 : 73 + 2 * ((int)(activePLL->FreqMHz - 310) / 250);
	//uint16_t Ncounter = 73 + 2 * ((int)(activePLL->FreqMHz - 310) / 250);
	uint32_t R0_temp = 0x20300000 | (Ncounter << 4);
	
	do{
		SPI_send32(activePLL->PortCS,activePLL->CS,0x0000002F);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0xFFFFF5FC);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x0161200B);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x000026BA);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x27801449);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x15596568);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x02000017);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0xC1104026);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x00800025);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x30027F84);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x40000003);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x00000022);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,0x00000001);
		delay_us(ADF_DELAY_US);
		SPI_send32(activePLL->PortCS,activePLL->CS,R0_temp);
		delay_ms(FLAG_RESPONSE_WAIT_ms);
		
		attempts++;
		if (attempts >= 10) {
			UART_send_string("ERROR: PLL failed to lock (lock flag not set) ADF \r\n");
			return 0;
		}
		
	}while((*activePLL->PortFlag & lockFlagMask) == 0);	
	
	
	//char buffer[10];         // Buffer to hold the string representation
	//sprintf(buffer, "%u", attempts);
	//UART_send_string(buffer); //Output channel one
	//send_uart('\n');
	//
	//sprintf(buffer, "%u",Ncounter);
	//UART_send_string(buffer); //Output channel one
	//send_uart('\n');
	//
	//float freqOut = 2*Ncounter*62.5;
	//
	//sprintf(buffer, "%.2f",freqOut);
	//UART_send_string(buffer); //Output channel one
	//send_uart('\n');
	return Ncounter;
}
