#include <main.h>

//extern bool unitIsOff;


void lockAllChains()
{
	/* #######  Setting RxA Freq ######## */
	UART_send_string("RXA...");
	led(ledSelection.RXA_MAIN,1);
	change_Rx_Frequency(START_RX_FREQ,1);
	led(ledSelection.RXA_MAIN,2);
	UART_send_string("OK\n");
	wdt_reset();  // Kick the dog (prevents reset)
	delay_ms(50);
	
	/* #######  Setting TxA Freq ######## */
	UART_send_string("TXA...");
	led(ledSelection.TXA_MAIN,1);
	change_Tx_Frequency(START_TX_FREQ,1);
	led(ledSelection.TXA_MAIN,2);
	UART_send_string("OK\n");
	wdt_reset();  // Kick the dog (prevents reset)
	delay_ms(50);
	
	/* #######  Setting RxB Freq ######## */
	UART_send_string("RXB...");
	led(ledSelection.RXB_MAIN,1);
	change_Rx_Frequency(START_RX_FREQ,2);
	led(ledSelection.RXB_MAIN,2);
	UART_send_string("OK\n");
	wdt_reset();  // Kick the dog (prevents reset)
	delay_ms(50);
	
	/* #######  Setting TxB Freq ######## */
	UART_send_string("TXB...");
	led(ledSelection.TXB_MAIN,1);
	change_Tx_Frequency(START_TX_FREQ,2);
	led(ledSelection.TXB_MAIN,2);
	UART_send_string("OK\n");
	wdt_reset();  // Kick the dog (prevents reset)
	delay_ms(250);

	
	led(ledSelection.ALL,0);
}

int main(void) {
	MCUSR &= ~(1 << WDRF); // Clear the watchdog reset flag
	wdt_enable(WDTO_2S); //make sure power on delays are reasonable before enable

	setup_uart();
	UART_send_string("Booting... please wait\n");
	UART_send_string("Initializing GPIOs...\n");
	setup_GIO_directions();
	setup_Output_state();

	UART_send_string("Initializing timers...\n");
	setup_timer1();
	setup_timer3();
	setup_timer4();
	
	UART_send_string("Initializing interrupts...\n");
	setup_ext_interrupt();
	
	UART_send_string("Enabling interrupts...\n");
	sei();
	
	UART_send_string("Powering on unit...\n");
	wdt_reset();  // Pet the dog (prevents reset)
	power_On_routine();

	wdt_reset();  // Pet the dog (prevents reset)
	delay_ms(750); //Waiting for all PCBs to power on (500ms max delay on power distr board)
	
	setup_spi();
	
	UART_send_string("Initializing software and components...\n");
	wdt_reset();  // Pet the dog (prevents reset)
	init_pll();//setup PLLs and Prepare645 must be before SetupPortExp
	init_tx_chains();
	init_rx_chains();
	wakeupDACs();
	initADCs();
	TC72_init();
	setupPortEx();
	init_EEPROMs();
	
	UART_send_string("Locking chains...\n");
	wdt_reset();  // Pet the dog (prevents reset)
	lockAllChains();	//Locks all PLLs based on default frequencies
	
	/* init global interupt variables */
	int_flags.powerToggle = 0;
	int_flags.eth = 0;
	int_flags.uart = 0;
	int_flags.timer = 0;
	int_flags.ADC_Rx_Ready = 0;
	int_flags.agcRx = 0;
	int_flags.agcTx = 0;
	int_flags.ADC_Tx_Ready = 0;
	
	UART_send_string("Initializing Ethernet controller...\n");
	wdt_reset();  // Pet the dog (prevents reset)
	init_w5500(SOCKET_0);
	UART_send_string("Initialization successful\nVIBB is Online!\n\n"); //Output channel one
	
	/* =========TEMP FOR TEST======== */	
//	tx.TxB.agcEnable = tx.TxB.isItOn = 	tx.TxA.agcEnable = tx.TxA.isItOn = Rx_Chains.RxA.agcEnable = Rx_Chains.RxA.isItOn = Rx_Chains.RxB.agcEnable = Rx_Chains.RxB.isItOn = 1;
	/*======================*/		
	
	
	while (1) {
		// Watchdog petting done when health check interrupts
		//wdt_reset();  // Kick the dog (prevents reset) 
		interruptHandler();
		
		if (eth_reset_pressed) {
			if ((timer_ticks - eth_reset_pressed_time) >= FIVE_SECONDS) {
				eth_reset_pressed = false; // Prevent repeated triggers
				eth_status.reset = 1;
			}
		}
	}
}