#include <interrupts_n_timers.h>

volatile uint32_t timer2_millis = 0;  // Millisecond counter
volatile uint8_t sec_counter = 0;
volatile uint8_t uart_index = 0;
volatile char uart_buffer[UART_BUFFER_SIZE];
volatile bool uart_line_ready = false;

volatile bool eth_reset_pressed = false;
volatile uint32_t eth_reset_pressed_time = 0;
volatile uint32_t eth_activity_tracking = 0;
volatile uint32_t timer_ticks = 0;  // Variable to track the timer ticks (in ms)
volatile uint32_t perif_health = 0;

void interruptHandler()
{
	// Power off unit is handled directly from interrupt.
	// Otherwise we cannot stop infinite boot bug
	// if (int_flags.powerToggle > 0)
	// {
	// 	int_flags.powerToggle = 0;
	// }
	if (eth_activity_tracking >= (TWO_SECONDS*15))  //Check if ethernet activity is inactive
	{ 		
		w5500_disconnect_then_abort(SOCKET_0,100); //Wait for 100ms for mercifull disconection else close TCP connection bruttaly
		eth_activity_tracking = 0; 
		int_flags.eth = 0;
		eth_status.F_fin = 1;
	}
	
	if (eth_status.reset >0)
	{
		CID_eth_factrst();
		init_w5500(SOCKET_0);
		eth_status.F_fin=0; // already zeroed in init_w5500.
	}
	
	if (eth_status.F_fin >0)
	{
		init_w5500(SOCKET_0);
		eth_status.F_fin=0; // already zeroed in init_w5500.
	}
	
	if(int_flags.eth>0)
	{
		w5500_handle_interrupt();  // Process W5500 events
		int_flags.eth--;
	}

	if (int_flags.ADC_Rx_Ready > 0)
	{
		//delay_us(5); //Give ADC some time to prepare averaging
		readRXPower(); //read ADC
		int_flags.ADC_Rx_Ready=0;
		correctionloopRx(1);
		correctionloopRx(2);
	}

	if (int_flags.ADC_Tx_Ready > 0)
	{
		//delay_us(5); //Give ADC some time to prepare averaging
		readTXPower(); //read ADC
		int_flags.ADC_Tx_Ready=0;
		correctionloopTx(1);
		correctionloopTx(2);
	}

	if(int_flags.agcTx > 0) //Request ADC Tx sample
	{
		int_flags.agcTx=0;
		
		if(int_flags.ADC_Tx_Ready==0) //Only if Previous were read and agcEnabled
		{
			requestNewSample(true);// request new sample Tx ADC
		}
	}

	if(int_flags.agcRx > 0) //Request ADC Rx sample
	{
		int_flags.agcRx = 0;
		if(int_flags.ADC_Rx_Ready==0) //Only if Previous were read and agcEnabled
		{
			requestNewSample(false); // request new sample Rx ADC
		}
	}

	if (int_flags.uart > 0 && uart_line_ready)
	{
		for (uint8_t i = 0; uart_buffer[i] != '\0'; i++) {
			send_uart(uart_buffer[i]);
		}
		send_uart('\n');  // Echo newline
		uart_index = 0;
		int_flags.uart = 0;
		uart_line_ready = false;
		UART_execute_cmd();
	}

	if (int_flags.health > 0)
	{
		static bool LED = false;
		perif_health = CID_health_check();
		uint8_t color = 1; // Default color is red
		uint32_t masked_health = perif_health & 0x7FFFF;  // Keep only bits 0 18. PSU bits not relevant
		
		if(LED)
		{
			led(ledSelection.HEALTH,0);
			LED = false;
		}
		else
		{
			if(masked_health == 0x7FFFF) color = 2; //If healthcheck OK give color green
			else if(masked_health == 0x7EFFF) color = 3; //If only 10MHz is missing give color orange
			led(ledSelection.HEALTH,color);
			LED = true;
		}
		int_flags.health = 0;

		wdt_reset();
	}
}


void readUartBuff()
{
	int_flags.uart +=1;
	
	char c = UDR0;  // Read received character

	if (!uart_line_ready) {
		if (c == '\n' || c == '\r') {
			uart_buffer[uart_index] = '\0';  // Null-terminate the string
			uart_line_ready = true;         // Signal complete line
			int_flags.uart += 1;
			} else {
			if (uart_index < UART_BUFFER_SIZE - 1) {
				uart_buffer[uart_index++] = c;
				} else {
				uart_index = 0;  // Overflow: reset buffer
			}
		}
	}
}


void powerHandling()
{
	static uint32_t manual_delay = 0; //required since timers wont run while waiting for for unit to power on
	static bool allow_powerOn = false;
	static char local_uart_buf[64];
	static uint8_t local_uart_index = 0;

	if (timer_ticks >= TWO_SECONDS) {
		timer_ticks = 0; // Reset timer
		manual_delay = 0;
		
		if (PINL & (1 << REST.PSU_CONTROL.PIN)) {
			
			w5500_disconnect_then_abort(SOCKET_0,500); //Wait for 500ms for mercifull disconection else close TCP connection bruttaly
			*REST.PSU_CONTROL.PORT &= ~(1 << REST.PSU_CONTROL.PIN);
			configure_ports_low(); // Avoid leakage 5V
			
			local_uart_buf[0] = '\0';// Clear UART first char
			local_uart_index = 0;
			UCSR0B &= ~(1 << RXCIE0);  // Disable USART RX interrupt to receive characters here
			
			while (1) {
				wdt_reset();  // Kick the dog
				
				manual_delay++;
				
				if (UCSR0A & (1 << RXC0)) {
					uint8_t received_byte = UDR0;
					
					if (local_uart_index < UART_BUFFER_SIZE - 1) { //Check UART command for reboot
						received_byte = toupper((unsigned char)received_byte);  // Convert to uppercase
						local_uart_buf[local_uart_index++] = received_byte;
						local_uart_buf[local_uart_index] = '\0';  // Null-terminate
					}
				}

				
				if(manual_delay >= MANUAL_DELAY_2SEC) allow_powerOn = true;
				
				if ((!( *REST.POWER_SWITCH_SENSE.PORT & (1 << REST.POWER_SWITCH_SENSE.PIN)) && allow_powerOn) ||
				strstr(local_uart_buf, REBOOT_CMD) != NULL) //REBOOT or reboot
				{
					wdt_enable(WDTO_15MS); // Trigger reset
					while (1); // Wait for watchdog to fire
				}
			}
		}
	}
}

void RebootHandling()
{
	static uint32_t manual_delay = 0; //required since timers wont run while waiting for for unit to power on
	static bool allow_powerOn = false;

	if (timer_ticks >= TWO_SECONDS) {
		timer_ticks = 0; // Reset timer
		manual_delay = 0;
		
		if (PINL & (1 << REST.PSU_CONTROL.PIN)) {
			
			w5500_disconnect_then_abort(SOCKET_0,500); //Wait for 500ms for mercifull disconection else close TCP connection bruttaly
			*REST.PSU_CONTROL.PORT &= ~(1 << REST.PSU_CONTROL.PIN);
			configure_ports_low(); // Avoid leakage 5V
			
			
			while (1) {
				wdt_reset();  // Kick the dog
				
				manual_delay++;
				
				if(manual_delay >= MANUAL_DELAY_2SEC)
				{
					while (1);
				}
			}
		}
	}
}

void setup_ext_interrupt() {
	// === Set INT pins as inputs ===
	DDRE &= ~(1 << DDE5);  // INT5 (PE5) - Power Sense
	DDRE &= ~(1 << DDE7);  // INT7 (PE7) - W5500
	DDRD &= ~(1 << DDD2);  // INT2 (PD2) - ADC EOC
	DDRD &= ~(1 << DDD3);  // INT3 (PD3) - ADC EOC

	// === Configure INT5 (PE5) for falling edge ===
	EICRB &= ~(1 << ISC50);
	EICRB |=  (1 << ISC51);

	// === Configure INT7 (PE7) for falling edge ===
	EICRB &= ~(1 << ISC70);
	EICRB |=  (1 << ISC71);

	// === Configure INT2 (PD2) for falling edge ===
	EICRA &= ~(1 << ISC20);
	EICRA |=  (1 << ISC21);

	// === Configure INT3 (PD3) for falling edge ===
	EICRA &= ~(1 << ISC30);
	EICRA |=  (1 << ISC31);


	// === Enable INTx interrupts ===
	EIMSK = (1 << INT5) | (1 << INT7) | (1 << INT2) | (1 << INT3); // INT2 EOC ADC Tx ** INT3 EOC ADC Rx ** INT7 ETHERNET Interrupt ** INT5 Power button
	
	// === PCINT setup for PK0 (PCINT16) === Interrupt for ethernet switch
	PCICR  |= (1 << PCIE2);      // Enable Pin Change Interrupt Control for PCINT[23:16] => PORTK
	PCMSK2 |= (1 << PCINT16);    // Enable interrupt only on PCINT16 (PK0)
}

void setup_timer4()
{
	TCCR4A = 0;                  // Normal operation
	TCCR4B = 0;                  // Clear to configure

	TCCR4B |= (1 << WGM42);      // CTC mode (WGM42 = 1, WGM43 = 0)
	TCCR4B |= (1 << CS42);       // Pre-scaler = 256

	OCR4A = 46863;            // 850ms at 16 MHz with /256 pre-scaler

	TCNT4 = 0;                   // Reset timer counter
	TIMSK4 |= (1 << OCIE4A);     // Enable compare match interrupt on OCR4A
}

void setup_timer3() {
	TCCR3A = 0;                          // Normal port operation
	TCCR3B = (1 << WGM32) | (1 << CS32) | (1 << CS30);  // CTC mode, Prescaler 1024
	
	// Calculate OCR3A using integer math to avoid floating point
	OCR3A = (uint16_t)(((uint32_t)AGC_PERIOD_MS * 15625UL + 500) / 1000) - 1;
	
	TCNT3 = 0;                          // Reset counter
	
	char buffer[64];
	sprintf(buffer, "AGC period: %u ms | OCR3A: %u\r\n", AGC_PERIOD_MS, OCR3A);
	UART_send_string(buffer);
	
	TIMSK3 |= (1 << OCIE3A);            // Enable Timer3 Compare Match A Interrupt
}


void delay_ms(uint16_t del_ms)
{
	for(uint16_t i=0; i<del_ms; i++) {
		_delay_ms(1);
	}
}

void delay_us(uint16_t del_us)
{
	for(uint16_t i=0; i<del_us; i++) {
		_delay_us(1);
	}
}


void setup_timer1() {
	TCCR1A = 0;                  // Normal operation
	TCCR1B = 0;                  // Clear to configure

	// Set Timer1 to CTC (Clear Timer on Compare Match) mode
	TCCR1B |= (1 << WGM12);  // Mode 4 (CTC mode)
	
	// Set prescaler to 256
	TCCR1B |= (1 << CS12);   // Clock prescaler: 256
	
	// Set OCR1A to generate an interrupt every 1 ms (16 MHz / 256 / 1000 = 62.5)
	//OCR1A = 62;  // (16,000,000 / 256) / 1000 - 1 = 62
	OCR1A = 468;
	
	// Enable Timer1 compare interrupt
	TIMSK1 |= (1 << OCIE1A);  // Enable compare match interrupt for Timer1
}


ISR(TIMER1_COMPA_vect) { //Used to avoid switch bouncing
	timer_ticks++;
	eth_activity_tracking++;
	int_flags.timer++;
}

ISR(TIMER3_COMPA_vect) { //ASK ADC Tx to start new samples
	int_flags.agcTx++;
	int_flags.agcRx++;
}

ISR(TIMER4_COMPA_vect)
{
	int_flags.health++;
}

ISR(INT2_vect) { // New sample is availabe EOC TX is pulling this ISR
	int_flags.ADC_Tx_Ready++;
}

ISR(INT3_vect) { // New sample is availabe EOC RX is pulling this ISR
	int_flags.ADC_Rx_Ready++;
}

ISR(INT5_vect) {
	// Power off unit is handled directly from interrupt.
	// Otherwise we cannot stop infinite boot bug
	//int_flags.powerToggle = 1;
	powerHandling();
}

ISR(INT7_vect)
{
	int_flags.eth++;
	eth_activity_tracking = 0;
}

/**
* @brief PCINT2 interrupt handler.
*        Ethernet reset pin is pressed.
*        Only do something when pressed long enough.
*/
ISR(PCINT2_vect)
{
	// Handle PK0 going LOW
	if (!(*REST.ETH_RESET.PORT & (1 << REST.ETH_RESET.PIN))) {
		if(!eth_reset_pressed)
		{
			eth_reset_pressed = true;
			eth_reset_pressed_time = timer_ticks;
		}
		else
		{
			eth_reset_pressed = false;
		}
	}
}


/**
* @brief USART0_RX interrupt handler.
*        Read the incoming UART command.
*/
ISR(USART0_RX_vect) {
	readUartBuff();
}


/**
* @brief INT0 interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(INT0_vect)
{
	;
}

/**
* @brief INT1 interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(INT1_vect)
{
	;
}


/**
* @brief INT4 interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(INT4_vect)
{
	;
}

/**
* @brief INT6 interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(INT6_vect)
{
	;
}

/**
* @brief TIMER2_COMPA interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER2_COMPA_vect)
{
	;
}

/**
* @brief TIMER2_COMPB interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER2_COMPB_vect)
{
	;
}

/**
* @brief TIMER2_OVF interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER2_OVF_vect)
{
	;
}

/**
* @brief TIMER1_CAPT interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER1_CAPT_vect)
{
	;
}

/**
* @brief TIMER1_COMPB interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER1_COMPB_vect)
{
	;
}


/**
* @brief TIMER0_COMPA interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER0_COMPA_vect)
{
	;
}

/**
* @brief TIMER0_COMPB interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER0_COMPB_vect)
{
	;
}

/**
* @brief TIMER0_OVF interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER0_OVF_vect)
{
	;
}

/**
* @brief TIMER1_OVF (Overflow Vector Flag) interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER1_OVF_vect)
{
	;
}


/**
* @brief SPI_STC interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(SPI_STC_vect)
{
	;
}

/**
* @brief USART0_UDRE interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(USART0_UDRE_vect)
{
	;
}

/**
* @brief USART0_TX interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(USART0_TX_vect)
{
	;
}

/**
* @brief ADC interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(ADC_vect)
{
	;
}

/**
* @brief EE_READY interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(EE_READY_vect)
{
	;
}

/**
* @brief ANALOG_COMP interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(ANALOG_COMP_vect)
{
	;
}

/**
* @brief TIMER1_COMPC interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER1_COMPC_vect)
{
	;
}

/**
* @brief TIMER3_CAPT interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER3_CAPT_vect)
{
	;
}

/**
* @brief TIMER3_COMPB interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER3_COMPB_vect)
{
	;
}

/**
* @brief TIMER3_COMPC interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER3_COMPC_vect)
{
	;
}

/**
* @brief TIMER3_OVF interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TIMER3_OVF_vect)
{
	;
}

/**
* @brief USART1_UDRE interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(USART1_UDRE_vect)
{
	;
}

/**
* @brief USART1_TX interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(USART1_TX_vect)
{
	;
}

/**
* @brief TWI interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(TWI_vect)
{
	;
}

/**
* @brief SPM_READY interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(SPM_READY_vect)
{
	;
}

/**
* @brief USART0_RX interrupt handler.
*
* Atmel-41086A states: We recommend initializing all the interrupt vectors,
*                      even if they are not used.
*/
ISR(USART1_RX_vect)
{
	;
}





