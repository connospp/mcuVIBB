#!/usr/bin/python3
###
#
#                   C E L E S T I A    A N T W E R P   (c)
#
# A simple to use MCU configuration tool for legacy V-IBB 1.0
#
# Author:   Magdy Abdel
# Created:  20.10.2022
# Version:  v0.1.0
#
#                                 Changelog
#   v0.1.0 Initial version 
#
##
APP_NAME = 'MCU Tool'
APP_DESC = 'A simple to use MCU configuration tool for legacy V-IBB 1.0'
VERS = '0.1.0'
YEAR = 2022

import logging
import sys
import os
import time
import socket
import struct
import enum
import typing

from PySide2.QtCore import Qt, QRegExp, QCommandLineOption, QCommandLineParser
from PySide2.QtGui import QIntValidator, QRegExpValidator, QCloseEvent
from PySide2.QtWidgets import QApplication, QMainWindow, QMessageBox, QLabel, QActionGroup, QAction

from ui_wnd_main import Ui_MainWindow as ui_main

# Logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)


# Default MCU address and port
MCU_ADDR = '192.168.50.10'
MCU_PORT = 5000
MCU_BYTEORDER = 'big'
MCU_SUBNET = '255.255.255.0'
MCU_GATEWAY = '192.168.50.1'
# Default ID for recordmarker
MCU_REC_ID = 0x01
# Recordmarker length defined in bytes
MCU_REC_LEN = 4
# Start of body defined in fields (start = 0)
MCU_BODY_START = 2


# Enum with possible MCU commands/responses: format[0], (C)ID[1]
# They all start with 'B' because the first byte is the CID
class MCU_FMT(enum.Enum):

    # Commands
    CMD_LED             = ('BBB',               0x00)
    CMD_TX_ENABLE       = ('BBB',               0x01)
    CMD_STATUS          = ('BB',                0x02)
    CMD_AGC_SET         = ('BBB'+'h'+'H'*2,     0x03)
    CMD_TX_CAL          = ('B'+'B'*29,          0x04)
    CMD_LOOP            = ('BBB',               0x05)
    CMD_DAC_SET         = ('B'+'B'*4,           0x06)
    CMD_IP              = ('B'+'B'*12,          0x07)
    CMD_PORT            = ('BH',                0x08)
    CMD_HEALTH          = ('B',                 0x09)
    CMD_SN              = ('B',                 [0x0A, 0x0B])
    CMD_TEMP            = ('B',                 0x0C)
    CMD_LED_REF         = ('BB',                0x0D)
    CMD_ADC_READ        = ('BBB',               0x0E)
    CMD_AGC_RESET       = ('BB',                0x0F)
    CMD_VERSION         = ('B',                 0x10)
    CMD_LEVEL_SET       = ('BBBB',              0x11)
    CMD_FILTER_PATH     = ('BBB',               0x12)
    CMD_FREQ_SET        = ('BBBB',              0x13)
    CMD_AGC_ENABLE      = ('BB',                0x14)
    # CMD_SETPOINT_READ   = ('BBB',               0x15)
    CMD_ETHERNET_RESET  = ('B',                 0x16)
    # CMD_DAC_READ        = ('BBB',               0x17)
    # Responses
    RES_ACK             = ('BBB',               0x00)
    RES_STATUS          = ('B'+'B'*5,           0x02)
    RES_HEALTH          = ('BB',                0x09)
    RES_SN              = ('B'+'B'*8,           [0x0A, 0x0B])
    RES_TEMP            = ('BbB',               0x0C) # first byte is value and second byte should be shifted by 6 and divided by 4
    RES_ADC             = ('BBBH',              0x0E)
    RES_VERSION         = ('BBBB',              0x10)

    # Define children of MCU_FMT: format and id
    def __init__(self, format:str, id:typing.Union[int, typing.List[int]]) -> None:
        self.format:str                                 = format
        self.id:typing.Union[int, typing.List[int]]     = id

# Status dot
STATUS_DOT = '\u25cf'
# HEX colors
HEX_RED = 'FF6961'
HEX_YLW = 'F8D66D'
HEX_GRN = '7ABD7E'


# IP address regular expression
ip_range = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])"
regex_ip = QRegExp("^" + ip_range + "\\." + ip_range + "\\." + ip_range + "\\." + ip_range + "$")


# Class that encapsulates an MCU command
class MCU_command():

    def __init__(self, cmd_type:MCU_FMT, params:list=None, cid_index:int=0, id:int=MCU_REC_ID):
        # Command type
        self.cmd_type = cmd_type
        # Format in which to pack the request body: network big endian (!)
        self.format:str = f'!{cmd_type.format}'
        # CID
        self.cid_index:int = cid_index
        self.cid:int = cmd_type.id[cid_index] if type(cmd_type.id) == list else cmd_type.id
        # Parameters of MCU command
        self.params:list = list(params) if type(params) == tuple else (None if params == None else ([params] if type(params) != list else params))
        # Recordmarker ID
        self.id:bytes = id.to_bytes(1, MCU_BYTEORDER, signed=False)
        # Message length in bytes: recordmarker (4B) + body (anyB)
        self.length:int = MCU_REC_LEN + struct.calcsize(self.format)

    # Compare self.cid to CIDs
    def equal_cid(self, to_cid:typing.Union[int, typing.List[int]]):
        equal = False
        if type(self.cmd_type.id) == int:
            if type(to_cid) == int:
                equal = True if self.cid == to_cid else equal
            else:
                for t in to_cid:
                    equal = True if self.cid == t else equal
        else:
            for f in self.cmd_type.id:
                if type(to_cid) == int:
                    equal = True if f == to_cid else equal
                else:
                    for t in to_cid:
                        equal = True if f == t else equal
        return equal

    # Recordmarker (4B): ID (1B) + length (3B)
    def recordmarker(self):
        return self.id + self.length.to_bytes(3, MCU_BYTEORDER, signed=False)

    # MCU request to send over socket: recordmarker (4B) + body (anyB)
    def request(self):
        request = None
        recordmarker = self.recordmarker()
        v = [self.cid] if self.params == None else [self.cid] + self.params
        if self.length > MCU_REC_LEN and recordmarker != None:
            if self.format != None:
                try:
                    logger.debug(f"Packing {v} with '{self.format}' format.")
                    body = struct.pack(self.format, *v)
                    request = recordmarker + body
                    logger.debug(f"Creating request from recordmarker ({recordmarker.hex()}) and body ({body.hex()}): {request.hex()}")
                except struct.error as err:
                    logger.error(f"Unable to pack request ('{self.format}', {v}, len={self.length}): {err}")
            else:
                logger.error(f"Unable to pack request ('{self.format}', {v}, len={self.length}): No format defined for response.")
        else:
            logger.error(f"Unable to pack request ('{self.format}', {v}, len={self.length}): Length of request is less than {MCU_REC_LEN} bytes.")
        return request


# Class that encapsulates an MCU response
class MCU_response():

    # Calculate length based on format
    def calc_length(format:str) -> int:
        length = 0
        if format != None:
            length = MCU_REC_LEN + struct.calcsize(format)
        return length

    # Get length from recordmarker in response
    def get_length(response:bytes) -> int:
        length = 0
        if format != None and len(response[:MCU_REC_LEN]) >= MCU_REC_LEN:
            logger.debug(f"Extracting length {response[1:MCU_REC_LEN].hex()} from recordmarker {response.hex()}")
            length = int.from_bytes(response[1:MCU_REC_LEN], MCU_BYTEORDER, signed=False)
        else:
            logger.debug(f"Unable to extract length {response[1:MCU_REC_LEN].hex()} from recordmarker {response.hex()}: No format defined or length of response too short.")
        return length

    def __init__(self, format:str, response:bytes):
        # Format in which to unpack the response: network big endian (!)
        self.format:str = f'!{format}'
        # Complete received MCU response
        self.response:bytes = response
        # Length of complete received MCU response
        self.length:int = len(self.response) if self.response != None else 0

    # Recordmarker (4B): ID (1B) + length (3B)
    def get_recordmarker(self):
        recordmarker = None
        if self.length > MCU_REC_LEN:
            recordmarker = (int.from_bytes(bytes(self.response[0]), MCU_BYTEORDER, signed=False), 
                            int.from_bytes(self.response[1:MCU_REC_LEN], MCU_BYTEORDER, signed=False))
        else:
            logger.error(f"Unable to get recordmarker ({self.response}, len={len(self.response)}): Length of response is less than {MCU_REC_LEN} bytes.")
        return recordmarker

    # Unpack the response by format and only return data when data_only=True
    def unpack_msg(self, data_only:bool=False):
        message = None
        recordmarker = self.get_recordmarker()
        if self.length > MCU_REC_LEN and recordmarker != None:
            if self.format != None:
                try:
                    message = recordmarker + struct.unpack(self.format, self.response[MCU_REC_LEN:])
                    if data_only: message = message[MCU_BODY_START+1:]
                except struct.error as err:
                    logger.error(f"Unable to unpack response ({self.response.hex()}, len={len(self.response)}): {err}")
            else:
                logger.error(f"Unable to unpack response ({self.response.hex()}, len={len(self.response)}): No format defined for response.")
        else:
            logger.error(f"Unable to unpack response ({self.response.hex()}, len={len(self.response)}): Length of response is less than {MCU_REC_LEN} bytes.")
        return message

    # Get command ID
    def cid(self):
        cid = None
        msg = self.unpack_msg()
        if msg != None: cid = self.unpack_msg()[MCU_BODY_START]
        return cid

# MCU acknowledgement
class MCU_ack(MCU_response):

    def calc_length() -> int:
        return MCU_response.calc_length(MCU_FMT.RES_ACK.format)

    class ID(enum.Enum):
        # (N)ACK error IDs
        ACK                 = 0x00
        UNKNOWN_CMD_ID      = 0x01
        WRONG_PARAM         = 0x02
        WRONG_CMD_LEN       = 0x03
        WRONG_PARAM_LEN     = 0x04
        WRONG_REC_ID        = 0x05

    ack_desc = {
        ID.ACK              : 'Successful',
        ID.UNKNOWN_CMD_ID   : 'Unknown CID',
        ID.WRONG_PARAM      : 'Wrong parameter',
        ID.WRONG_CMD_LEN    : 'Wrong command length',
        ID.WRONG_PARAM_LEN  : 'Wrong parameter length',
        ID.WRONG_REC_ID     : 'Wrong recordmarker ID'
    }

    def __init__(self, response: bytes):
        format = MCU_FMT.RES_ACK.format
        super().__init__(format, response)
    
    # Get command ID of command the ack corresponds to (not the CID of the ack message 0x00)
    def cid(self):
        return self.unpack_msg(data_only=True)[0]

    # Get acknowledgement ID
    def ack(self):
        return self.ID(self.unpack_msg(data_only=True)[1])

    # Return description for acknowledgement
    def __str__(self):
        return self.ack_desc[self.ack()]


# Basic MCU information
class MCU():

    def __init__(self, addr:str=MCU_ADDR, port:int=MCU_PORT):
        # MCU address and port information
        self.addr = addr
        self.port = port
        # MCU socket
        self.sock:socket.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(2)
        self.connected = False

        # Frontend information
        self.version:str = None
        self.sn:list[str] = [None, None]
        
        # Status
        self.reboot = False

    # Configure connection parameters
    def configure(self, addr:str=MCU_ADDR, port:int=MCU_PORT):
        result = False
        if not self.connected:
            logger.debug(f"Configuring the MCU address and port from {self.addr}:{self.port} to {addr}:{port}")
            # MCU address and port information
            self.addr = addr
            self.port = port
            self.sock:socket.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(2)
            result = True
        else:
            logger.error(f"Unable to reconfigure MCU address and port from {self.addr}:{self.port} to {addr}:{port}: Close connection first.")
        return result

    # Open connection to MCU
    def connect(self):
        result = True
        if not self.connected:
            try:
                logger.debug(f"Connecting with MCU ({self.addr}:{self.port})")
                self.sock.connect((self.addr, self.port))
                self.connected = True
                logger.info(f"Connected with MCU ({self.addr}:{self.port})")
            except socket.error as err:
                logger.error(f"Unable to connect with MCU ({self.addr}:{self.port}): {err}")
                self.sock = None
                result = False
        else:
            logger.error(f"Unable to connect with MCU ({self.addr}:{self.port}): Close connection first.")
        return result
    
    # Close connection to MCU 
    def close(self):
        result = False
        # Close the session
        if self.connected:
            logger.info(f"Closing connection with MCU ({self.addr}:{self.port})")
            self.sock.close()
            self.connected = False
            self.reboot = False
            result = True
        else:
            logger.warning(f"Unable to close connection with MCU ({self.addr}:{self.port}): Already closed.")
        return result

    # Send a request to MCU and wait for response
    def send(self, cmd:MCU_command, response_type:MCU_FMT, latest_only:bool=True):
        request:bytes = cmd.request()
        response:bytes = None
        responses:list[MCU_response] = []
        if request != None:
            if self.connected:
                logger.info(f"Sending request (CID=0x{cmd.cid}) to MCU ({self.addr}:{self.port}): {request.hex()} (len={cmd.length})")
                try:
                    self.sock.sendall(request)
                except Exception as err:
                    logger.error(f"Socket error while trying to send request (CID=0x{cmd.cid}) to MCU ({self.addr}:{self.port}): {err}")
                # Loop that waits for bytes until everything is received
                while self.connected:
                    # Get bytes from socket
                    response = self.wait_for_response()
                    # End of stream
                    if response == None: break
                    # Byte marker defining location within retrieved bytes
                    marker = 0
                    prev_marker = marker # Marker of the last byte of previous  retrieved response
                    tmp_response:bytearray = bytearray() # Previous retrieved bytes that are not yet a response
                    # If temporary response is not empty we prepend it to the current response
                    if len(tmp_response) != 0:
                        response = bytes(tmp_response) + response
                        tmp_response.clear()
                    # Keep moving until end of response is reached
                    while marker < len(response):
                        # Check whether remaining length of response is big enough to hold at least the recordmarker
                        if len(response[marker:]) < MCU_REC_LEN: 
                            logger.debug(f"Need more bytes to create a response from received bytes: {response[marker:].hex()}")
                            # Store remaining bytes in tmp_response
                            logger.debug(f"Temporarily storing {response[marker:].hex()}")
                            tmp_response.extend(response[prev_marker:])
                            break
                        logger.debug(f"Checking for recordmarker ID (0x{MCU_REC_ID.to_bytes(1, MCU_BYTEORDER, signed=False).hex()}) at byte {marker}: 0x{response[marker].to_bytes(1, MCU_BYTEORDER, signed=False).hex()}")
                        # Check for recordmarker ID
                        rec_id = struct.unpack('>B', response[marker].to_bytes(1, MCU_BYTEORDER, signed=False))[0]
                        if rec_id == MCU_REC_ID:
                            # Read length from recordmarker
                            length = MCU_response.get_length(response[marker:marker+MCU_REC_LEN])
                            if length > 0 and len(response[marker:marker+length]) >= length:
                                logger.debug(f"Retrieving MCU message (len={length}) from {response[marker:marker+length].hex()}")
                                # Handle response
                                res = None
                                unpacked = None
                                # Get acknowledgement first
                                if len(responses) is 0:
                                    res = MCU_ack(response[marker:marker+length])
                                # Get any other messages if available
                                else:
                                    res = MCU_response(response_type.format, response[marker:marker+length])
                                if res != None:
                                    unpacked = res.unpack_msg()
                                    if unpacked != None:
                                        responses.append(res)
                                        logger.info(f"MCU {'acknowledgement' if isinstance(res, MCU_ack) else 'response' } received (CID=0x{res.cid().to_bytes(1, MCU_BYTEORDER, signed=False).hex()}): {str(res) if isinstance(res, MCU_ack) else unpacked[MCU_BODY_START+1:]}")
                                        # Check whether command CIDs are equal to response CIDs
                                        if not cmd.equal_cid(res.cid()):
                                            logger.warning(f"MCU message CID mismatch: CID is 0x{res.cid().to_bytes(1, MCU_BYTEORDER, signed=False).hex()} but 0x{cmd.cid.to_bytes(1, MCU_BYTEORDER, signed=False).hex()} expected.")
                                        marker += length
                                        prev_marker = marker
                                    else:
                                        logger.error(f"Unable to retrieve MCU message (len={length}) from {response[marker:marker+length].hex()}: Could not unpack.")
                                        marker += 1
                                else:
                                    logger.error(f"Something went wrong creating MCU message (len={length}) from {response[marker:marker+length].hex()}")
                                    marker += 1
                            else:
                                marker +=1
                        else:
                            marker += 1
            else:
                logger.error(f"Unable to send request to MCU ({self.addr}:{self.port}): Not connected.")
        else:
            logger.error(f"Unable to send request to MCU ({self.addr}:{self.port}): Request is empty.")
        return responses[-1] if latest_only and len(responses) != 0 else responses

    # Get response from a socket
    def wait_for_response(self):
        response = None
        if self.connected:
            # Set a time-out period of on all blocking operations on the socket, in 
            # particular the recv() call. Assumption is that the mcu sends all of 
            # its answer(s) in this period.
            self.sock.settimeout(0.5)
            size = 4096 # read buffer size in bytes
            try:
                response = self.sock.recv(size)
                # Server disconnected
                if response is None or len(response) is 0:
                    logger.error(f"Unable to receive response from MCU ({self.addr}:{self.port}): Lost connection.")
                    self.close()
                    self.connected = False
            except socket.timeout:
                logger.debug(f"No response from MCU ({self.addr}:{self.port}): Receive timeout.")
                response = None
            if response != None:
                logger.debug(f"Response from MCU ({self.addr}:{self.port}): {response.hex()} (len={len(response)})")
        else:
            logger.error(f"Unable to receive response from MCU ({self.addr}:{self.port}): Not connected.")
        return response


# Main window
class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        
        # Set up GUI
        self.ui = ui_main()
        self.ui.setupUi(self)

        # Set title
        self.setWindowTitle(f"{APP_NAME} v{VERS}")

        # MCU object
        self.mcu = MCU()
        
        # Status
        lbl_status = QLabel(self.ui.statusbar)
        lbl_status.setObjectName('lbl_status')
        lbl_status.setTextFormat(Qt.RichText)
        lbl_status.setContentsMargins(9, 0, 9, 3)
        self.ui.statusbar.addPermanentWidget(lbl_status)
        self.status = Status(self, self.mcu)
        self.status.update()

        # Validator for IP addresses
        val_ip = QRegExpValidator(regex_ip, self)
        # Validator for port numbers
        val_port = QIntValidator(1, 65535, self)

        # Setting fields and buttons
        self.ui.line_host.setText(MCU_ADDR)
        self.ui.line_host.setValidator(val_ip)
        self.ui.line_hostport.setValidator(val_port)
        self.ui.line_hostport.setText(str(MCU_PORT))
        self.ui.line_ip.setValidator(val_ip)
        self.ui.line_ip.setText(MCU_ADDR)
        self.ui.line_port.setValidator(val_port)
        self.ui.line_port.setText(str(MCU_PORT))
        self.ui.line_subnet.setValidator(val_ip)
        self.ui.line_subnet.setText(MCU_SUBNET)
        self.ui.line_defgate.setValidator(val_ip)
        self.ui.line_defgate.setText(MCU_GATEWAY)

        # Creating menus
        self.loglevel_group = QActionGroup(self.ui.menuLog_level)
        self.loglevel_group.addAction(self.ui.actionDebug)
        self.loglevel_group.addAction(self.ui.actionInfo)
        self.loglevel_group.addAction(self.ui.actionWarning)
        self.loglevel_group.addAction(self.ui.actionError)
        self.loglevel_group.addAction(self.ui.actionCritical)

        # Setup button click signals and slots
        self.ui.btn_connect.clicked.connect(self.connect_mcu)
        self.ui.btn_configure.clicked.connect(self.configure_mcu)

        # Setup menu action click signals and slots
        self.loglevel_group.triggered.connect(self.setloglevel)
        self.ui.actionHelp.triggered.connect(self.showHelp)

    # Connect/disconnect to/from MCU and request information when connected
    def connect_mcu(self):
        configured = False
        # Connected
        if self.mcu.connected:
            # Disconnect from MCU
            closed = self.mcu.close()
            if closed:
                self.status.message(f"MCU disconnected!", 3000)
        # Disconnected
        else:
            # Configure host address and port
            if self.ui.line_host.text() != '' and self.ui.line_hostport.text() != '':
                configured = self.mcu.configure(self.ui.line_host.text(), int(self.ui.line_hostport.text()))
            else:
                logger.warning(f"Unable to connect to MCU: Host address or port is empty!")        
            # Connect to MCU
            if configured:
                self.mcu.connect()
            if self.mcu.connected:
                self.status.message(f"MCU connected!", 3000)
                # Request MCU information (version, serial numbers)
                # Version
                cmd = MCU_command(MCU_FMT.CMD_VERSION)
                res = self.mcu.send(cmd, MCU_FMT.RES_VERSION).unpack_msg(True)
                self.mcu.version = f'{res[0]}.{res[1]}.{res[2]}'
                logger.info(f"MCU version: {self.mcu.version}")
                # Serial numbers
                for n in [0, 1]:
                    cmd = MCU_command(MCU_FMT[f'CMD_SN'], cid_index=n)
                    res = self.mcu.send(cmd, MCU_FMT.RES_SN).unpack_msg(True)
                    self.mcu.sn[n] = f"{''.join([str(i) for i in res[:4]])}/{''.join([str(i) for i in res[4:]])}"
                    logger.info(f"MCU serial number {n+1}: {self.mcu.sn[n]}")
        self.status.update()

    # Configure MCU
    def configure_mcu(self):
        if self.mcu.connected:
            ip = self.ui.line_ip.text()
            port = self.ui.line_port.text()
            subnet = self.ui.line_subnet.text()
            default_gateway = self.ui.line_defgate.text() 
            # Check al necessary fields whether empty ('') or invalid (regexp)
            if ip != '' and port != '' and subnet != ''  and default_gateway != '' :
                # Check IP
                ip_match = regex_ip.exactMatch(ip)
                logger.debug(f"Valid IP address: {ip_match}")
                # Check port
                port_match = port.isdecimal()
                logger.debug(f"Valid port: {port_match}")
                # Check subnet
                subnet_match = regex_ip.exactMatch(subnet)
                logger.debug(f"Valid subnet: {subnet_match}")
                # Check default gateway
                defgate_match = regex_ip.exactMatch(default_gateway)
                logger.debug(f"Valid default gateway: {defgate_match}")
                if ip_match and subnet_match and defgate_match:
                    # Change port first because IP change will trigger watchdog
                    cmd_port    = MCU_command(MCU_FMT.CMD_PORT, int(port))
                    res:MCU_ack = self.mcu.send(cmd_port, MCU_FMT.RES_ACK)
                    if res.ack() == MCU_ack.ID.ACK:
                        self.mcu.reboot = True
                    self.status.message(f"IP port: {str(res)}", 3000)
                    cmd_ip      = MCU_command(MCU_FMT.CMD_IP, [int(b) for b in ip.split('.')] + [int(b) for b in subnet.split('.')] + [int(b) for b in default_gateway.split('.')])
                    res:MCU_ack = self.mcu.send(cmd_ip, MCU_FMT.RES_ACK)
                    if res.ack() == MCU_ack.ID.ACK:
                        self.mcu.reboot = True
                    self.status.message(f"IP settings: {str(res)}", 3000)
                    self.status.update()
                    # Disconnect from the MCU
                    self.connect_mcu()
            else:
                logger.warning(f"Unable to connect to MCU: Some fields are empty!")  
        else:
            logger.error(f"Unable to configure settings for MCU: Not connected.")

    # Set logging level
    def setloglevel(self, action:QAction=None, loglevel:str=None):
        level = None if loglevel == None else loglevel.upper()
        levels = {
            'DEBUG'     : self.ui.actionDebug,
            'INFO'      : self.ui.actionInfo,
            'WARNING'   : self.ui.actionWarning,
            'ERROR'     : self.ui.actionError,
            'CRITICAL'  : self.ui.actionCritical
        }
        if action != None:
            for l, a in levels.items():
                if a.isChecked(): 
                    level = l
                    break
        logger.debug(f"Changing log level to: {level}")
        logger.setLevel(level)
        levels[level].setChecked(True)
        logger.info(f"Changed log level to: {level}")
        
    def showHelp(self):
        QMessageBox.information(self.centralWidget(), f"Help - {self.windowTitle()}", 
        f"{self.windowTitle()}\nCelestia Antwerp \u00A9 {YEAR}\n{APP_DESC}\n\nIn case the IP address is unknown it is possible to reset the IP settings by pressing the reset button at the back of the frontend.\nDefault settings are listed below.\n\nFactory IP address: {MCU_ADDR}\nFactory subnet: {MCU_SUBNET}\nFactory default gateway: {MCU_GATEWAY}\nFactory port: {MCU_PORT}", 
        buttons=QMessageBox.StandardButton.Ok, defaultButton=QMessageBox.StandardButton.Ok)

    # Override close event to do some checks before closing the window if needed
    def closeEvent(self, event:QCloseEvent):
        return super(QMainWindow, self).closeEvent(event)


# Status bar message
class Status():
    def __init__(self, main:MainWindow, mcu:MCU):
        self.main:MainWindow = main
        self.mcu:MCU = mcu

    def tag(self, color:str, type:str='span'):
        return f"<{type} style='color:#{color}'>"

    def __str__(self):
        msg = f"MCU {self.tag(HEX_RED, 'b') if not self.mcu.connected else self.tag(HEX_GRN, 'b')}{STATUS_DOT}</b>"
        if self.mcu.connected: msg += f" (v{self.mcu.version})"
        if self.mcu.reboot: msg += f" {self.tag(HEX_RED, 'b')}MCU rebooting...</b>"
        return msg

    # Update statusbar message and other UI fields
    def update(self):
        lbl:QLabel = self.main.ui.statusbar.findChild(QLabel, 'lbl_status')
        if lbl != None:
            lbl.setText(str(self))
        if self.mcu.connected:
            self.main.ui.line_host.setEnabled(False)
            self.main.ui.line_hostport.setEnabled(False)
            self.main.ui.grp_ip.setEnabled(True)
            self.main.ui.btn_connect.setText(self.main.tr('Disconnect'))
        else:
            self.main.ui.line_host.setEnabled(True)
            self.main.ui.line_hostport.setEnabled(True)
            self.main.ui.grp_ip.setEnabled(False)
            self.main.ui.btn_connect.setText(self.main.tr('Connect'))

    # Display message in statusbar
    def message(self, msg:str, timeout:int=0):
        self.main.ui.statusbar.showMessage(msg, timeout)

    # Clear message in statusbar
    def clear(self):
        self.main.ui.statusbar.clearMessage()

if __name__ == "__main__":
    exitcode = 0
    try:
        # Initialise logger
        logFormatter = logging.Formatter("%(asctime)s [%(levelname)s]  \t%(message)s")
        logPath = os.path.join('logs', f'mcu_configuration_legacy_{time.strftime("%Y%m%d_%H%M%S")}.log')
        os.makedirs(os.path.dirname(logPath), exist_ok=True)
        fileHandler = logging.FileHandler(logPath, mode='w')
        fileHandler.setFormatter(logFormatter)
        logger.addHandler(fileHandler)
        consoleHandler = logging.StreamHandler()
        consoleHandler.setFormatter(logFormatter)
        logger.addHandler(consoleHandler)

        logger.info(f"== {APP_NAME} v{VERS} ==")

        # Initialise application
        app = QApplication(sys.argv)
        app.setApplicationName(APP_NAME)
        app.setApplicationDisplayName(f"{APP_NAME} v{VERS}")
        app.setApplicationVersion(f'{VERS}')

        # Define and process command line arguments
        parser = QCommandLineParser()
        parser.setApplicationDescription(APP_DESC)
        parser.addHelpOption()
        parser.addVersionOption()
        opt_loglevel = QCommandLineOption(['loglevel'], 'Logging level [debug, info, warning, error, critical]', 'level', 'debug')
        parser.addOption(opt_loglevel)
        parser.process(app)
        # Setting log level
        log_level = None
        if parser.isSet(opt_loglevel):
            log_level = parser.value(opt_loglevel).upper()
            if log_level not in ['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL']:
                print(f"{APP_NAME}: Invalid log level '{log_level}' defined in command line.")
                exitcode = 126  # Command invoked cannot execute
        
        # If no exitcodes generated start mainwindow
        wnd_main = None
        if exitcode is 0:
            # Create and show mainwindow
            wnd_main = MainWindow()
            # Set log level
            if log_level != None:
                wnd_main.setloglevel(loglevel=log_level)
            wnd_main.setMinimumSize(wnd_main.width(), wnd_main.height())
            wnd_main.show()
            exitcode = app.exec_()
    except KeyboardInterrupt as err:
        logger.info(f"Killing program.")
        exitcode = 137 # SIGKILL EXIT
    # Exit application

    # Close anything related to main window
    if wnd_main != None:
        # Close the mcu connection
        if wnd_main.mcu.connected: wnd_main.mcu.close()

    logger.info(f"Shutting down.")
    sys.exit(exitcode)
