/* This is a script to be run on C compiler in order to replicate interpolation of MCU code*/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define CAL_TABLE_SIZE 11
#define NUM_POINTS 11
#define Delay_between 3 //Tested with 8MHz SPI clock and minimum required is 2ms delay (not μs)
#define FLOAT_TO_INT(x) ((x) >= 0 ? (int)((x) + 0.5) : (int)((x)-0.5))  //Convert float to int and round up/down to closest int
#define StartTxFreq  84150000LL   // 841.5 MHz * 100000


typedef struct {
	volatile int16_t cbm[NUM_POINTS];
	volatile uint16_t adc[NUM_POINTS];
} CalibrationTable;

struct Tx_status{
	long long FreqMHz;						// Frequency in Mhz*SCALE_FACTOR
	volatile uint8_t Output;             // 0 = Loopback, 1 = Main_Input
	volatile uint8_t isItOn;				// 1=ON 0=OFF
	volatile uint8_t health;	          // 1 = BAD 2 = GOOD
	volatile uint16_t targetADC;		 //AGC ADC
	volatile int16_t carrierPower;
};

struct Tx_chains {
	struct Tx_status TxA;
	struct Tx_status TxB;
};

struct Tx_chains tx = {
	.TxA = {.FreqMHz=StartTxFreq, .Output = 1, .isItOn = 0, .health = 2,.targetADC = 0x00, .carrierPower = -102 },
	.TxB = {.FreqMHz=StartTxFreq, .Output = 1, .isItOn = 0, .health = 2,.targetADC = 0x00, .carrierPower = -202 }
};

CalibrationTable txa_calibration_table = {
	.cbm = {100, 50, 0, -50, -100, -200, -300, -400, -500, -600, -700},
	.adc = {100,500,1000,1800,2000,2500,3200,3500,3700,4000,4090}
};

CalibrationTable txb_calibration_table = {
	.cbm = {100, 50, 0, -50, -100, -200, -300, -400, -500, -600, -700},
	.adc = {4090,4000,3700,3500,3200,2500,2000,1800,1000,500,100}
};


void set_tx_out_power(uint8_t chain) { 

	uint16_t high_cal_pointer, low_cal_pointer;	
	CalibrationTable *table;
	struct Tx_status *SelectedChain;
	
	/* initialise to avoid poor calibration error */
	high_cal_pointer = 0;
	low_cal_pointer = 1;	
	
	if(chain == 1)
	{
		SelectedChain = &tx.TxA;
		table = &txa_calibration_table; 
	}
	else if (chain ==2)
	{
		SelectedChain = &tx.TxB;
		table = &txb_calibration_table;
	}

	/* get low and high values around the desired output power */
	for (uint8_t i = 0; i < CAL_TABLE_SIZE; i++) {
		int16_t power = table->cbm[i];  //Div by one to allow casting
		if (power >= SelectedChain->carrierPower) {
			high_cal_pointer = i;
			} else {
			low_cal_pointer = i;
			break;
		}
	}
	
	//Need to solve y = ax + b
	/* START BY INTERPOLATE ADC FOR TARGET VALUE */
	float a_adc = (float)(table->cbm[low_cal_pointer] - table->cbm[high_cal_pointer]) / (table->adc[low_cal_pointer] - table->adc[high_cal_pointer]);
	float b_adc = (float)table->cbm[high_cal_pointer] - a_adc * table->adc[high_cal_pointer];

	uint16_t x_adc = FLOAT_TO_INT((float)(SelectedChain->carrierPower - b_adc) / a_adc);
	
	SelectedChain->targetADC = x_adc;
	printf("Integer: %d\n", SelectedChain->targetADC);       // %d for integers
	/* ADC INTERPOLATE OVER */
}
int main() {
    set_tx_out_power(1);
    set_tx_out_power(2);
    return 0;
}