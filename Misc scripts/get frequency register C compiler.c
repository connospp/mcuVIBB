/* This is a script to be run on C compiler in order to find memory registers and index of each frequency
To use change "long long freq" in main (Scale factor to not be touched)
and Depending if Tx or Rx Change int step under "uint32_t get_calibration_address(uint8_t chain, long long freq_mhz_scaled)" function
STEP_FREQ_MHZ_TX or STEP_FREQ_MHZ_TX*/

#include <stdint.h>
#include <stdio.h>
#define SCALE_FACTOR 100000
#define START_FREQ_MHZ     60
//#define STEP_FREQ_MHZ      33
#define TABLE_SIZE_BYTES   352
#define NUM_CAL_POINTS 186
#define STEP_FREQ_MHZ_TX      22 //Start 60Mhz with 22MHz step, gives a range 60MH-4110MHz
#define STEP_FREQ_MHZ_RX      33 //Start 60Mhz with 33MHz step, gives a range 60MH-6135MHz

uint32_t get_calibration_address(uint8_t chain, long long freq_mhz_scaled)
{
    int step = STEP_FREQ_MHZ_TX;
    uint32_t freq_mhz = freq_mhz_scaled / SCALE_FACTOR;
    
    if (freq_mhz < START_FREQ_MHZ)
        freq_mhz = START_FREQ_MHZ;
    // Compute index to closest calibration slot
    uint16_t index = (freq_mhz - START_FREQ_MHZ + step / 2) / step;
    if (index >= NUM_CAL_POINTS) index = NUM_CAL_POINTS - 1;
    
     printf("Index: %lu\n", (unsigned long)index);
    // Interleaved layout:
    // Chain 1: even slots, Chain 2: odd slots
    if (chain == 1)
        return (uint32_t)(index * 2) * TABLE_SIZE_BYTES;
    else if (chain == 2)
        return (uint32_t)(index * 2 + 1) * TABLE_SIZE_BYTES;
    else
        return 0xFFFFFFFF;  // Invalid chain
}
void print_cal_table_registers(long long freq_mhz_scaled)
{ 
    uint32_t freq_mhz = freq_mhz_scaled / SCALE_FACTOR;
    
    for (uint8_t chain = 1; chain <= 2; ++chain)
    {
        uint32_t start = get_calibration_address(chain, freq_mhz_scaled);
        uint32_t end = start + TABLE_SIZE_BYTES - 1;
       printf("Chain %u | Freq %4u MHz | Table Start Addr: %6lu | Last Register: %6lu\n",
               chain, freq_mhz, (unsigned long)start, (unsigned long)end);
    }
}
int  main()
{
    long long freq = 2029*SCALE_FACTOR;
    uint32_t addr1 = get_calibration_address(1, freq);
    uint32_t addr2 = get_calibration_address(2, freq);
    print_cal_table_registers(freq);
    printf("Chain 1 Address: %lu\n", (unsigned long)addr1);
    printf("Chain 2 Address: %lu\n", (unsigned long)addr2);
    
    return 0;
    
}

