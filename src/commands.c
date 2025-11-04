/*
* @brief All data coming in over one of the USARTs is handled with functions
* underneath. The commands use functions in other files (led0 in led.c, ...)
*
* Commands USART0 are all ASCII based:
*   led0, led1, led2, leds: modify the LEDs: 1 = on; 0 = off; t = toggle.
*   reset: initialize the USARTs again + flush hanging data
*
*
* Company: Celestia Antwerp B.V.
* Project: IBBE
* Created: 2020-12-15
* Author : pg
* Copyright (c) Celestia Antwerp B.V.
*/

#include "commands.h"

#define BIT(n) (1UL << (n))
// Helper macro to safely read pin state
#define READ_PIN(GPIO) ((*((volatile uint8_t *)(GPIO.PORT)) & (1U << (GPIO.PIN))) ? 1 : 0)

const char* REBOOT_CMD = "REBOOT";

/*
* Build in pow function is not correct
* Only need powers of 16 for now
*/
uint32_t pow16 (uint8_t x) {
	uint32_t p = 1;

	for (uint8_t i=0;i<x;i++) p *= 16;

	return p;
}

/*
* convert hex value to 32bit unsigned int
*/
uint32_t convert_hex_cmd(void) {
	uint32_t x = 0;
	uint8_t i = cmd_i-1;
	uint8_t j = i-8;
	while ( i > j ) {
		if (cmd[i] == '\r' || cmd[i] == '\n') {
			i--;
			j = i-8;
			} else {
			/* do the conversion of hex 32 bit word */
			if (cmd[i] >= '0' && cmd[i] <= '9') {
				x += (cmd[i] - '0') * pow16((8 - (i - j)));
				} else if (cmd[i] >= 'a' && cmd[i] <= 'f') {
				x += (cmd[i] - 'a' + 10) * pow16((8 - (i - j)));
				} else if (cmd[i] >= 'A' && cmd[i] <= 'F') {
				x += (cmd[i] - 'A' + 10) * pow16 ((8 - (i - j)));
				} else {
				return x;
			}
			i--;
		}
	}
	return x;
}

/*
* Send the value of the command over the SPI interface
* The value has to be a 32 bit integer.
*/
uint32_t send_spi_cmd (uint32_t tranceive) {
	/* get unsigned 32 bit int value provided by the user */
	long_msg_t rx_flash_msg;
	long_msg_t tx_flash_msg;

	/* convert hex value to 32bit unsigned int */
	tx_flash_msg.msg = tranceive;

	/* transmit to flash */
	rx_flash_msg = long_wr_spi(&tx_flash_msg);
	return rx_flash_msg.msg;
}


/*
* LEDS 0 to 7 can be switched red(1), green(2) or off(0), or can be toggled red/off)
*/
void handle_uart_led_cmd (void)
{
	//if (cmd[5] == 't' || cmd[5] == 'T') toggle_led(cmd[3]-'0');
	//else led(cmd[3]-'0', cmd[5]-'0');
}


//////////////////////////////////////////////////////////////////////////
/*                               UART Commands                          */
//////////////////////////////////////////////////////////////////////////

/*
* Execute a UART command
*/
void UART_execute_cmd(void) {
	char buffer[64];

	for (uint8_t i = 0; uart_buffer[i] != '\0'; i++) {
		if (uart_buffer[i] >= 'a' && uart_buffer[i] <= 'z') {
			uart_buffer[i] -= 32;
		}
	}
	
	if (strcmp((const char *)uart_buffer, "LED") == 0)
	{
		UART_send_string("Command not in use");
	}
	else if (strncmp((const char *)uart_buffer, "R_SN", 5) == 0) { // match full "W_SNA"
		uint8_t ep_SN[8];
		char buffer[32];
		
		eeprom_read_block((void*)&ep_SN, (const void*)sn1_p, 8);
		snprintf(buffer, sizeof(buffer), "SN1: %u%u%u%u/%u%u%u%u\r\n",
		ep_SN[0], ep_SN[1], ep_SN[2], ep_SN[3], ep_SN[4], ep_SN[5], ep_SN[6], ep_SN[7]);
		UART_send_string(buffer);

		eeprom_read_block((void*)&ep_SN, (const void*)sn2_p, 8);
		snprintf(buffer, sizeof(buffer), "SN2: %u%u%u%u/%u%u%u%u\r\n",
		ep_SN[0], ep_SN[1], ep_SN[2], ep_SN[3], ep_SN[4], ep_SN[5], ep_SN[6], ep_SN[7]);
		UART_send_string(buffer);
	}
	else if (strncmp((const char *)uart_buffer, "W_SN1", 5) == 0) { // match full "W_SNA"
		volatile char* arg = uart_buffer + 5;  // move pointer past "W_SN1"
		uint8_t serialN[8];           // 8 bytes
		while (*arg == ' ') arg++; // skip spaces

		for (int i = 0; i < 8; i++) {
			serialN[i] = arg[i] - '0'; // convert each character to integer
		}

		eeprom_update_block((const void*)serialN, sn1_p, 8);  // save only 8 bytes
		UART_send_string("SN1 written to EEPROM\r\n");
	}
	else if (strncmp((const char *)uart_buffer, "W_SN2",5) == 0)
	{
		volatile char* arg = uart_buffer + 5;  // move pointer past "W_SN2"
		uint8_t serialN[8];		   // 8 bytes
		while (*arg == ' ') arg++; // skip spaces

		for (int i = 0; i < 8; i++) {
			serialN[i] = arg[i] - '0'; // convert each character to integer
		}

		eeprom_update_block((const void*)serialN, sn2_p, 8);  // save only 8 bytes
		UART_send_string("SN2 written to EEPROM\r\n");
	}
	else if (strncmp((const char *)uart_buffer, "W_MAC", 5) == 0) {
		write_mac_address((const char *)uart_buffer + 5);
	}
	else if (strncmp((const char *)uart_buffer, "R_MAC", 5) == 0) {
		uint8_t macAddress[6];
		char macString[18];

		eeprom_read_block(macAddress, (const void*)mac_p, 6);

		// Format as colon-separated hex string
		snprintf(macString, sizeof(macString), "%02X:%02X:%02X:%02X:%02X:%02X",
		macAddress[0], macAddress[1], macAddress[2],
		macAddress[3], macAddress[4], macAddress[5]);

		UART_send_string("MAC: ");
		UART_send_string(macString);
		UART_send_string("\r\n");
	}
	else if (strcmp((const char *)uart_buffer, "RESET") == 0)
	{
		CID_eth_factrst();
	}
	else if (strncmp((const char *)uart_buffer, "DTXA",4) == 0)
	{
		uint16_t number_buffer = extractFloat(4,uart_buffer);
		tx.TxA.currentDACValue[0] = tx.TxA.currentDACValue[1] = tx.TxA.currentDACValue[2] = number_buffer;
		setupDACTxA();
	}
	else if (strncmp((const char *)uart_buffer, "DTXB",4) == 0)
	{
		uint16_t number_buffer = extractFloat(4,uart_buffer);
		tx.TxB.currentDACValue[0] = tx.TxB.currentDACValue[1] = tx.TxB.currentDACValue[2] = number_buffer;
		setupDACTxB();
	}
	else if (strncmp((const char *)uart_buffer, "DRXA",4) == 0)
	{
		uint16_t number_buffer = extractFloat(4,uart_buffer);
		Rx_Chains.RxA.currentDACValue[0] = Rx_Chains.RxA.currentDACValue[1] = Rx_Chains.RxA.currentDACValue[2] = number_buffer;
		setupDACRxA();
	}
	else if (strncmp((const char *)uart_buffer, "DRXB",4) == 0)
	{
		uint16_t number_buffer = extractFloat(4,uart_buffer);
		Rx_Chains.RxB.currentDACValue[0] = Rx_Chains.RxB.currentDACValue[1] = Rx_Chains.RxB.currentDACValue[2] = number_buffer;
		setupDACRxB();
	}
	
	else if (strncmp((const char *)uart_buffer, "PTXA", 4) == 0)
	{
		float value = extractFloat(4, uart_buffer);    // extract float from UART buffer
		value *= 10.0f;                                // multiply by 10
		
		// Clamp to desired range
		if (value > 10.0f)      value = 10.0f;
		if (value < -70.0f)     value = -70.0f;

		// remove any decimal part
		tx.TxA.carrierPower = (int16_t)value;          // implicit truncation (no rounding)

		set_tx_out_power(1);

		sprintf(buffer, "Power TXA: %d cbm\r\n ", tx.TxA.carrierPower);
		UART_send_string(buffer);
	}
	
	else if (strncmp((const char *)uart_buffer, "PTXB", 4) == 0)
	{
		float value = extractFloat(4, uart_buffer);    // extract float from UART buffer
		value *= 10.0f;                                // multiply by 10

		// Clamp to desired range
		if (value > 10.0f)      value = 10.0f;
		if (value < -70.0f)     value = -70.0f;

		// remove any decimal part
		tx.TxB.carrierPower = (int16_t)value;          // implicit truncation (no rounding)

		set_tx_out_power(2);

		sprintf(buffer, "Power TXB: %d cbm \r\n ", tx.TxB.carrierPower);
		UART_send_string(buffer);
	}
	
	else if (strcmp((const char *)uart_buffer, "ADC") == 0)
	{
		sprintf(buffer,"TxA Freq: %lu MHz\r\n", (uint32_t)(tx.TxA.FreqMHz / SCALE_FACTOR));
		UART_send_string(buffer);
		
		sprintf(buffer, "TxA ADC = %u | Target = %u\r\n", tx.TxA.currentADC,tx.TxA.targetADC);  // %u for unsigned int
		UART_send_string(buffer);
		
		sprintf(buffer, "TxA Power = %d\r\n", tx.TxA.carrierPower);  // %d for signed int
		UART_send_string(buffer);
		
		sprintf(buffer, "TxA TimeConst = %u\r\n", tx.TxA.timeConst);  // %u for unsigned int
		UART_send_string(buffer);
		
		UART_send_string("======================================\r\n");
		
		sprintf(buffer,"TxB Freq: %lu MHz\r\n", (uint32_t)(tx.TxB.FreqMHz / SCALE_FACTOR));
		UART_send_string(buffer);
		
		sprintf(buffer, "TxB ADC = %u | Target = %u\r\n", tx.TxB.currentADC,tx.TxB.targetADC);  // %u for unsigned int
		UART_send_string(buffer);
		
		sprintf(buffer, "TxB Power = %d\r\n", tx.TxB.carrierPower);  // %d for signed int
		UART_send_string(buffer);
		
		sprintf(buffer, "TxB TimeConst = %u\r\n", tx.TxB.timeConst);  // %u for unsigned int
		UART_send_string(buffer);
		
		UART_send_string("======================================\r\n");
		sprintf(buffer,"RxA Freq: %lu MHz\r\n", (uint32_t)(Rx_Chains.RxA.FreqMHz / SCALE_FACTOR));
		UART_send_string(buffer);
		
		sprintf(buffer, "RxA ADC = %u | Target = %u\r\n", Rx_Chains.RxA.currentADC,Rx_Chains.RxA.targetADC);  // %u for unsigned int
		UART_send_string(buffer);
		
		sprintf(buffer, "RxA TimeConst = %u\r\n", Rx_Chains.RxA.timeConst);  // %u for unsigned int
		UART_send_string(buffer);
		
		sprintf(buffer, "RxA AUX offset = %u\r\n", rxa_calibration.aux.offset);  // %u for unsigned int
		UART_send_string(buffer);
		
		sprintf(buffer, "RxA Main offset = %u\r\n", rxa_calibration.main.offset);  // %u for unsigned int
		UART_send_string(buffer);
		
		UART_send_string("======================================\r\n");

		sprintf(buffer,"RxB Freq: %lu MHz\r\n", (uint32_t)(Rx_Chains.RxB.FreqMHz / SCALE_FACTOR));
		UART_send_string(buffer);
		
		sprintf(buffer, "RxB ADC = %u | Target = %u\r\n", Rx_Chains.RxB.currentADC,Rx_Chains.RxB.targetADC);  // %u for unsigned int
		UART_send_string(buffer);
		
		sprintf(buffer, "RxB TimeConst = %u\r\n", Rx_Chains.RxB.timeConst);  // %u for unsigned int
		UART_send_string(buffer);
		
		sprintf(buffer, "RxB AUX offset = %u\r\n", rxb_calibration.aux.offset);  // %u for unsigned int
		UART_send_string(buffer);
		
		sprintf(buffer, "RxB Main offset = %u\r\n", rxb_calibration.main.offset);  // %u for unsigned int
		UART_send_string(buffer);
		
		UART_send_string("======================================\r\n");

		
	}
	else if (strcmp((const char *)uart_buffer, "TEMP") == 0)
	{
		float temp = TC72_read_float();
		sprintf(buffer, "MCU = %.2f�C\r\n", temp);
		UART_send_string(buffer);
	}
	else if (strncmp((const char *)uart_buffer, "SP",3) == 0)
	{
		UART_send_string("Command not in use");
	}
	else if (strcmp((const char *)uart_buffer, "TXA_LOOP") == 0)
	{
		tx.TxA.Output ^= 1; // flip between 1/2

		if ((*REST.TXA_LOOP_SW.PORT & (1 << REST.TXA_LOOP_SW.PIN)) == 0) {
			// Pin is LOW -> set HIGH and apply agcDisable + DAC = 1800
			//tx.TxA.agcEnable = 0;
			//tx.TxA.currentDACValue[0] = tx.TxA.currentDACValue[1] = tx.TxA.currentDACValue[2] = 1800;
			//setupDACTxA();
			
			*REST.TXA_LOOP_SW.PORT |= (1 << REST.TXA_LOOP_SW.PIN);
		}
		else {
			// Pin is HIGH -> set LOW (no extra actions here)
			*REST.TXA_LOOP_SW.PORT &= ~(1 << REST.TXA_LOOP_SW.PIN);
		}
	}

	else if (strcmp((const char *)uart_buffer, "TXB_LOOP") == 0)
	{
		tx.TxB.Output ^= 1; // flip between 1/2

		if ((*REST.TXB_LOOP_SW.PORT & (1 << REST.TXB_LOOP_SW.PIN)) == 0) {
			// Pin is LOW -> set HIGH and apply agcDisable + DAC = 1800
			//tx.TxB.agcEnable = 0;
			//tx.TxB.currentDACValue[0] = tx.TxB.currentDACValue[1] = tx.TxB.currentDACValue[2] = 1800;
			//setupDACTxB();
			
			*REST.TXB_LOOP_SW.PORT |= (1 << REST.TXB_LOOP_SW.PIN);
		}
		else {
			// Pin is HIGH -> set LOW (no extra actions here)
			*REST.TXA_LOOP_SW.PORT &= ~(1 << REST.TXA_LOOP_SW.PIN);
		}
	}
	else if (strcmp((const char *)uart_buffer, "RXA_LOOP") == 0)
	{
		const char* input_names[] = {
			"Loopback",         // 0
			"Auxiliary Output", // 1
			"Main Output"       // 2
			//"OFF" which is index 3 is not saved.
		};
		
		if (Rx_Chains.RxA.Input < 2)
		{
			Rx_Chains.RxA.Input++;
		}
		else
		{
			Rx_Chains.RxA.Input = 0;
		}

		configurePortExpRx(); // RX PortExpander configuration must be done after Voltage translator is disabled
		
		sprintf(buffer, "RXA = %s\r\n", input_names[Rx_Chains.RxA.Input]);
		UART_send_string(buffer);
	}
	else if (strcmp((const char *)uart_buffer, "RXB_LOOP") == 0)
	{
		const char* input_names[] = {
			"Loopback",         // 0
			"Auxiliary Output", // 1
			"Main Output"       // 2
			//"OFF" which is index 3 is not saved.
		};
		
		if (Rx_Chains.RxB.Input < 2)
		{
			Rx_Chains.RxB.Input++;
		}
		else
		{
			Rx_Chains.RxB.Input = 0;
		}

		configurePortExpRx(); // RX PortExpander configuration must be done after Voltage translator is disabled

		sprintf(buffer, "RXB = %s\r\n", input_names[Rx_Chains.RxB.Input]);
		UART_send_string(buffer);
		
	}
	else if (strcmp((const char *)uart_buffer, "ETH") == 0)
	{
		read_eth();
	}
	else if (strncmp((const char *)uart_buffer, "IP",2) == 0)
	{
		volatile char* input = uart_buffer + 2;  // skip "IP"
		uint8_t def_ip_param[12] = {0}; // default IP, subnet mask and gateway
		
		for (size_t i = 0; i < 12; i++) {
			char segment[4] = {0}; // 3 digits + null terminator
			strncpy(segment,(const char *) input + i*3, 3);
			def_ip_param[i] = (uint8_t)atoi(segment);
		}
		
		eeprom_update_block((const void*)def_ip_param, (void*)ip_p, 12);
		eth_status.F_fin = 1; // re-initialize IP/TCP protocol w5500
	}
	else if (strncmp((const char *)uart_buffer, "PORT",4) == 0)
	{
		uint8_t port_bytes[2];
		uint16_t number_buffer = (uint16_t)extractFloat(4,uart_buffer);
		port_bytes[1] = number_buffer & 0xFF;       // LSB
		port_bytes[0] = (number_buffer >> 8) & 0xFF; // MSB
		
		eeprom_update_block((const void*)port_bytes, (void*)tcp_p, 2);
		eth_status.F_fin = 1;
	}
	else if (strcmp((const char *)uart_buffer, "HEALTH") == 0)
	{
		const char* msgs[] = {
			"ADC Tx", "ADC Rx", "Ethernet", NULL, "Temp",
			"RXA PLLA", "RXA PLLB", "RXB PLLA", "RXB PLLB",
			"TXA LOG DET PLL", "TXB LOG DET PLL", "PLL MASTER 645",
			"PLL Master 100M (10M)", "PLL Master 3.5G OK", "PLL Master VHF",
			"PLL Master 8-12G", "PLL Slave VHF", "PLL Slave 3.5G", "PLL Slave 8-12G"
		};

		perif_health = CID_health_check();
		char buffer[64];
		for (uint8_t bit = 0; bit < sizeof(msgs)/sizeof(msgs[0]); bit++) {
			if (msgs[bit] == NULL) continue; // skip unused bit 3
			snprintf(buffer, sizeof(buffer), "%s = %lu\r\n", msgs[bit], (perif_health >> bit) & 1);
			UART_send_string(buffer);
		}
		
		// Handle bits 19�21 for PSU status
		uint8_t bit19 = (perif_health >> 19) & 1;
		uint8_t bit20 = (perif_health >> 20) & 1;
		uint8_t bit21 = (perif_health >> 21) & 1;

		if (!bit19 && bit20 && bit21) {
			UART_send_string("PSU OK\r\n");
		}
		else if (bit21 && bit19 && !bit20) {
			UART_send_string("One of two AC cables not connected ");
			UART_send_string("or One of two PSUs has fatal error\r\n");
		}
		else if (!bit19 && !bit20 && bit21) {
			UART_send_string("One of the two PSU modules not properly inserted\r\n");
		}
		else if (bit19 && !bit20 && !bit21) {
			UART_send_string("Fatal error\r\n");
			} else {
			UART_send_string("PSU status unknown\r\n");
		}
	}
	else if (strcmp((const char *)uart_buffer, "AGC") == 0)
	{
		Rx_Chains.RxA.agcEnable ^= 1;
		Rx_Chains.RxB.agcEnable ^= 1;
		tx.TxA.agcEnable        ^= 1;
		tx.TxB.agcEnable        ^= 1;
		
		sprintf(buffer, "AGC - RxA:%s RxB:%s TxA:%s TxB:%s\r\n",
		Rx_Chains.RxA.agcEnable ? "ON" : "OFF",
		Rx_Chains.RxB.agcEnable ? "ON" : "OFF",
		tx.TxA.agcEnable ? "ON" : "OFF",
		tx.TxB.agcEnable ? "ON" : "OFF");
		
		UART_send_string(buffer);
	}
	else if (strcmp((const char *)uart_buffer, REBOOT_CMD) == 0)
	{
		w5500_disconnect_then_abort(SOCKET_0,500); //Wait for 500ms for mercifull disconection else close TCP connection bruttaly
		wdt_enable(WDTO_15MS); // Set watchdog to timeout in 15ms
		while (1);           // Wait for watchdog to reset the MCU
	}
	else if (strcmp((const char *)uart_buffer, "OFF") == 0)
	{
		powerHandling();
	}
	else if (strncmp((const char *)uart_buffer, "TEST_MODE",3) == 0)
	{
		UART_send_string("Command not in use");
	}
	else if (strncmp((const char *)uart_buffer, "QPC", 3) == 0 || strncmp((const char *)uart_buffer, "ATT", 3) == 0)
	{
		float number_buffer = extractFloat(3, uart_buffer);
		float TxAFreq = Rf_PLL._8_12Ghz_PLL2_ADF_TxA.FreqMHz;
		uint16_t Ncounter = (TxAFreq < 310) ? 71 : 73 + 2 * ((int)(TxAFreq - 310) / 250);
		
		snprintf(buffer, sizeof(buffer), "TxA Freq: %.3f MHz | N: %u\r\n", TxAFreq, Ncounter);
		UART_send_string(buffer);

		EnableSPI_FOR(SPI_route.PLL_M);

		if (number_buffer < 0.0f) number_buffer = 0.0f;
		if (number_buffer > 31.75f) number_buffer = 31.75f;

		uint8_t atten_value = (uint8_t)(number_buffer * 4) & 0x7F;
		uint16_t reg_value = (uint16_t)atten_value;

		SPI_send16_LSB_First(Rf_PLL.Master1.Port_CS_ATT_QPC, Rf_PLL.Master1.CS_ATT_QPC, reg_value);

		EnableSPI_FOR(SPI_route.MCU_ONLY);

		// Update ATT value in the calibration table
		for (int i = 0; i < CAL_TABLE_SIZE; i++) {
			if (freqTable.N[i] == Ncounter) {
				freqTable.ATT[i] = number_buffer;
				break;
			}
		}
	}
	else if (strncmp((const char *)uart_buffer, "RHE", 3) == 0)
	{
		EnableSPI_FOR(SPI_route.PLL_M);

		uint16_t number_buffer = extractFloat(3, uart_buffer);
		
		float TxAFreq = Rf_PLL._8_12Ghz_PLL2_ADF_TxA.FreqMHz;
		uint16_t Ncounter = (TxAFreq < 310) ? 71 : 73 + 2 * ((int)(TxAFreq - 310) / 250);

		snprintf(buffer, sizeof(buffer), "TxA Freq: %.3f MHz | N: %u\r\n", TxAFreq, Ncounter);
		UART_send_string(buffer);
		
		if (number_buffer <= 0) number_buffer = 0;
		if (number_buffer > 100) number_buffer = 100;

		// Scale percentage to 10-bit value
		uint16_t percent_value = (((uint32_t)number_buffer * 1023) / 100) & 0x03FF;

		// Merge with fixed 6 MSBs (000001)
		uint16_t reg_value = (0x01 << 10) | percent_value;

		SPCR |= (1 << CPHA);  // Change SPI mode
		delay_us(10);
		SPI_send16(Rf_PLL.Master1.Port_CS_REO_AD5270, Rf_PLL.Master1.CS_REO_AD5270, reg_value);
		delay_us(10);
		SPCR &= ~(1 << CPHA); // Reset SPI mode

		EnableSPI_FOR(SPI_route.MCU_ONLY);

		// Update RHE value in the calibration table
		for (int i = 0; i < CAL_TABLE_SIZE; i++) {
			if (freqTable.N[i] == Ncounter) {
				freqTable.RHE[i] = number_buffer;
				break;
			}
		}
	}
	else if (strncmp((const char *)uart_buffer, "TXA",3) == 0)
	{
		long long number_buffer = extractFloatToLong(3,uart_buffer);
		change_Tx_Frequency(number_buffer,1);
	}
	else if (strncmp((const char *)uart_buffer, "TXB",3) == 0)
	{
		long long number_buffer = extractFloatToLong(3,uart_buffer);
		change_Tx_Frequency(number_buffer,2);
	}
	else if (strncmp((const char *)uart_buffer, "RXA",3) == 0)
	{
		long long number_buffer = extractFloatToLong(3,uart_buffer);
		change_Rx_Frequency(number_buffer,1);
	}
	else if (strncmp((const char *)uart_buffer, "RXB",3) == 0)
	{
		long long number_buffer = extractFloatToLong(3,uart_buffer);
		change_Rx_Frequency(number_buffer,2);
	}
	else if (strncmp((const char *)uart_buffer, "SAVE_N",3) == 0)
	{
		write_freqTable_to_eeprom();
	}
	else if (strcmp((const char *)uart_buffer, "PLL645") == 0)
	{
		Prepare645M();
	}
	else
	{
		UART_send_string("Invalid command");
	}
	UART_send_string("Done\n\r");

}


//////////////////////////////////////////////////////////////////////////
/*                               CID Ethernet                           */
//////////////////////////////////////////////////////////////////////////


/*
* @brief Command ID LED: control the LED(s)
*
* @param lednumber * selects one of the 8 LEDs (0x00-0x07)
* @param state     * OFF - RED - GREEN - ORANGE (0x00-0x03)
*/
void CID_led(uint8_t lednumber, uint8_t state) {
	if(lednumber >= 0 && lednumber < 0x08) {
		led(lednumber, state);
		send_ack(0x00, ACK);
		} else {
		send_ack(0x00, WRONG_PARAM);
	}
}

/*
* @brief Command ID getStatus: trigger a status message
*
* @param chain * get status of chain A (0x00) or chain B (0x01)
*/
void CID_getStatus(uint8_t chain) {
	chain++;
	if (chain == 1) {
		
		/* transmit status of chain A */
		send_status(chain);
	}
	else if (chain == 2) {
		//send_ack(0x02, ACK);
		/* transmit status of chain B */
		send_status(chain);
	}
	else {
		send_ack(0x02, WRONG_PARAM);
	}
}


/*
* @brief Command ID AGCset: set the setpoint + time constant of a particular RF chain
*
* @param chain: set AGC of chain A (0x00) or chain B (0x01)
* @param TxRx: select TX (0x00) or Rx (0x01) of chain
* @param in_lvl: measured input level offset in cB
* @param setpoint: expected output power in ADC value
* @param t_constant: time constant to get 63.2% of the expected output power (in ms)
*/
void CID_AGCset(uint8_t chain, uint8_t TxRx, int16_t inpt_lvl, uint16_t setpoint, uint16_t t_constant) {
	if (t_constant < 10 || t_constant > 10000 || chain >1 || TxRx > 1) {
		send_ack(0x03, WRONG_PARAM);
		return;
	}
	chain++;
	
	struct Rx_PLLs *SelectedChain;
	struct Tx_status *SelChainTx;
	rxPoints *calPoints;
	
	if (TxRx == 0) {
		if(chain == 1) {
			SelChainTx = &tx.TxA;
			} else {
			SelChainTx = &tx.TxB;
		}
		SelChainTx->timeConst = t_constant;
		send_ack(0x03, ACK);
		return;
	}

	if(chain == 1) {
		SelectedChain = &Rx_Chains.RxA;
		calPoints = &rxa_calibration;
		} else{
		SelectedChain = &Rx_Chains.RxB;
		calPoints = &rxb_calibration;
	}

	SelectedChain->timeConst = t_constant;
	
	//1 = AUX, 2 = Main_Input
	if(SelectedChain->Input == 1)
	{
		calPoints->aux.dac = setpoint;
		calPoints->aux.offset = inpt_lvl;
		//calPoints->aux.t_constant = t_constant;
	}
	else if(SelectedChain->Input == 2)
	{
		calPoints->main.dac = setpoint;
		calPoints->main.offset = inpt_lvl;
		//calPoints->main.t_constant = t_constant;
	}
	
	write_calibration_points_to_eeprom(chain); //Save new changes to EEPROM
	delay_ms(2);
	load_Rxcalibration(chain); //Activate changes
	send_ack(0x03, ACK);
}


/*
* @brief Command ID AGCconf: configure the calibration points of output Tx AGC
*
* @param chain * configure AGC of chain A (0x00) or chain B (0x01)
*        calib_points * 13 calibration points (4 bytes/calib point)
*/
void CID_AGCconf(uint8_t chain, uint8_t* calib_points, uint8_t num_bytes) {
	if (chain > 1 || (num_bytes % 4) != 0) {
		send_ack(0x04, WRONG_PARAM);
		return;
	}

	uint8_t num_points = num_bytes / 4;
	if (num_points > NUM_POINTS) {
		send_ack(0x04, WRONG_PARAM);
		return;
	}

	chain++; // 0 ? 1 (TXA), 1 ? 2 (TXB)
	CalibrationTable* table = (chain == 1) ? &txa_calibration_table : &txb_calibration_table;
	CalibrationTable shadow_table;

	// Parse and fill both the actual and shadow table
	for (uint8_t i = 0, p = 0; p < num_points; i += 4, p++) {
		shadow_table.cbm[p] = table->cbm[p] = (int16_t)((calib_points[i] << 8) | calib_points[i + 1]);
		shadow_table.adc[p] = table->adc[p] = (uint16_t)((calib_points[i + 2] << 8) | calib_points[i + 3]);
	}

	// Retry EEPROM write/read/verify up to 3 times
	for (uint8_t attempt = 1; attempt <= 3; attempt++) {
		write_calibration_table_to_eeprom(chain);
		delay_ms(EEPROM_DELAY);
		load_Txcalibration_table(chain);  // overwrites 'table'

		bool mismatch_found = false;

		for (uint8_t i = 0; i < num_points; i++) {
			if (table->cbm[i] != shadow_table.cbm[i] || table->adc[i] != shadow_table.adc[i]) {
				char buf[80];
				sprintf(buf, "[Try %u] EEPROM Mismatch @%u: CBM 0x%04X != 0x%04X, ADC 0x%04X != 0x%04X\r\n",
				attempt, i,
				(uint16_t)shadow_table.cbm[i], (uint16_t)table->cbm[i],
				shadow_table.adc[i], table->adc[i]);
				UART_send_string(buf);
				// Restore correct value from shadow for next write
				table->cbm[i] = shadow_table.cbm[i];
				table->adc[i] = shadow_table.adc[i];
				mismatch_found = true;
			}
		}

		if (!mismatch_found) {
			if (attempt < 3) {
				char ok_msg[40];
				sprintf(ok_msg, "EEPROM verified OK after %u attempts.\r\n", attempt);
				UART_send_string(ok_msg);
			}
			break;
			} else if (attempt == 3) {
			UART_send_string("EEPROM verification failed after 3 attempts.\r\n");
		}
	}
	send_ack(0x04, ACK);
}

/*
* @brief Command ID loop: set to internal (0) or EXTERNAL (1)
*
* @param chain * configure AGC of chain A (0x00) or chain B (0x01)
*        path * select internal (0x00) or EXTERNAL (0x01)
*/
void CID_loop(uint8_t chain, uint8_t path) {
	
	chain++; //We use 1 for chain 1 and 2 for chain 2
	
	if (path > 1|| chain > 2)
	{
		send_ack(0x05, WRONG_PARAM);
	}
	else
	{
		setTxPath(path, chain);
		send_ack(0x05, ACK);
	}
}

/*
* @brief Command ID setDAC: directly set the value of the DAC
*
* @param chain   * set AGC of chain A (0x00) or chain B (0x01)
*        TxRx    * select TX (0x00) or Rx (0x01) of chain
*		 DAC sel * 0x00-0x02 To select one of 3 DACs per
*        dac_val * value to set the DAC of chain A/B in TX/RX chain
*/
void CID_setDAC(uint8_t chain, uint8_t TxRx, uint8_t DAC, uint16_t dac_val) {
	chain++; // Convert to internal chain index (1/2)
	if (chain > 2 || TxRx > 1 || DAC > 2) {
		send_ack(0x06, WRONG_PARAM);
		return;
	}

	uint16_t remaining = dac_val;

	if (TxRx == 0) {
		struct Tx_status *SelectedChain = (chain == 1) ? &tx.TxA : &tx.TxB;

		for (uint8_t i = DAC; i < 3; i++) {
			uint16_t val = (remaining > DAC_MAX) ? DAC_MAX : remaining;
			SelectedChain->currentDACValue[i] = val;
			remaining -= val;

			send_DAC_package(
			SelectedChain->DAC_PortCS[i],
			SelectedChain->DAC_CS[i],
			SelectedChain->dacChan[i],
			val);
			
			if (remaining == 0) break;  // Exit after distribution is complete
		}
	}
	else {
		struct Rx_PLLs *SelectedChain = (chain == 1) ? &Rx_Chains.RxA : &Rx_Chains.RxB;

		for (uint8_t i = DAC; i < 3 && remaining > 0; i++) {
			uint16_t val = (remaining > DAC_MAX) ? DAC_MAX : remaining;
			SelectedChain->currentDACValue[i] = val;
			remaining -= val;

			send_DAC_package(
			SelectedChain->DAC_PortCS[i],
			SelectedChain->DAC_CS[i],
			SelectedChain->dacChan[i],
			val);
		}
	}

	send_ack(0x06, ACK);
}


/*
* @brief Command ID changeIP_sub_default: Change the IP address, subnet mask and default gateway of the M&C connection
*
* @param ipParameters * array of all the parameters of the IP protocol
*/
void CID_changeIP_sub_default(uint8_t* ipparam) {
	eeprom_update_block((const void*)ipparam, (void*)ip_p, 12);
	send_ack(0x07, ACK);
	//eth_status.F_fin = 1; // re-initialize IP/TCP protocol w5500
}


/*
* @brief Command ID Command ID changePort
*
* @param tcpParameters * array of TCP protocol
*/
void CID_changePort(uint8_t* tcpParameters) {
	
	eeprom_update_block((const void*)tcpParameters, (void*)tcp_p, 2);
	eth_status.F_fin = 1; // re-initialize IP/TCP protocol w5500
}


/*
* @brief Report the health of the MCU (don't restart anything)
*
*/
uint32_t CID_health_check() {
	// Initialize health indicators (1=BAD, 2=GOOD)
	Rx_Chains.RxA.health = 1;
	Rx_Chains.RxB.health = 1;
	tx.TxA.health = 1;
	tx.TxB.health = 1;
	
	// Build peripheral health status word
	uint32_t health = 0;
	
	// ADC health (bits 0-1)
	health = read_id_adc();
	
	// Network and temperature sensor IDs (bits 2-3)
	health |= (read_id_w5500() << 2);
	health |= (readID_TC72() << 3);

	// Temperature check (bit 4)
	TC72_read();
	float temp = (temperature.shrt_arr[0] >> 6) * 0.25 + temperature.shrt_arr[1];
	if (temp >= 0 && temp < 55) {
		health |= BIT(4);
	}

	// Read all PLL GPIO states at once (bits 5-18)
	health |= ((uint32_t)READ_PIN(PLL_GPIOs.RXA_PLLA)       << 5)  |
	((uint32_t)READ_PIN(PLL_GPIOs.RXA_PLLB)       << 6)  |
	((uint32_t)READ_PIN(PLL_GPIOs.RXB_PLLA)       << 7)  |
	((uint32_t)READ_PIN(PLL_GPIOs.RXB_PLLB)       << 8)  |
	((uint32_t)READ_PIN(PLL_GPIOs.TXA_LOGDET)     << 9)  |
	((uint32_t)READ_PIN(PLL_GPIOs.TXB_LOGDET)     << 10) |
	((uint32_t)READ_PIN(PLL_GPIOs.PLL_M_645)      << 11) |
	((uint32_t)READ_PIN(PLL_GPIOs.PLL_M_100)      << 12) |
	((uint32_t)READ_PIN(PLL_GPIOs.PLL_M_3500)     << 13) |
	((uint32_t)READ_PIN(PLL_GPIOs.PLL_M_STW_VHF)  << 14) |
	((uint32_t)READ_PIN(PLL_GPIOs.PLL_M_8_12_ADF) << 15) |
	((uint32_t)READ_PIN(PLL_GPIOs.PLL_S_STW_VHF)  << 16) |
	((uint32_t)READ_PIN(PLL_GPIOs.PLL_S_3500)     << 17) |
	((uint32_t)READ_PIN(PLL_GPIOs.PLL_S_8_12_ADF) << 18);

	// PSU status (bits 19-21)
	health |= ((uint32_t)READ_PIN(REST.PSU_SMB_ALERT) << 19) |
	((uint32_t)READ_PIN(REST.PSU_TTL2)      << 20) |
	((uint32_t)READ_PIN(REST.PSU_TTL1)      << 21);

	perif_health = health;

	// Define health check masks
	const uint32_t TXA_MASK = BIT(0) | BIT(9) | BIT(11) | BIT(13) | BIT(14) | BIT(15);
	const uint32_t TXB_MASK = BIT(0) | BIT(10) | BIT(11) | BIT(16) | BIT(17) | BIT(18);
	const uint32_t RXA_MASK = BIT(1) | BIT(5) | BIT(6);
	const uint32_t RXB_MASK = BIT(1) | BIT(7) | BIT(8);
	
	// Update health status (more efficient comparisons)
	if ((health & TXA_MASK) == TXA_MASK) tx.TxA.health = 2;
	if ((health & TXB_MASK) == TXB_MASK) tx.TxB.health = 2;
	if ((health & RXA_MASK) == RXA_MASK) Rx_Chains.RxA.health = 2;
	if ((health & RXB_MASK) == RXB_MASK) Rx_Chains.RxB.health = 2;

	return health;
}


/*
* @brief Command ID Command Reference: Turns a led green if a reference is connected. Turns a led red if a reference is not connected.
*
* @param con: 0 if reference is not connected = RED
*             1 if reference is connected = GREEN
*/
void CID_Reference(uint8_t con) {
	if (con == 1) led(ledSelection.REF, 2); // green led on to mark the 10MHz is connected
	else if (con == 0) led(ledSelection.REF, 1); // red: error with 10 MHz
	else led(ledSelection.REF, 0); // off: in all other cases
	
	send_ack(0x0D, ACK);
}


/*
* @brief Command ID readADC:
*       If agc loop is off, then read the real-time ADC value from the
*       particular chain RX or TX; otherwise send latest AGC value
*
* @param txOrRx: Select ADC TX (0) or ADC RX (1)
* @param chain: Select Chain A or B
*
* @note  initialized in main
*
*/
void CID_readADC(uint8_t chain, uint8_t txOrRx) {
	if(chain < 2 && txOrRx <2) send_ack(0x0E, ACK);
	else
	{
		send_ack(0x0E,WRONG_PARAM);
		return;
	}
	
	chain++; //adapt chain to 1/2
	uint16_t adcValue;
	
	if(chain == 1 && txOrRx == 0)
	{
		adcValue = tx.TxA.currentADC;
	}
	else if(chain == 2 && txOrRx == 0)
	{
		adcValue = tx.TxB.currentADC;
	}
	else if(chain == 1 && txOrRx == 1)
	{
		adcValue = Rx_Chains.RxA.currentADC;
	}
	else if(chain == 2 && txOrRx == 1)
	{
		adcValue = Rx_Chains.RxB.currentADC;
	}
	send_adc_value(--chain, txOrRx, adcValue);
	
}


/*
* reset the calibration values of the Tx chain to defaults
*/
void CID_resetCalAGC(uint8_t chain) {
	// select which calibration tables to be reset
	if(chain>1)
	{
		send_ack(0x0F,WRONG_PARAM);
		return;
	}
	chain++;
	reset_tx_calibration(chain);
	send_ack(0x0F, ACK);
}



/*
* @brief Notification ID version number: return the version number of the Atmel SW
*
*/
void CID_version() {
	send_ack(0x10, ACK);
	
	uint8_t version[4];
	version[0] = 0x10;
	version[1] = VERSION_N1; // represents counter (big changes)
	version[2] = VERSION_N2; // represents changeset (MSB)
	version[3] = VERSION_N3; // represents changeset (LSB)
	w5500_TXsend(SOCKET_0, version, 4);
}


/*
* @brief Command ID to select the output and input path (setting of filters)
*
* @param chain: chain A or B 0/1
* @param functionality:  (tx/tx_logDetector/rx) (0-1-2)
* @param path: path selection (tx -> x0-0xA)  (rx -> x0-0x2) (log_detector -> x0-0x8)
*Tx -> (0=60-100M) (1=100M+-180M) (2=180M+-350M) (3=350M+-520M) (4=520M+-680M) (5=680M+-920M) (6=920M+-1500M) (7=1500M+-2200M) (8=2200M+-3200M) (9=3200M+-4000M) (A=4000M+-4401M)
*Rx -> (1=60-2000M) (0=2000M+-6660M) (2=2000M+-6660M + mixer)
*Tx_logDet -> (0=60M-630M) (1=630M+-850M) (2=850M+-1400M) (3=1400+-1800M) (4=1800M+-2400M) (5=2400M+-3100M) (6=3100M+-3500M) (7=3500M+-3800M) (8=3800M+-4300M)
*/
void CID_manual_filter_selection(uint8_t chain, uint8_t functionality, uint8_t filter_selection) {
	
	// Validate chain
	if (chain > 1 || functionality < 1 || functionality > 3) {
		send_ack(0x12, WRONG_PARAM);
		return;
	}
	
	chain++; //Compatible with MCU chain numbering

	// Validate functionality and corresponding filter_selection range
	switch (functionality) {
		default:
		case 1:  // Functionality 1: filter_selection 0 to 0x0A
		if (filter_selection <= 0x0A) {
			send_ack(0x12, ACK);
			manual_tx_filter(chain,filter_selection);
			return;
		}
		break;

		case 2:  // Functionality 2: filter_selection 0 to 0x08
		if (filter_selection <= 0x08) {
			send_ack(0x12, ACK);
			manual_txLogDet_filter(chain,filter_selection);
			return;
		}
		break;

		case 3:  // Functionality 3: filter_selection 0 to 0x02
		if (filter_selection <= 2 ) {
			send_ack(0x12, ACK);
			manual_rx_filter(chain,filter_selection);
			return;
		}
		break;
	}

	// Any invalid case
	send_ack(0x12, WRONG_PARAM);
}



/*
* @brief This command sets the correct filter path and PLL frequency
*        for the prefered chain Tx or Rx. It also modifies the calibration
*        if a different band was chosen.
*
* @param chain: CHAIN_A (0) or CHAIN_B (1)
* @param TxRx: Tx == 0 and Rx == 1
* @param freq: carrier frequency of the uplink or downlink (in MHz)
*/
void CID_set_freq(uint8_t chain, uint8_t TxRx, uint16_t freq) {
	
	/* Determine the filter path */
	if (freq < 60 || freq > 6600 || chain > 1 || TxRx > 1 ) {
		send_ack(0x13,WRONG_PARAM);
		return;
	}
	long long scaledFreq = freq * SCALE_FACTOR;
	chain++;
	if(TxRx == 0)
	{
		change_Tx_Frequency(scaledFreq,chain);
	}
	else if(TxRx == 1)
	{
		change_Rx_Frequency(scaledFreq,chain);
	}
	send_ack(0x13, ACK);
}


/*
* This command returns the configured setpoint for the preferred chain Tx or Rx
* @param chain: CHAIN_A (0) or CHAIN_B (1)
* @param TxRx: Tx == 0 and Rx == 1
*/
void CID_readsetpoint(uint8_t chain, uint8_t TxRx) {
	if (chain < 2 && TxRx < 2) {
		chain++;
		send_ack(0x15, ACK);
		send_setpoint(chain, TxRx);
		} else {
		send_ack(0x15, WRONG_PARAM);
	}
}


/*
* Turn on/off the AGC
* @param onoff: toggles the AGC
* 0: MGC is activated (AGC off)
* 1: AGC is activated (MGC off)
*/
void CID_toggle_agc(uint8_t onoff) {
	if (onoff <= 0x01)
	{
		Rx_Chains.RxA.agcEnable = Rx_Chains.RxB.agcEnable = tx.TxB.agcEnable = tx.TxA.agcEnable = onoff;
		send_ack(0x14, ACK);
	}
	else
	{
		send_ack(0x14, WRONG_PARAM);
	}
}

/*
* @brief Command ID Command Ethernet factory reset: write default values
*
*/
void CID_eth_factrst() {
	
	uint8_t def_ip_param[] = {192,168,50,10,255,255,255,0,192,168,50,1}; //default IP, subnet mask and default gateway
	CID_changeIP_sub_default(def_ip_param);
	uint8_t def_tcp_param[] = {0x13,0x88}; //default TCP port: 5000
	CID_changePort(def_tcp_param);
	eth_status.F_fin = 1;
	led(ledSelection.ALL,2); //Flash all LEDs to indicate reset
	delay_ms(100); //delay for user to see LED flashing
	send_ack(0x16, ACK);
	//
	///* reboot */
	//while (1) {
	//// do nothing until watchdog triggers
	//};
}


/*
* @brief Command ID read_dac:    Read the real-time ADC value (hex) from the particular chain RX or TX.
* @param TxRx_select:           Select DAC TX (0) or DAC RX (1)
*        chain:                 Select Chain A or B
* @note  initialized in main
*
*/
void CID_read_dac(uint8_t chain, uint8_t TxRx_select) {
	if(chain < 2 && TxRx_select <2)
	{
		send_ack(0x17, ACK);
		///* get new DAC measurement and send it */
		chain++;
		send_dac_value(chain, TxRx_select);
	}
	else send_ack(0x09,WRONG_PARAM);
	
}

void CID_read_agc_calibration(uint8_t chain) {
	if(chain<2)
	{
		chain++;
		send_ack(0x18, ACK);
		send_read_agc_calib(chain);
	}
	else send_ack(0x18,WRONG_PARAM);
}


/*
* @brief Command ID set carrier:  Awareness of the state of the carrier allows to influence AGC behaviour.
* @param carr_state:  Can be carrier off (0) or on (1)
*        chain:    Select Chain A or B
* @note  initialized in main
*
*/
void CID_set_carrier(uint8_t chain, uint8_t carr_state) {
	if (chain < 2 && carr_state < 2) {
		chain++; // Convert 0/1 to 1/2

		send_ack(0x1a, ACK);

		struct Tx_status* SelectedChain = (chain == 1) ? &tx.TxA : &tx.TxB;

		SelectedChain->isItOn = SelectedChain->agcEnable = carr_state;

		uint16_t val = (carr_state == 0) ? 0 : DAC_MIN;
		SelectedChain->currentDACValue[0] = val;
		SelectedChain->currentDACValue[1] = val;
		SelectedChain->currentDACValue[2] = val;

		if (chain == 1)
		setupDACTxA();
		else
		setupDACTxB();
		} else {
		send_ack(0x1a, WRONG_PARAM);
	}
}

//void CID_read_eeprom() {
//
//send_ack(0x1b, ACK);
//
///* dump complete EEPROM in one message */
//send_eeprom();
//}

/*
* @brief Command ID reboots/power off:  Will handle reboot or power off of unit.
* @param carr_state:  Can be unit off (1) or reboot (0)
* @note  initialized in main
*
*/
void CID_unit_power(uint8_t state)
{
	if(state < 2)
	{
		send_ack(0x19, ACK);
		if (state == 0)
		{
			w5500_disconnect_then_abort(SOCKET_0,500); //Wait for 500ms for mercifull disconection else close TCP connection bruttaly
			*REST.PSU_CONTROL.PORT &= ~(1 << REST.PSU_CONTROL.PIN);
			wdt_enable(WDTO_2S); // Trigger reset
			while (1); // Wait for watchdog to fire
		}
		else if (state == 1)
		{
			powerHandling(); //Power off and wait for ON
		}
	}
	else send_ack(0x19, WRONG_PARAM);
}

void CID_returnEthConfig()
{
	send_ack(0x1D, ACK);

	uint8_t buffer[8];  // 4 bytes subnet + 4 bytes gateway
	eeprom_read_block(buffer, (const void*)(ip_p + 4), 8);

	uint64_t stitched = 0;

	// Stitch bytes into one 64-bit integer (big-endian style)
	for (uint8_t i = 0; i < 8; i++) {
		stitched = (stitched << 8) | buffer[i];
	}

	// Convert stitched 64-bit into a byte array for transmission
	uint8_t tx_bytes[8];
	for (uint8_t i = 0; i < 8; i++) {
		tx_bytes[7 - i] = (uint8_t)(stitched >> (i * 8));  // big-endian order
	}
    w5500_TXsend(SOCKET_0, tx_bytes, sizeof(tx_bytes));	
}

void CID_select_rx_path(uint8_t chain, uint8_t path)
{
	if (chain < 2 && path < 4) {
		send_ack(0x1c, ACK);

		chain++; // chain: 0->1, 1->2

		if (path == 3) {
			// If path == 3, do NOT update Input
			// But only reset RxA (since chain 0 means RxA)
			if (chain == 1) {
				Rx_Chains.RxA.isItOn = Rx_Chains.RxA.agcEnable = 0;
				
				Rx_Chains.RxA.currentDACValue[2] = Rx_Chains.RxA.currentDACValue[1] = Rx_Chains.RxA.currentDACValue[0] = DAC_MIN;
				setupDACRxA();
			}
			if (chain == 2) {
				Rx_Chains.RxB.isItOn = Rx_Chains.RxB.agcEnable = 0;

				Rx_Chains.RxB.currentDACValue[1] =	Rx_Chains.RxB.currentDACValue[2] = Rx_Chains.RxB.currentDACValue[0] = DAC_MIN;
				setupDACRxB();
			}
			return;
		}
		else
		{
			// Normal case: update Input for chain 1 or 2
			if (chain == 1) {
				Rx_Chains.RxA.isItOn = Rx_Chains.RxA.agcEnable = 1;
				Rx_Chains.RxA.Input = path;
			}
			else if (chain == 2) {
				Rx_Chains.RxB.isItOn = Rx_Chains.RxB.agcEnable = 1;
				Rx_Chains.RxB.Input = path;
			}
			configurePortExpRx();
		}
	}
	else {
		send_ack(0x1c, WRONG_PARAM);
	}
}
//////////////////////////////////////////////////////////////////////////
/*                      NOTIFICATIONS Ethernet                          */
//////////////////////////////////////////////////////////////////////////

/*
* @brief Sends error ID or ACK to client
*
* @param CID: command ID
* @param error_id: ID of which error occured
*/
void send_ack(uint8_t CID, uint8_t error_id)
{
	uint8_t transmit_data[3];
	
	transmit_data[0] = CID_NAK;
	transmit_data[1] = CID;
	transmit_data[2] = error_id;
	w5500_TXsend(SOCKET_0, transmit_data, 3);
}

/*
* @brief Notification ID ADC value: sends the read ADC value to client
*
* @param adc_hex: ADC value which is read over SPI.
*/
void send_adc_value(uint8_t chain, uint8_t ADCselect, uint16_t adc_hex)
{
	uint8_t transmit_data[5];
	
	transmit_data[0] = 0x0E; // CID
	transmit_data[1] = chain;
	transmit_data[2] = ADCselect;
	transmit_data[3] = (uint8_t)(adc_hex >> 8) ;
	transmit_data[4] = (uint8_t)(adc_hex >> 0) ;
	w5500_TXsend(SOCKET_0, transmit_data, 5);
}

/*
* @brief Notification ID ADC value: sends the read ADC value to client
*
* @param chain : chain A or B selection
# @param TxRxselect: Rx (1) or Tx (0) selection
*/
void send_dac_value(uint8_t chain, uint8_t TxRxselect)
{
	uint8_t transmit_data[9];      // 3 header bytes + 3 DAC values � 2 bytes = 9
	short_msg_t dac_val;
	volatile uint16_t *src_dac = NULL;

	transmit_data[0] = 0x17;        // CID
	transmit_data[1] = chain-1;
	transmit_data[2] = TxRxselect;

	// === Get source DAC pointer ===
	if (TxRxselect) { // Rx
		if (chain == 1)
		src_dac = Rx_Chains.RxA.currentDACValue;
		else if (chain == 2)
		src_dac = Rx_Chains.RxB.currentDACValue;
		} else { // Tx
		if (chain == 1)
		src_dac = tx.TxA.currentDACValue;
		else if (chain == 2)
		src_dac = tx.TxB.currentDACValue;
	}

	if (src_dac == NULL) {
		UART_send_string("Invalid chain or TxRx in send_dac_value()\r\n");
		return;
	}

	// === Copy all 3 DAC values (MSB first) ===
	for (uint8_t i = 0; i < 3; i++) {
		dac_val.shrt = src_dac[i];
		transmit_data[3 + (i * 2)] = dac_val.shrt_arr[1];  // MSB
		transmit_data[4 + (i * 2)] = dac_val.shrt_arr[0];  // LSB
	}
	// === Send all 9 bytes ===
	w5500_TXsend(SOCKET_0, transmit_data, 9);
}

/*
* @brief Notification ID status:
*
* @param chainid: can be 0 or 1
*/
void send_status(uint8_t chainid)
{
	struct Tx_status *TxSelectedChain = (chainid == 1) ? &tx.TxA : &tx.TxB;
	struct Rx_PLLs   *RxSelectedChain = (chainid == 1) ? &Rx_Chains.RxA : &Rx_Chains.RxB;

	uint8_t transmit_data[6];
	uint16_t led = ((uint16_t)led_port_a << 8) | led_port_b;

	transmit_data[0] = 0x02;
	transmit_data[1] = chainid - 1;
	transmit_data[2] = (TxSelectedChain->isItOn == 0) ? 0x02 : TxSelectedChain->Output; //0x02 means Tx is off
	transmit_data[3] = (RxSelectedChain->isItOn == 0) ? 0x03 : RxSelectedChain->Input; //0x03 means rx is off

	uint8_t tx_c, rx_aux_c, rx_main_c, ref_c, health_c;

	if (chainid == 1) {
		tx_c       = ((led >> 3) & 1) | (((led >> 2) & 1) << 1);  // B3,R + B2,G
		rx_aux_c   = ((led >> 7) & 1) | (((led >> 6) & 1) << 1);  // B7,R + B6,G
		rx_main_c  = ((led >> 5) & 1) | (((led >> 4) & 1) << 1);  // B5,R + B4,G
		} else {
		tx_c       = ((led >> 13) & 1) | (((led >> 12) & 1) << 1); // A5,R + A4,G
		rx_aux_c   = ((led >> 1) & 1) | (((led >> 0) & 1) << 1);   // B1,R + B0,G
		rx_main_c  = ((led >> 15) & 1) | (((led >> 14) & 1) << 1); // A7,R + A6,G
	}

	ref_c     = ((led >> 11) & 1) | (((led >> 10) & 1) << 1); // A3,R + A2,G
	health_c  = ((led >> 9) & 1) | (((led >> 8) & 1) << 1);   // A1,R + A0,G

	// Health override using perif_health
	uint32_t ph = perif_health & 0x0007FFFF;
	if (ph == 0x0007FFFF)      health_c = 2; // Green
	else if (ph == 0x0007EFFF) health_c = 3; // Orange
	else                       health_c = 1; // Red

	// Pack
	transmit_data[4] = (tx_c << 6) | (rx_aux_c << 4) | (rx_main_c << 2) | (ref_c & 0x03);
	transmit_data[5] = (health_c << 6);
	send_ack(0x02, ACK);

	w5500_TXsend(SOCKET_0, transmit_data, 6);
}
/*
* @brief Notification ID serial number: Sends the serial number of the equipment to the user
*
*/
void send_serial_nr(uint8_t sn)
{
	uint8_t serialbuf[9];
	if (sn)
	{
		eeprom_read_block((void*)(serialbuf+1), (const void*) sn2_p, 8);
		serialbuf[0] = 0x0B;
	}
	else
	{
		eeprom_read_block((void*)(serialbuf+1), (const void*) sn1_p, 8);
		serialbuf[0] = 0x0A;
	}
	w5500_TXsend(SOCKET_0, serialbuf, 9);
}


/*
* @brief Notification ID health status: sends the response of the peripherals to the client
*
* @param output of CID_healthCheck
*/
void send_health_stat(uint32_t hlth)
{
	uint8_t buffer[4];
	buffer[0] = 0x09; // Command ID
	buffer[1] = (hlth >> 16) & 0xFF; // Most significant byte (bits 16 to 23)
	buffer[2] = (hlth >> 8) & 0xFF;  // Middle byte (bits 8 to 15)
	buffer[3] = hlth & 0xFF;         // Least significant byte (bits 0 to 7)

	w5500_TXsend(SOCKET_0, buffer, 4);
}

/*
* @brief Notification ID health status: sends the response of the peripherals to the client
*
*/
void send_temp()
{
	//uint16_t temp = temperature.shrt;

	uint8_t tx_buffer[3];
	tx_buffer[0] = 0x0C;
	tx_buffer[1] = (uint8_t)(temperature.shrt >> 8);
	tx_buffer[2] = (uint8_t)(temperature.shrt & 0xFF);

	w5500_TXsend(SOCKET_0, tx_buffer, 3);
}



/*
* @brief
*/
void send_setpoint(uint8_t chain, uint8_t txrx)
{
	uint8_t msg[7];
	int16_t in_lvl = 0xFFFF;
	uint16_t setpoint = 0x0000;
	
	if(txrx == 0)
	{
		if(chain == 1) setpoint = tx.TxA.targetADC;
		
		else setpoint = tx.TxB.targetADC;
	}
	else if(txrx == 1)
	{
		//UART_send_string("offset\r\n");
		if(chain == 1)
		{
			setpoint = Rx_Chains.RxA.targetADC;
			in_lvl = (Rx_Chains.RxA.Input == 1)? rxa_calibration.aux.offset : rxa_calibration.main.offset;
		}
		
		else
		{
			setpoint = Rx_Chains.RxB.targetADC;
			in_lvl = (Rx_Chains.RxB.Input == 1)? rxb_calibration.aux.offset : rxb_calibration.main.offset;
		}
	}
	
	msg[0] = 0x15;
	msg[1] = chain-1;
	msg[2] = txrx;
	msg[3] = (in_lvl>>8) & 0xFF;
	msg[4] = in_lvl & 0xFF;
	msg[5] = (setpoint>>8) & 0xFF;
	msg[6] = setpoint & 0xFF;
	
	w5500_TXsend(SOCKET_0, msg, 7);
}

/*
* @brief
*/
void send_read_agc_calib(uint8_t chain)
{
	uint8_t msg[2 + NUM_POINTS * 4];
	CalibrationTable* cal;

	if (chain == 1)
	cal = &txa_calibration_table;
	else if (chain == 2)
	cal = &txb_calibration_table;
	else
	return;

	msg[0] = 0x18;
	msg[1] = chain-1;

	for (int j = 0; j < NUM_POINTS; j++)
	{
		int16_t cbm = cal->cbm[j];
		uint16_t adc = cal->adc[j];

		int i = 2 + j * 4;
		msg[i + 0] = (cbm >> 8) & 0xFF;
		msg[i + 1] = cbm & 0xFF;
		msg[i + 2] = (adc >> 8) & 0xFF;
		msg[i + 3] = adc & 0xFF;
	}

	w5500_TXsend(SOCKET_0, msg, sizeof(msg));
}

/*
* @brief Dump complete eeprom in 32 messages
*/
// void send_eeprom()
// {
//void *pointer_eeprom;
//uint8_t msg[128];
//
//for (int i = 0; i < 32; i++) {
//pointer_eeprom = i * 128;
//eeprom_read_block((void*)msg, (const void*)pointer_eeprom,  128);
//w5500_TXsend(SOCKET_0, msg, 128);
//wdt_reset();
//}
// }

//////////////////////////////////////////////////////////////////////////
/*                               Receive command                        */
//////////////////////////////////////////////////////////////////////////

/* index is CID and value is parameter length */
/*                 CID:   0  1  2  3   4  5  6   7  8  9  A  B  C  D  E  F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C*/
uint8_t CID_paramlen[] = {2, 2, 1, 8, 45, 2, 5, 12, 2, 0, 0, 0, 0, 1, 2, 1, 0, 3, 3, 4, 1, 2, 0, 2, 1, 1, 2, 0, 2};

/*
* @brief Decoding of the received data: Decoding the record marker + CID and checks the format of all the data
*
* @param *RXdata * pointer to received data array
*        RX_data_len * length of received data
*/
uint8_t eth_recv_command(uint8_t *RX_data)
{
	uint8_t CID = 0; // command ID
	int8_t paramlen = 0; // size of parameter list in bytes

	//char buffer[10];
	//sprintf(buffer, "0x%02X\n", RX_data[4]);
	//UART_send_string(buffer);
	
	if(RX_data[0] == HEADMARKER_ID) //ID OK
	{
		paramlen = RX_data[3] - 4 - 1; // length - recordmarker(4) - CID_len(1)
		CID = RX_data[4]; // command ID
		
		if (paramlen < 0 || paramlen > MAX_DATA_LEN)
		{
			/* size of complete command is too small or too large */
			send_ack(CID, WRONG_CMD_SIZE);
			return MAX_DATA_LEN;
		}
		/* check expected parameter length */
		if (CID < (sizeof(CID_paramlen)/sizeof(CID_paramlen[0])))
		{
			// CID is valid, now check the parameter list
			if (paramlen != CID_paramlen[CID])
			{
				/* NACK: expected parameter list does not match the received parameter sizes */
				send_ack(CID, WRONG_DATA_SIZE);
				return MAX_DATA_LEN;
			}
			//else
			// do nothing; ACK/NACK is sent after execution of command
		}
		/* pet watchdog before each command because several commands at once could take more than 2 s */
		wdt_reset();
		//UART_send_string("X");
		switch (CID)
		{
			case 0x00:
			{
				CID_led(RX_data[5], RX_data[6]); //LED expects 2 parameters: led & state (color)
			}
			break;

			case 0x01:
			{
				send_ack(CID,NOT_IN_USE); //Command not in use atm
				//CID_TXpower(RX_data[5], RX_data[6]); //TX power with 2 param: chain and on/off
			}
			break;

			case 0x02:
			{
				CID_getStatus(RX_data[5]); //get_status with 1 parameter: chain
			}
			break;

			case 0x03: //agc_set
			//// expects 5 parameters: chain, TxRx, input level, setpoint & time constant
			{
				//UART_send_string("cal\r\n");
				// receive in centiBell (cB) tenths of a deciBell, setp in
				int16_t in_lvl = (((int16_t)RX_data[7])<<8) | (int16_t)RX_data[8];
				uint16_t setp = (((uint16_t)RX_data[9])<<8) | (uint16_t)RX_data[10];
				uint16_t timeconstant = (((uint16_t)RX_data[11])<<8) | (uint16_t)RX_data[12];
				CID_AGCset(RX_data[5], RX_data[6], in_lvl, setp, timeconstant);
			}
			break;

			case 0x04:
			{
				CID_AGCconf(RX_data[5], RX_data+6,paramlen-1); //agc_conf only tx
			}
			break;

			case 0x05:
			{
				CID_loop(RX_data[5],RX_data[6]); // RF switchboard: expects 2 parameters(chain & path)
			}
			break;

			case 0x06: //set_DAC . Expects 5 parameters: chain, TxRx, DAC, dac_val(2x8)
			{
				uint16_t dachex = RX_data[8];
				dachex = dachex<<8 | RX_data[9];
				CID_setDAC(RX_data[5], RX_data[6],RX_data[7],dachex);
			}
			break;

			case 0x07: //change_ip_sub
			// expects 3 parameters: IP address (4 bytes) + subnet (4 bytes) + default gateway (4 bytes)
			{
				CID_changeIP_sub_default(RX_data+5);
				//re-initialize the IP/TCP protocol w5500
				while (1) {};// do nothing until watchdog triggers
			}
			break;

			case 0x08:
			{
				uint8_t eeprom_TCP[2];
				eeprom_read_block((void*)eeprom_TCP, (const void*)tcp_p, 2);
				uint16_t TCP = (eeprom_TCP[0] << 8) | eeprom_TCP[1];
				
				// Read the new TCP port from RX_data (assuming it's also 2 bytes in big-endian format)
				uint16_t new_TCP = (RX_data[5] << 8) | RX_data[6];
				send_ack(0x08, ACK);
				
				if(TCP != new_TCP)
				{
					CID_changePort(RX_data+5); // change port number of TCP protocol (2 bytes)
				}
			}
			break;

			case 0x09:      //Health status
			{
				uint32_t hhhealth = CID_health_check();
				send_ack(CID, ACK);
				send_health_stat(hhhealth);
			}
			break;

			case 0x0A:      // Read serial number 1
			{
				send_ack(CID, ACK);
				send_serial_nr(0); //transmit serial number
			}
			break;

			case 0x0B:      // Read serial number 2
			{
				send_ack(CID, ACK);
				send_serial_nr(1); //transmit serial number
			}
			break;

			case 0x0C:      // Read temp
			{
				send_ack(CID, ACK);
				send_temp(); //transmit temperature
			}
			break;

			case 0x0D:
			{
				CID_Reference(RX_data[5]); //10MHz connected
			}
			break;

			case 0x0E:
			{
				CID_readADC(RX_data[5], RX_data[6]); //ADC READ
			}
			break;

			case 0x0F:
			{
				CID_resetCalAGC(RX_data[5]); //Reset calibration table AGC
			}
			break;

			case 0x10:
			{
				CID_version(); //Return the version number
			}
			break;

			case 0x11: // Set output level in cBm
			{
				uint8_t chain = RX_data[5]+1;
				int16_t power= RX_data[6];
				power = power <<8 | RX_data[7];
				
				if(chain == 1)
				{
					tx.TxA.carrierPower = power;
				}
				else if (chain == 2)
				{
					tx.TxB.carrierPower = power;
				}
				else
				{
					send_ack(CID, WRONG_PARAM);
					break;
				}
				send_ack(CID, ACK);
				set_tx_out_power(chain); // set the output power
			}
			break;
			
			case 0x12:  // set filterpath independant of the frequency for testing only !
			{
				CID_manual_filter_selection(RX_data[5], RX_data[6], RX_data[7]);
			}
			break;
			
			case 0x13: // select GPIO extender via frequency
			{
				uint16_t freq = RX_data[7];
				freq = freq<<8 | RX_data[8];
				CID_set_freq(RX_data[5], RX_data[6], freq);
			}
			break;
			
			case 0x14:
			{
				CID_toggle_agc(RX_data[5]); //Toggle the AGC
			}
			break;
			
			case 0x15:
			{
				CID_readsetpoint(RX_data[5], RX_data[6]); // Read setpoint
			}
			break;

			case 0x16: // Factory reset Ethernet
			{
				if (paramlen == 0) // expects 0 parameters
				{
					send_ack(CID, ACK);
					CID_eth_factrst();
				}
				else send_ack(CID, WRONG_DATA_SIZE);
			}
			break;
			
			case 0x17: CID_read_dac(RX_data[5], RX_data[6]); // Read DAC values
			break;
			
			case 0x18: CID_read_agc_calibration(RX_data[5]);
			break;

			case 0x19: CID_unit_power(RX_data[5]);
			break;

			case 0x1a: CID_set_carrier(RX_data[5], RX_data[6]);
			break;

			case 0x1b: send_ack(CID,NOT_IN_USE); // Command not in use atm
			break;
			
			case 0x1c:
			{
				//char buffer[16];
				//UART_send_string("path\r\n");
				//sprintf(buffer, "%u\r\n", RX_data[6]);  // Convert uint8_t to string
				//UART_send_string(buffer);
				
				CID_select_rx_path(RX_data[5], RX_data[6]);
			}
			break;
			
			case 0x1D: // return subnet and default gateway
			{
				CID_returnEthConfig();
			}
			break;

			default:
			send_ack(CID, UNKNOWN_CMD_ID); // ERROR_ID: UNKNOWN COMMAND ID (0x01)
		}
		return RX_data[3];
	}
	else
	{ //ID NOK
		// Transmit NAK ERROR_ID wrong recordmarker
		send_ack(CID, WRONG_RECORDMARKER);
		return MAX_DATA_LEN;
	}
}





