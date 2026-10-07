/*
* agc.c
*
* Created: 30/06/2025 16:59:48
*  Author: constantinos.pavlide
*/
#include <agc.h>

//bool agcEnable = true;

CalibrationTable txa_calibration_table, txb_calibration_table;
CalibrationTable default_tx_calibration_table = { //Default Values limit the power to 0dbm
	.cbm = {100,   0,  -50, -100,-200,-300,-400,-500,-600,-700,-800},
	.adc = {3500,3470,3255,3050,2700,2300,1870,1480,1335,1300,1300},
};

//CalibrationTable default_tx_calibration_table = {
//.cbm = {100,   0,  -50, -100,-200,-300,-400,-500,-600,-700,-800},
//.adc = {3560,3470,3255,3050,2700,2300,1870,1480,1335,1300,1300},
//};

rxPoints rxa_calibration = {
	.aux  = { .offset = -200, .dac = 1570},
	.main = { .offset = -180, .dac = 1550}
};

rxPoints rxb_calibration = {
	.aux  = { .offset = -200, .dac = 1860},
	.main = { .offset = -180, .dac = 1860}
};



void load_Rxcalibration(uint8_t chain)
{
	rxPoints *calPoints;
	struct EEPROMs_IC *eeprom;
	
	if(chain == 1)
	{
		calPoints =  &rxa_calibration;
	}
	else
	{
		calPoints =  &rxb_calibration;
	}
	
	eeprom = &EEPROMs_GPIOs.RXB_ROM; //All Rx calibrations are saved in RXB eeprom
	EnableSPI_FOR(SPI_route.RXB);
	
	delay_ms(EEPROM_DELAY);
	
	uint32_t address = get_calibration_address(chain,0);
	
	if (address == 0xFFFFFFFF)
	{
		EnableSPI_FOR(SPI_route.MCU_ONLY);
		return;
	}

	// Create a pointer array to main and aux blocks
	volatile rxCalBlock* blocks[2] = { &calPoints->main, &calPoints->aux };
	uint8_t high;
	uint8_t low;
	// Read DAC values
	for (int p = 0; p < 2; p++) {
		high = readEEPROM(eeprom, address++);
		delay_ms(EEPROM_DELAY);
		low  = readEEPROM(eeprom, address++);
		delay_ms(EEPROM_DELAY);
		blocks[p]->dac = (uint16_t)((high << 8) | low);
	}

	// Read offset values
	for (int p = 0; p < 2; p++) {
		high = readEEPROM(eeprom, address++);
		delay_ms(EEPROM_DELAY);
		low  = readEEPROM(eeprom, address++);
		delay_ms(EEPROM_DELAY);
		blocks[p]->offset = (int16_t)((high << 8) | low);
	}

	//Read t_const values
	//for (int p = 0; p < 2; p++) {
	//high = readEEPROM(eeprom, address++);
	//delay_ms(EEPROM_DELAY);
	//low  = readEEPROM(eeprom, address++);
	//delay_ms(EEPROM_DELAY);
	//blocks[p]->t_constant = (uint16_t)((high << 8) | low);
	//}
	
	EnableSPI_FOR(SPI_route.MCU_ONLY);
	
	calculate_rx_in_power(chain);
}

void write_calibration_points_to_eeprom(uint8_t chain)
{
	rxPoints *calPoints;
	struct EEPROMs_IC *eeprom;
	struct EEPROMs_IC *eepromBKP;
	
	if(chain == 1)
	{
		calPoints =  &rxa_calibration;
	}
	else
	{
		calPoints =  &rxb_calibration;
	}
	
	eeprom = &EEPROMs_GPIOs.RXB_ROM;
	eepromBKP = &EEPROMs_GPIOs.MCU_2_ROM;

	EnableSPI_FOR(SPI_route.RXB);
	
	uint32_t base_address = get_calibration_address(chain,0); // This already returns a byte address
	
	if (base_address == 0xFFFFFFFF) { // invalid address
		EnableSPI_FOR(SPI_route.MCU_ONLY); //Disable all SPI routes
		return;
	}

	volatile rxCalBlock* blocks[2] = { &calPoints->main, &calPoints->aux };
	

	// Write DAC values
	for (int p = 0; p < 2; p++) {
		uint16_t dac = blocks[p]->dac;
		writeEEPROM(eeprom, (uint8_t)(dac >> 8), base_address);
		writeEEPROM(eepromBKP, (uint8_t)(dac >> 8), base_address++);
		delay_ms(EEPROM_DELAY);
		writeEEPROM(eeprom, (uint8_t)(dac & 0xFF), base_address);
		writeEEPROM(eepromBKP, (uint8_t)(dac & 0xFF), base_address++);
		delay_ms(EEPROM_DELAY);
	}

	// Write offset values
	for (int p = 0; p < 2; p++) {
		int16_t offset = blocks[p]->offset;
		writeEEPROM(eeprom, (uint8_t)(offset >> 8), base_address);
		writeEEPROM(eepromBKP, (uint8_t)(offset >> 8), base_address++);
		delay_ms(EEPROM_DELAY);
		writeEEPROM(eeprom, (uint8_t)(offset & 0xFF), base_address);
		writeEEPROM(eepromBKP, (uint8_t)(offset & 0xFF), base_address++);
		delay_ms(EEPROM_DELAY);
	}
	EnableSPI_FOR(SPI_route.MCU_ONLY);
}


void load_Txcalibration_table(uint8_t chain)
{
	if (chain != 1 && chain != 2)
	return; // Invalid chain
	struct Tx_status *SelectedChain;
	CalibrationTable *table;
	//CalibrationTable *table = (chain == 1) ? &txa_calibration_table : &txb_calibration_table;

	if(chain == 1)
	{
		SelectedChain = &tx.TxA;
		table = &txa_calibration_table;
	}
	else
	{
		SelectedChain = &tx.TxB;
		table = &txb_calibration_table;
	}

	uint32_t address = get_calibration_address(chain,1); //IN case of invalid address return do this
	if (address == 0xFFFFFFFF) {
		// Invalid address, load default calibration
		memcpy(table, &default_tx_calibration_table, sizeof(CalibrationTable));
		SelectedChain->uncalibratedFreq = 1;
		set_tx_out_power(chain);
		return;
	}
	
	for (uint8_t i = 0; i < NUM_POINTS; i++) { //Load the cbm saved values
		uint8_t high = readEEPROM(&EEPROMs_GPIOs.MCU_1_ROM, address++);
		delay_ms(EEPROM_DELAY);
		uint8_t low  = readEEPROM(&EEPROMs_GPIOs.MCU_1_ROM, address++);
		delay_ms(EEPROM_DELAY);
		table->cbm[i] = (int16_t)((high << 8) | low);
	}
	
	if (table->cbm[0] == 0xFFFF && table->cbm[NUM_POINTS - 1] == 0xFFFF) { //if loaded cbm values are FFFF use default calibration and return
		// Uninitialized table, load default values
		memcpy(table, &default_tx_calibration_table, sizeof(CalibrationTable));
		SelectedChain->uncalibratedFreq = 1;
		set_tx_out_power(chain);
		return;
	}

	for (uint8_t i = 0; i < NUM_POINTS; i++) {  // Load ADC values of calibrated value
		SelectedChain->uncalibratedFreq = 0;
		uint8_t high = readEEPROM(&EEPROMs_GPIOs.MCU_1_ROM, address++);
		delay_ms(EEPROM_DELAY);
		uint8_t low  = readEEPROM(&EEPROMs_GPIOs.MCU_1_ROM, address++);
		delay_ms(EEPROM_DELAY);
		table->adc[i] = (uint16_t)((high << 8) | low);
	}
	
	set_min_allowed_adc(chain);	
	set_tx_out_power(chain);
}

void write_calibration_table_to_eeprom(uint8_t chain)
{
	uint32_t base_address = get_calibration_address(chain,1); // This already returns a byte address
	CalibrationTable* table = (chain == 1) ? &txa_calibration_table : &txb_calibration_table;

	// Write power levels
	for (uint8_t i = 0; i < NUM_POINTS; ++i)
	{
		int16_t value = table->cbm[i];
		writeEEPROM(&EEPROMs_GPIOs.MCU_1_ROM, (uint8_t)(value >> 8), base_address++);
		delay_ms(EEPROM_DELAY);
		writeEEPROM(&EEPROMs_GPIOs.MCU_1_ROM, (uint8_t)(value & 0xFF), base_address++);
		delay_ms(EEPROM_DELAY);
	}

	// Write ADC values
	for (uint8_t i = 0; i < NUM_POINTS; ++i)
	{
		uint16_t value = table->adc[i];
		writeEEPROM(&EEPROMs_GPIOs.MCU_1_ROM, (uint8_t)(value >> 8), base_address++);
		delay_ms(EEPROM_DELAY);
		writeEEPROM(&EEPROMs_GPIOs.MCU_1_ROM, (uint8_t)(value & 0xFF), base_address++);
		delay_ms(EEPROM_DELAY);
	}
}

void calculate_rx_in_power(uint8_t chain)
{
	struct Rx_PLLs *selected_chain = NULL;
	rxPoints *calibration = NULL;

	if (chain == 1) {
		selected_chain = &Rx_Chains.RxA;
		calibration   = &rxa_calibration;
	}
	else if (chain == 2) {
		selected_chain = &Rx_Chains.RxB;
		calibration   = &rxb_calibration;
	}
	else {
		return; // Invalid chain
	}

	// Set targetADC based on input source
	if (selected_chain->Input == 1) {
		selected_chain->targetADC = calibration->aux.dac;
	}
	else if (selected_chain->Input == 2) {
		selected_chain->targetADC = calibration->main.dac;
	}
}

void set_min_allowed_adc(uint8_t chain)
{
	CalibrationTable *table;
	struct Tx_status *SelectedChain;
	
	if(chain == 1)
	{
		SelectedChain = &tx.TxA;
		table = &txa_calibration_table;
	}
	else
	{
		SelectedChain = &tx.TxB;
		table = &txb_calibration_table;
	}
	
	/* Find lowest  allowed ADC reading before marking the chain as faulty*/
	uint16_t lowestADC = UINT16_MAX;
	for (size_t i = 0; i < NUM_POINTS; i++)
	{
		if ( table->adc[i] < lowestADC)
		{
			lowestADC = table->adc[i];
		}
	}
	SelectedChain->minAllowedADC = ((int32_t)lowestADC * (100 + PERCENTAGE_ADC_ALLOWED)) / 100;
	/* Find lowest allowed ADC over */
}

void set_tx_out_power(uint8_t chain) {

	uint16_t high_cal_pointer, low_cal_pointer;
	CalibrationTable *table;
	struct Tx_status *SelectedChain;
	
	/* initialise to avoid poor calibration error */
	// Find the bounding points
	high_cal_pointer = 0;
	low_cal_pointer = CAL_TABLE_SIZE - 1;
	
	if(chain == 1)
	{
		SelectedChain = &tx.TxA;
		table = &txa_calibration_table;
	}
	else
	{
		SelectedChain = &tx.TxB;
		table = &txb_calibration_table;
	}
	
	bool ascending = table->cbm[0] < table->cbm[CAL_TABLE_SIZE-1];

	/* get low and high values around the desired output power */
	for (uint8_t i = 0; i < CAL_TABLE_SIZE - 1; i++) {
		int16_t current_power = table->cbm[i];
		int16_t next_power = table->cbm[i + 1];
		
		if (ascending) {
			// For ascending table: current <= target <= next
			if (current_power <= SelectedChain->carrierPower &&
			SelectedChain->carrierPower <= next_power) {
				low_cal_pointer = i;      // Lower power value
				high_cal_pointer = i + 1; // Higher power value
				break;
			}
			} else {
			// For descending table: current >= target >= next
			if (current_power >= SelectedChain->carrierPower &&
			SelectedChain->carrierPower >= next_power) {
				high_cal_pointer = i;     // Higher power value
				low_cal_pointer = i + 1;  // Lower power value
				break;
			}
		}
	}
	
	//Need to solve y = ax + b
	/* START BY INTERPOLATE ADC FOR TARGET VALUE */
	float a_adc = ((float)table->adc[low_cal_pointer] - (float)table->adc[high_cal_pointer]) /
	((float)table->cbm[low_cal_pointer] - (float)table->cbm[high_cal_pointer]);

	float b_adc = (float)table->adc[high_cal_pointer] - a_adc * (float)table->cbm[high_cal_pointer];

	SelectedChain->targetADC = FLOAT_TO_INT(a_adc * (float)SelectedChain->carrierPower + b_adc);
	/* ADC INTERPOLATE OVER */
}

//
// Correction loop to be called every 5ms from timer interrupt
//
void correctionloopRx(uint8_t Chain)
{
	uint8_t active_DAC = 0xFF;
	int32_t nextVal;
	int32_t adc_error_threshold;
	int32_t error;
	int32_t step;
	
	struct Rx_PLLs  *SelectedChain;
	if (Chain == 1) {
		SelectedChain = &Rx_Chains.RxA;
		}else if (Chain == 2) {
		SelectedChain= &Rx_Chains.RxB;
		} else {
		return; // Invalid chain
	}

	
	if (!SelectedChain->agcEnable) return;
	
	// Threshold = the time constant (in ms) times 2 (50% instead of 63%)
	//             divided by Rx AGC timer
	adc_error_threshold = (int32_t)(SelectedChain->timeConst * 2) / AGC_PERIOD_MS;
	if (adc_error_threshold < 100) adc_error_threshold = 100;
	// Positive error means more power needed hence increase
	error = (int32_t)SelectedChain->targetADC - (int32_t)SelectedChain->currentADC;
	// To calculate step size : 3 times the error (for 3 DACs) divided by the threshold
	step = error * 3 / adc_error_threshold;
	if (step == 0 && error > 0) step = 1;
	else if (step == 0 && error < 0) step = -1;
	
	if (error < 0 ) {
		//UART_send_string("Correction dec\n");
		for (int i = 0; i < 3; i++) {
			if(SelectedChain->currentDACValue[i] <= DAC_MIN_RX) continue;
			nextVal = (int32_t)SelectedChain->currentDACValue[i] + step;
			SelectedChain->currentDACValue[i] = (nextVal < DAC_MIN_RX) ? DAC_MIN_RX : (uint16_t)nextVal;
			active_DAC = i;
			break;
		}
		} else if (error > 0) {
		//UART_send_string("Correction inc\n");
		//ADC too low so increase DACs from lowest priority (DAC1 to DAC3)
		for (int i = 2; i >= 0; i--) {
			if(SelectedChain->currentDACValue[i] >= DAC_MAX_RX) continue;
			nextVal = SelectedChain->currentDACValue[i] + step;
			SelectedChain->currentDACValue[i] = (nextVal > DAC_MAX_RX) ? DAC_MAX_RX : nextVal;
			active_DAC = i;
			break;
		}
	}
	else return; //No correction needed
	
	//char buf[21]; // Enough for -2,147,483,648 plus null terminator

	//if(active_DAC == 0xFF)
	//{
	//snprintf(buf, sizeof(buf), "Rx DAC at limit: %u \n",Chain);
	//UART_send_string(buf);
	//return; // in case all DACs at MAX or MIN then do nothing
	//}
	
	send_DAC_package(
	SelectedChain->DAC_PortCS[active_DAC],
	SelectedChain->DAC_CS[active_DAC],
	SelectedChain->dacChan[active_DAC],
	SelectedChain->currentDACValue[active_DAC]);
}




/**
* @brief Correction loop to be called every timer interrupt
*
* @param Chain : 1 for TxA, 2 for TxB
*/
void correctionloopTx(uint8_t Chain)
{
	uint8_t active_DAC = 0xFF;
	int32_t nextVal;
	int32_t tau;
	int32_t error;
	int32_t step;
	
	struct Tx_status *SelectedChain;
	if (Chain == 1) {
		SelectedChain = &tx.TxA;
		}else if (Chain == 2) {
		SelectedChain= &tx.TxB;
		} else {
		return; // Invalid chain
	}
	
	if (!SelectedChain->agcEnable || SelectedChain->FaultyChain == 1) return;
	
	if(SelectedChain->currentADC < SelectedChain->minAllowedADC && SelectedChain->isItOn == 1 && SelectedChain->uncalibratedFreq == 0)//Only trigger if current ADC too low, Chain is ON and A properly calibrated frequency
	{
		if(SelectedChain->failedADCattempts < AGC_MAX_FAILED_ATTEMPTS) SelectedChain->failedADCattempts++;
		
		else
		{
			SelectedChain->FaultyChain = 1;
			SelectedChain->agcEnable = 0;
			SelectedChain->currentDACValue[0] = SelectedChain->currentDACValue[1] = SelectedChain->currentDACValue[2] = 0;
			
			if(Chain==1)
			{
				setupDACTxA(); // Apply DAC values
				UART_send_string("Chain 1: ");
			}
			else if (Chain==2)
			{
				setupDACTxB();
				UART_send_string("Chain 2: ");
			}
			
			UART_send_string("Unexpected Log detector value detected for the last 333ms.");
			UART_send_string("AGC and Power will now be disabled to prevent damages\n\r");
			return;
		}
	}
	else if (SelectedChain->currentADC >= SelectedChain->minAllowedADC) SelectedChain->failedADCattempts = 0; //If ADC number legit then reset counter
	
	uint16_t minTau = 6*AGC_PERIOD_MS;
	tau = (int32_t)(SelectedChain->timeConst);
	if (tau < minTau) tau = minTau; // minTau should not cause oscillations (it sets the step = error)
	// Positive error means more power needed hence increase
	error = (int32_t)SelectedChain->targetADC - (int32_t)SelectedChain->currentADC;
	// Step size = agc_period LoopBW x 3 times the error (for 3 DACs) divided by the time constant
	step = error * (3*AGC_PERIOD_MS) / tau;
	if (step == 0 && error > 0) step = 1;
	else if (step == 0 && error < 0) step = -1;
	
	if (error < 0 ) {
		for (int i = 2; i >= 0; i--) {
			if(SelectedChain->currentDACValue[i] <= DAC_MIN_TX) continue;
			nextVal = SelectedChain->currentDACValue[i] + step;
			SelectedChain->currentDACValue[i] = (nextVal < DAC_MIN_TX) ? DAC_MIN_TX : nextVal;
			active_DAC = i;
			break;
		}
	}
	else if (error > 0) {
		for (int i = 0; i < 3; i++) {

			if(SelectedChain->currentDACValue[i] >= DAC_MAX_TX) continue;
			nextVal = SelectedChain->currentDACValue[i] + step;
			SelectedChain->currentDACValue[i] = (nextVal > DAC_MAX_TX) ? DAC_MAX_TX : nextVal;
			active_DAC = i;
			break;
		}
	}
	else return; //No correction needed
	
	send_DAC_package(
	SelectedChain->DAC_PortCS[active_DAC],
	SelectedChain->DAC_CS[active_DAC],
	SelectedChain->dacChan[active_DAC],
	SelectedChain->currentDACValue[active_DAC]);
}


void reset_tx_calibration(uint8_t chain)
{
	CalibrationTable *table;
	if(chain == 1)
	{
		table = &txa_calibration_table;
	}
	else
	{
		table = &txb_calibration_table;
	}

	for (uint8_t i = 0; i < sizeof(table->cbm) / sizeof(table->cbm[0]); i++) {
		table->cbm[i] = 0;
		table->adc[i] = 0;
	}
	write_calibration_table_to_eeprom(chain);
}