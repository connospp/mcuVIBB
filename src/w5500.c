/*
* w5500.c
*
* @brief Functions related with the Ethernet controller W5500 are made in this file.
* https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/W5500_datasheet_v1.0.2_1.pdf
* https://wizwiki.net/wiki/doku.php/products:w5500:allpages#plugin_include__products__w5500__application__tcp_function
*
* Company: Celestia Antwerp B.V.
* Project: IBBE
* Created: 10-3-2021 15:20:35
* Author : ARNE DE BRABANTER
* Copyright (c) Celestia Antwerp B.V.
*
*/

// chip WIZnet W5500 DNT208 2111
#include "SPI.h"
#include "common.h"
#include "commands.h"
#include "w5500.h"
#include <avr/eeprom.h>
/*
FLOW ETHERNET

2.0 Common Register Block (0x00).
2.1 Write MR = 0x80 (Software Reset).
2.2 Write GAR (Gateway).
2.3 Write SUBR (Subnet Mask).
2.4 Write SHAR (Source Hardware Address).
2.5 Write SIPR (Source IP Address).
2.6 Write IMR = 0x01 (Enable Socket 0 Interrupts).

3.0 Socket 0 Register Block (0x01).
3.1 Write S0_PORT (Port Number).
3.2 Write S0_MR = 0x01 (Set TCP Protocol).
3.3 Write S0_CR = 0x01 (OPEN).
3.4 Write S0_CR = 0x02 (LISTEN).
3.5 Wait for an Interrupt when a
ion is Established.
3.6 Write to Common Block SIR to Clear the Interrupt.
3.7 Wait for RECV Interrupt (Data Received).
3.7.1 Read S0_RX_RSR to Get the RX Buffer Data Size.
3.7.2 Read S0_RX_RD to Get RX Data Start Address.
3.7.3 Read RX Data from S0_RX_BUF (Block 0x02).
3.7.4 Write Data Size to S0_RX_RD to Update.
3.7.5 Write S0_CR = 0x40 (RECV) to Notify W5500.
3.8 When Data to Send is Available.
3.8.1 Read S0_TX_WR to Get RX Data Start Address.
3.8.2 Write TX Data from the Start Address of S0_TX_BUF (Block 0x01).
3.8.3 Write Data Size to S0_TX_WR to the Increased Value.
3.8.4 Write S0_CR = 0x20 (SEND) to Transmit the Data.
*/

/*
* @brief default parameters for establishing an Ethernet connection.
* Checks if parameters are written into the eeprom.
* If not, the default parameters will be used to initialize the ethernet controller
* If yes, parameters are written into the eeprom. The eeprom parameters will be used by the ethernet controller.
* is called in function init_w5500()
*/

// local variables, set with defaults at startup (take care about endianess..)
uint8_t macAddress[6] = {60, 50, 40, 30, 20, 10}; // mac-address
uint8_t ipAddress[4]  = { 10,  50, 168, 192};         // ip-address
uint8_t subAddress[4] = {  0, 255, 255, 255};         // subnet mask-address
uint8_t gateAddress[4]= {  1,  50, 168, 192};         // default gateway-address
uint8_t portNumber[2] = {0x88, 0x13};                 // port number: 5000 = 0x1388

// keep track of ethernet status
struct eth_Flags eth_status;

// INTERRUPTS
#define BIT(x) (1<<x)
#define W5500_IRQ_CON       BIT(0)
#define W5500_IRQ_DISCON    BIT(1)
#define W5500_IRQ_RECV      BIT(2)
#define W5500_IRQ_TIMEOUT   BIT(3)
#define W5500_IRQ_SEND_OK   BIT(4)


// just some state and interrupt tracing...
//#define NET_TRACE_EVENTS // uncomment this to enable tracking what happens during network traffic...
#ifdef NET_TRACE_EVENTS
#   define NETSTATE_MSK    (0x3F)
uint8_t net_statechanges[NETSTATE_MSK+1]={0};
uint8_t net_state_idx_next = 0x0;

static void log_netstate(uint8_t state)
{
	net_statechanges[net_state_idx_next & NETSTATE_MSK] = state;
	net_state_idx_next++;
}
#else
// disable logging
#   define log_netstate(p) do {} while(0)
#endif

void eth_param_default()
{
	// defaults are set at startup of the code... see above.
	uint8_t eeprom_buf[12];
	eeprom_read_block((void*)&eeprom_buf, (const void*) ip_p, 12);
	if (eeprom_buf[0] == 0xFF && eeprom_buf[1] == 0xFF && eeprom_buf[2] == 0xFF && eeprom_buf[3] == 0xFF)
	{
		//default IP, subnet mask and default gateway
		uint8_t ip_param[] = {ipAddress[3], ipAddress[2], ipAddress[1], ipAddress[0], subAddress[3], subAddress[2],
		subAddress[1], subAddress[0], gateAddress[3], gateAddress[2], gateAddress[1], gateAddress[0]};
		eeprom_update_block((const void*)ip_param, (void*)ip_p, 12); //ip_p is pointer to start location of eeprom
	}
	else
	{
		//initialize gateway address from EEPROM
		ipAddress[3] = eeprom_buf[0];
		ipAddress[2] = eeprom_buf[1];
		ipAddress[1] = eeprom_buf[2];
		ipAddress[0] = eeprom_buf[3];
		//initialize subnet mask from EEPROM
		subAddress[3] = eeprom_buf[4];
		subAddress[2] = eeprom_buf[5];
		subAddress[1] = eeprom_buf[6];
		subAddress[0] = eeprom_buf[7];
		//initialize IP address from EEPROM
		gateAddress[3] = eeprom_buf[8];
		gateAddress[2] = eeprom_buf[9];
		gateAddress[1] = eeprom_buf[10];
		gateAddress[0] = eeprom_buf[11];
	}

	eeprom_read_block((void*)&eeprom_buf, (const void*) tcp_p, 2);
	if(eeprom_buf[0] == 0xFF && eeprom_buf[1] == 0xFF)
	{
		uint8_t tcp_param[] = {portNumber[1], portNumber[0]};
		eeprom_update_block((const void*)tcp_param, (void*)tcp_p, 2);
	}
	else
	{
		// initialize port number from EEPROM
		portNumber[1] = eeprom_buf[0];
		portNumber[0] = eeprom_buf[1];
	}

	// Check if port is negative and set to 5000 if so
	uint16_t port = (portNumber[1] << 8) | portNumber[0];
	if((int16_t)port < 0)
	{
		port = 5000;
		portNumber[1] = (port >> 8) & 0xFF;
		portNumber[0] = port & 0xFF;
	}

	eeprom_read_block((void*)&eeprom_buf, (const void*) mac_p, 6);
	if(eeprom_buf[0] == 0xFF && eeprom_buf[1] == 0xFF && eeprom_buf[3] == 0xFF && eeprom_buf[5] == 0xFF)
	{
		uint8_t mac_param[] = {macAddress[5],macAddress[4],macAddress[3],macAddress[2],macAddress[1],macAddress[0]};
		eeprom_update_block((const void*)mac_param, (void*)mac_p, 6); //ip_p is pointer to start location of eeprom
	}
	else
	{
		//initialize MAC/HW address from EEPROM
		macAddress[5]= eeprom_buf[0];
		macAddress[4]= eeprom_buf[1];
		macAddress[3]= eeprom_buf[2];
		macAddress[2]= eeprom_buf[3];
		macAddress[1]= eeprom_buf[4];
		macAddress[0]= eeprom_buf[5];
	}

	//initialize flags
	eth_status.F_connected = 0;
	eth_status.F_fin = 0;
}


/*
* @brief Sends by SPI the correct amount of frames to write or read the W5500 chip in the preferred register
*
* @param reg: address of the preferred register
* @param writeread: decides if it is a read or write operation in the preferred register: _W5500_SPI_READ_ or _W5500_SPI_WRITE_
* @param *data: the array with data (pass by reference)
* @param length: the amount of bytes in the array of the data
*/
void w5500_spi(uint16_t reg, uint8_t writeread, uint8_t *data, uint8_t length)
{
	*ETH_GPIOs.W5500.CS_PORT &= ~(1 << ETH_GPIOs.W5500.CS_PIN);  // Set PH7 (CS) Low
	
	delay_ms(1);
	// address-phase (16-bit)
	writeread_spi(0x00); //SPI MSB address offset
	writeread_spi(((reg>>8) & 0xFF)); //SPI LSB address offset

	// control-phase (8-bit): MSB + write/read + variable data length
	writeread_spi(((reg>>0) & 0xFF) | writeread | _W5500_SPI_VDM_OP_);

	// data-phase
	// if write flag set, we do not overwrite original configured data with what is received.
	// SPI is always bidirectional on one line...
	if (writeread & _W5500_SPI_WRITE_)
	{
		for (int i = length ; i > 0 ; i--)
		{
			writeread_spi(data[i-1]); // write: do not overwrite source!
		}
	}
	else
	{
		for (int i = length ; i > 0 ; i--)
		{
			data[i-1] = writeread_spi(0); // read: data is now destination buffer, data send does not matter!
		}
	}

	*ETH_GPIOs.W5500.CS_PORT |= (1 << ETH_GPIOs.W5500.CS_PIN);  // Set PH7 (CS) high
	delay_ms(1);
}

void w5500_spi_write(uint16_t reg, uint8_t *data, uint8_t length)
{
	short_msg_t regi;

	regi.shrt = reg | _W5500_SPI_WRITE_;

	*ETH_GPIOs.W5500.CS_PORT &= ~(1 << ETH_GPIOs.W5500.CS_PIN); // Set Ethernet CS LOW

	// address-phase (16-bit)
	writeread_spi(0x00); //SPI MSB address offset
	short_wr_spi(&regi);

	// data-phase
	for (int i = length ; i > 0 ; i--)
	{
		writeread_spi(data[i-1]); // write: do not overwrite source!
	}

	*ETH_GPIOs.W5500.CS_PORT |= (1 << ETH_GPIOs.W5500.CS_PIN); // Set Ethernet CS HIGH
}


void w5500_spi_read(uint16_t reg, uint8_t *data, uint8_t length)
{
	short_msg_t regi;

	regi.shrt = reg;

	*ETH_GPIOs.W5500.CS_PORT &= ~(1 << ETH_GPIOs.W5500.CS_PIN); // Set Ethernet CS LOW
	
	// address-phase (16-bit)
	writeread_spi(0x00); //SPI MSB address offset
	short_wr_spi(&regi);

	// data-phase
	for (int i = length ; i > 0 ; i--)
	{
		data[i-1] = writeread_spi(0); // read: data is now destination buffer, data send does not matter!
	}

	*ETH_GPIOs.W5500.CS_PORT |= (1 << ETH_GPIOs.W5500.CS_PIN); // Set Ethernet CS HIGH
}


/*
* @brief Sends by SPI 1 byte to write or read the W5500 chip in the preferred register
*
* @param reg: address of the preferred register
* @param writeread: decides if it is a read or write operation in the preferred register: _W5500_SPI_READ_ or _W5500_SPI_WRITE_
* @param data: the byte of data (pass by value)
*/
void w5500_spi_write_one(uint16_t reg, uint8_t data)
{
	w5500_spi(reg, _W5500_SPI_WRITE_, &data, 1);
}

uint8_t w5500_spi_read_one(uint16_t reg)
{
	uint8_t data = 0;
	w5500_spi(reg, _W5500_SPI_READ_, &data, 1);
	return data;
}

/*
* @brief Initializes the common registers (CREG) and socket registers (SREG) with default parameters for socket
*
* @param socket:
*/
void init_w5500(uint8_t socket)
{
	uint8_t readbuffer[]= {0x00,0x00};

	eth_status.F_connected = 0;
	eth_status.F_fin = 0;
	eth_status.reset = 0;
	eth_param_default();

	
	/******************** Wait READY flag W5500 ***********************/
	*ETH_GPIOs.W5500.RESET_PORT &= ~(1 << ETH_GPIOs.W5500.RESET_PIN);
	delay_ms(1);
	*ETH_GPIOs.W5500.RESET_PORT |= (1 << ETH_GPIOs.W5500.RESET_PIN);
	delay_ms(150);
	
	while (!(*ETH_GPIOs.W5500.RDY_PORT & (1 << ETH_GPIOs.W5500.RDY_PIN))) { // Wait until chip ready to continue
		// Wait here until PH0 goes high
	}
	// IP initialization
	
	/******************** SW RESET W5500 ***********************/
	//w5500_spi_write_one(MR, MR_RST); //request chip reset and wait till done
	//wait_ms(250);
	//
	while (w5500_spi_read_one(MR) != 0) {} // wait till reset.

	/***************** w5500 OPERATION MODE ********************/
	w5500_spi_write_one(PHYCFGR, 0xF8); // configure for capable auto-negation
	w5500_spi_write_one(PHYCFGR, 0x78); // reset phy
	w5500_spi_write_one(PHYCFGR, 0xF8); // enable phy again.

	/******************** DEFAULT GATEWAY **********************/
	w5500_spi(GAR, _W5500_SPI_WRITE_ , gateAddress, 4);
	/*********************** SUBNET MASK ***********************/
	w5500_spi(SUBR, _W5500_SPI_WRITE_, subAddress, 4);
	/********************SOURCE IP ADDRESS *********************/
	w5500_spi(SIPR, _W5500_SPI_WRITE_, ipAddress, 4);
	/********************SOURCE HW ADDRESS *********************/
	w5500_spi(SHAR, _W5500_SPI_WRITE_, macAddress, 6);
	/***************** INTERRUPT ACTIVATION ********************/
	w5500_spi_write_one(SIMR, 0x01); //enable Socket 0 interrupt

	// TCP initialization
	/***************** SOCKET PORT Number **********************/
	w5500_spi(Sn_PORT(socket), _W5500_SPI_WRITE_, portNumber, 2);
	/********************* MODE: TCP ***************************/
	w5500_spi_write_one(Sn_MR(socket), Sn_MR_TCP);
	/**************** INTERRUPT TRIGGER ************************/
	w5500_spi_write_one(Sn_IMR(socket), W5500_IRQ_CON | W5500_IRQ_RECV); //CONN/RECV Interrupt MASK
	/***************** SOCKET0 STAT OPEN ***********************/
	w5500_spi_write_one(Sn_CR(socket), Sn_CR_OPEN);  //OPEN
	do{
		w5500_spi(Sn_SR(socket), _W5500_SPI_READ_, readbuffer, 2);
	}while (readbuffer[1] != SOCK_INIT); //check/wait if status changed to OPEN

	/***************** SOCKET0 STAT LISTEN *********************/
	w5500_spi_write_one(Sn_CR(socket), Sn_CR_LISTEN); //LISTEN
	do{
		w5500_spi(Sn_SR(socket), _W5500_SPI_READ_, readbuffer, 2);
	}while (readbuffer[1] != SOCK_LISTEN); //check/wait if status changed to LISTEN
	
	tx.TxA.failedADCattempts = tx.TxA.FaultyChain = tx.TxB.failedADCattempts = tx.TxB.FaultyChain = 0; //Reset faulty state of chains. Dont reenable AGC, in case unit is in Loopback mode
}


/*
* @brief Reads the RX buffer of incoming data
*        Format of receiving data:
* bytes:  1    3                 length
*       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*       |ID| Length |             DATA                  |
*       +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*
* @param socket - reading incoming data on socket n
*/
void w5500_RXread(uint8_t socket)
{
	short_msg_t data_len; // length of the incoming message(s)
	short_msg_t data_p; // pointer to the received data
	uint8_t data_arr[MAX_BIN_MSG_SIZE];
	memset(data_arr, 0, sizeof(data_arr));

	/******************* RX DATA SIZE *******************************/
	w5500_spi(Sn_RX_RSR(socket), _W5500_SPI_READ_, data_len.shrt_arr, 2);    // Received Size Register of data

	/******************* RX BUFFER POINTER **************************/
	w5500_spi(Sn_RX_RD(socket), _W5500_SPI_READ_, data_p.shrt_arr, 2);     // Read pointer of receive buffer

	/******************* READ DATA **********************************/
	if (data_len.shrt < MAX_BIN_MSG_SIZE)
	{
		*ETH_GPIOs.W5500.CS_PORT &= ~(1 << ETH_GPIOs.W5500.CS_PIN); // Set Ethernet CS LOW
		
		uint16_t addr = ((_W5500_IO_BASE_ + (WIZCHIP_RXBUF_BLOCK(socket) << 3)) >> 8) + data_p.shrt;
		uint8_t addr_msb = ((addr >> 8) & 0xFF); // MSB of address offset
		uint8_t addr_lSB = (addr & 0xFF);        // LSB of address offset
		uint8_t ctrl = ((_W5500_IO_BASE_ + (WIZCHIP_RXBUF_BLOCK(socket) << 3)) & 0xFF) | _W5500_SPI_READ_ | _W5500_SPI_VDM_OP_; //control bits (BSB + write/read + variable data length)
		//address-phase (16-bit)
		writeread_spi(addr_msb); //SPI MSB address offset
		writeread_spi(addr_lSB); //SPI LSB address offset
		//control-phase (8-bit)
		writeread_spi(ctrl); //SPI control byte
		for (int i = 0; i < data_len.shrt; i++)
		{
			data_arr[i] = writeread_spi(data_arr[i]);
		}
		*ETH_GPIOs.W5500.CS_PORT |= (1 << ETH_GPIOs.W5500.CS_PIN); // Set Ethernet CS HIGH
		//UART_send_string(data_arr);
	}

	/******************* INCREASE RX POINTER ************************/
	data_p.shrt += data_len.shrt;
	w5500_spi(Sn_RX_RD(socket), _W5500_SPI_WRITE_, data_p.shrt_arr, 2);

	/******************* HANDLE COMMAND(S) ************************/
	//to handle 2 or more commands after each other in the RX buffer of the Ethernet chip
	if (data_len.shrt < MAX_BIN_MSG_SIZE)
	{
		if (data_len.shrt >= (data_arr[3])) //multiple frames detected
		{
			uint8_t commandposition = 0;
			do
			{
				/* commandposition += eth_recv_command(data_arr + commandposition, socket); */
				commandposition += eth_recv_command(data_arr + commandposition); //n frame

			} while (data_len.shrt > commandposition + RECORDMARKER_LENGTH);
		}
		else
		{
			send_ack(0xFF, WRONG_RECORDMARKER);
		}
	}

	/******************* ORDER RECV COMMAND *************************/
	w5500_spi_write_one(Sn_CR(socket), Sn_CR_RECV);  //RECV
}


/*
* @brief sending data by sending the data stored in TX buffer. The recordmarker (ID + length is already included)
*  bytes:   1    3                data_length
*         +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*         |ID| Length |             DATA                  |
*         +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
* @param socket      - used socket to send data from MCU to the user
* @param *TC_data    - pointer to data array which will be transmitted (header included)
* @param TX_data_len - length of data
*/
void w5500_TXsend(uint8_t socket, uint8_t *TX_data, uint16_t TX_data_len)
{
	short_msg_t data_p; // pointer to the received data
	long_msg_t recordmarker;
	uint8_t readbuffer[]= {0x00,0x00};

	/******************* TX DATA FREE SIZE READ *******************************/
	w5500_spi(Sn_TX_FSR(socket), _W5500_SPI_READ_, readbuffer, 2);       // Transmitted buffer Size of data

	/******************* TX BUFFER POINTER: RD *******************************/
	w5500_spi(Sn_TX_RD(socket), _W5500_SPI_READ_, data_p.shrt_arr, 2);   // Read pointer of transmit buffer

	/**************************** SEND DATA **********************************/
	*ETH_GPIOs.W5500.CS_PORT &= ~(1 << ETH_GPIOs.W5500.CS_PIN); // Set Ethernet CS LOW
	
	// address-phase (16-bit)
	uint16_t addr = (((_W5500_IO_BASE_ + (WIZCHIP_TXBUF_BLOCK(socket) << 3)) >>  8));
	data_p.shrt_arr[1] += (addr >> 8) & 0xFF;
	data_p.shrt_arr[0] += (addr) & 0xFF;
	short_wr_spi(&data_p);

	// control-phase (8-bit)
	// control bits (BSB + write/read + variable data length)
	uint8_t ctrl = ((_W5500_IO_BASE_ + (WIZCHIP_TXBUF_BLOCK(socket) << 3)) & 0xFF) | _W5500_SPI_WRITE_ | _W5500_SPI_VDM_OP_;
	writeread_spi(ctrl); //SPI control byte

	// Send recordmarker
	recordmarker.msg = TX_data_len + 4;       // should be smaller than 16.7 Mb
	recordmarker.msg_arr[3] = HEADMARKER_ID;  // version number (usually 1)
	long_wr_spi(&recordmarker);

	// Send Data
	for (int i = 0; i < TX_data_len; i++)
	writeread_spi(TX_data[i]);

	*ETH_GPIOs.W5500.CS_PORT |= (1 << ETH_GPIOs.W5500.CS_PIN); // Set Ethernet CS HIGH
	
	/******************* INCREASE TX POINTER: WR ************************/
	data_p.shrt += TX_data_len + RECORDMARKER_LENGTH;
	w5500_spi(Sn_TX_WR(socket), _W5500_SPI_WRITE_, data_p.shrt_arr, 2);

	/******************* ORDER SEND COMMAND *************************/
	w5500_spi_write_one(Sn_CR(socket), Sn_CR_SEND);  //SEND
	w5500_spi_write_one(Sn_IR(socket), W5500_IRQ_SEND_OK); //clear Sn_IR (send interrupt: remark that anyhow, we mask send irqs)
}


/*
* @brief Disconnect the socket and wait till it is closed
*
* @param socket * disconnect the correct socket
*/
void w5500_disconnect(uint8_t socket)
{
	uint8_t readbuffer[]= {0x00,0x00};
	w5500_spi_write_one(Sn_CR(socket), Sn_CR_DISCON); //DISCONNECT
	do{
		w5500_spi(Sn_SR(socket), _W5500_SPI_READ_, readbuffer, 2);

	}while (readbuffer[1] != SOCK_CLOSED); //check/wait if status changed to CLOSED
}

/*
* @brief Close the socket and wait till it is closed
*
* @param socket * disconnect the correct socket
*/
void w5500_close(uint8_t socket)
{
	uint8_t readbuffer[]= {0x00,0x00};
	w5500_spi_write_one(Sn_CR(socket), Sn_CR_CLOSE); //Close
	do{
		w5500_spi(Sn_SR(socket), _W5500_SPI_READ_, readbuffer, 2);

	}while (readbuffer[1] != SOCK_CLOSED); //check/wait if status changed to CLOSED
}


/*
* @brief Check w5500 version number
*
*/
uint8_t read_id_w5500 (void)
{
	uint8_t W5500_check[]= {0x00,0x00};
	w5500_spi(VERSIONR, _W5500_SPI_READ_, W5500_check, 2);
	if (W5500_check[1] == W5500_version)   // Check w5500 version number
	return 1;
	else
	return 0;
}


void w5500_handle_interrupt(void)
{
	/* read interrupt register: and mask out interrupts, these are pending... */
	uint8_t pend_irqs = w5500_spi_read_one(Sn_IR(SOCKET_0));
	/* handle only interrupts which are enabled */
	uint8_t irqs = pend_irqs & w5500_spi_read_one(Sn_IMR(SOCKET_0));

	do
	{
		log_netstate(pend_irqs | 0x80);
		log_netstate(irqs | 0xC0);

		/* Also read out state... */
		uint8_t readbuffer[]= {0x00,0x00};
		w5500_spi(Sn_SR(SOCKET_0), _W5500_SPI_READ_, readbuffer, 2);
		uint8_t state = readbuffer[1];
		log_netstate(state);

		if (irqs & W5500_IRQ_CON)
		{
			if (state == SOCK_ESTABLISHED)
			{
				/* Handshake protocol done */
				eth_status.F_connected = 1;
				w5500_spi_write_one(Sn_IMR(SOCKET_0), W5500_IRQ_RECV | W5500_IRQ_DISCON);
				log_netstate(0xF1);
			}
			w5500_spi_write_one(Sn_IR(SOCKET_0), W5500_IRQ_CON);
		}

		if (irqs & W5500_IRQ_RECV)
		{
			if ((state == SOCK_ESTABLISHED) && eth_status.F_connected)
			{
				w5500_RXread(SOCKET_0);
				log_netstate(0xF2);
			}
			w5500_spi_write_one(Sn_IR(SOCKET_0), W5500_IRQ_RECV);
		}

		if (irqs & W5500_IRQ_DISCON)
		{
			if (state == SOCK_CLOSE_WAIT)
			{
				if (eth_status.F_connected == 1)
				{
					w5500_close(SOCKET_0);
					eth_status.F_connected = 0;
					log_netstate(0xF3);
				}
				log_netstate(0xF4);
				delay_ms(100);
				init_w5500(SOCKET_0);
			}
			else
			{
				log_netstate(0xF5);
				w5500_spi_write_one(Sn_IR(SOCKET_0), W5500_IRQ_DISCON);
			}
		}

		/* check if we did not miss one... reread irqs*/
		pend_irqs = w5500_spi_read_one(Sn_IR(SOCKET_0));
		/* handle only interrupts which are enabled */
		irqs = pend_irqs & w5500_spi_read_one(Sn_IMR(SOCKET_0));
	} while(irqs);
}

void read_eth(void)
{
	uint8_t eeprom_TCP[2];
	uint8_t eeprom_IP[12];
	uint8_t eeprom_MAC[6];
	char IP[32];

	eeprom_read_block((void*)&eeprom_IP, (const void*) ip_p, 12);
	UART_send_string("IP address: \r\n");
	sprintf(IP, "%3d.%3d.%3d.%3d\r\n", eeprom_IP[0], eeprom_IP[1], eeprom_IP[2], eeprom_IP[3]);
	UART_send_string(IP);
	UART_send_string("Subnet Mask: \r\n");
	sprintf(IP, "%3d.%3d.%3d.%3d\r\n", eeprom_IP[4], eeprom_IP[5], eeprom_IP[6], eeprom_IP[7]);
	UART_send_string(IP);
	UART_send_string("Gateway: \r\n");
	sprintf(IP, "%3d.%3d.%3d.%3d\r\n", eeprom_IP[8], eeprom_IP[9], eeprom_IP[10], eeprom_IP[11]);
	UART_send_string(IP);
	eeprom_read_block((void*)&eeprom_TCP, (const void*) tcp_p, 2);
	UART_send_string("Port: \r\n");
	uint16_t TCP = eeprom_TCP[0]<<8 | eeprom_TCP[1];
	sprintf(IP, "%04d\r\n",TCP);
	UART_send_string(IP);
	eeprom_read_block((void*)&eeprom_MAC, (const void*) mac_p, 6);
	UART_send_string("Mac address: \r\n");
	sprintf(IP, "%02x.%02x.%02x.%02x.%02x.%02x\r\n", eeprom_MAC[0], eeprom_MAC[1], eeprom_MAC[2], eeprom_MAC[3],eeprom_MAC[4],eeprom_MAC[5]);
	UART_send_string(IP);
}

bool w5500_wait_for_closed(uint8_t socket, uint32_t timeout_ms)
{
	uint8_t readbuffer[2] = {0x00, 0x00};
	uint32_t elapsed = 0;
	const uint32_t poll_interval_ms = 10;

	while (elapsed < timeout_ms) {
		w5500_spi(Sn_SR(socket), _W5500_SPI_READ_, readbuffer, 2);
		if (readbuffer[1] == SOCK_CLOSED)
		return true;
		delay_ms(poll_interval_ms);
		elapsed += poll_interval_ms;
	}
	return false; // Timed out
}

void w5500_disconnect_then_abort(uint8_t socket, uint32_t timeout_ms)
{
	uint8_t readbuffer[2] = {0x00, 0x00};

	// Read socket status
	w5500_spi(Sn_SR(socket), _W5500_SPI_READ_, readbuffer, 2);

	if (readbuffer[1] == SOCK_ESTABLISHED)
	{
		// Step 1: Send DISCONNECT command to gracefully close
		w5500_spi_write_one(Sn_CR(socket), Sn_CR_DISCON);

		// Step 2: Wait up to timeout_ms for socket to close
		bool closed = w5500_wait_for_closed(socket, timeout_ms);

		// Step 3: If not closed, force CLOSE (abort)
		if (!closed) {
			w5500_spi_write_one(Sn_CR(socket), Sn_CR_CLOSE);

			// Optional: wait until it is actually closed
			do {
				w5500_spi(Sn_SR(socket), _W5500_SPI_READ_, readbuffer, 2);
				delay_ms(10);
			} while (readbuffer[1] != SOCK_CLOSED);
		}
	}
	else
	{
		// Not established � just send CLOSE directly
		w5500_spi_write_one(Sn_CR(socket), Sn_CR_CLOSE);

		// Optional: wait until it is actually closed
		do {
			w5500_spi(Sn_SR(socket), _W5500_SPI_READ_, readbuffer, 2);
			delay_ms(10);
		} while (readbuffer[1] != SOCK_CLOSED);
	}
}

void write_mac_address(const char *mac_str) {
	uint8_t macAddress[6];  // Final binary MAC
	uint8_t macRead[12];    // Hex characters
	int i;

	while (*mac_str == ' ') mac_str++;  // Skip leading spaces

	if (strlen(mac_str) < 12) {
		UART_send_string("ERROR: MAC must be 12 hex digits\r\n");
		return;
	}

	// Parse 12 hex digits into nibbles
	for (i = 0; i < 12; i++) {
		char c = mac_str[i];
		if (c >= '0' && c <= '9') {
			macRead[i] = c - '0';
			} else if (c >= 'A' && c <= 'F') {
			macRead[i] = c - 'A' + 10;
			} else if (c >= 'a' && c <= 'f') {
			macRead[i] = c - 'a' + 10;
			} else {
			UART_send_string("ERROR: Invalid hex digit in MAC\r\n");
			return;
		}
	}

	// Convert to 6 bytes
	for (i = 0; i < 6; i++) {
		macAddress[i] = (macRead[i * 2] << 4) | macRead[i * 2 + 1];
	}

	eeprom_update_block(macAddress, (void*)mac_p, 6);
	UART_send_string("MAC address written to EEPROM\r\n");

	eth_status.F_fin = 1;  // Re-init W5500
}