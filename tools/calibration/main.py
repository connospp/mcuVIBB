#!/usr/bin/python3
###
#
#                   C E L E S T I A    A N T W E R P   (c)
#
# A simple to use MCU calibration tool
#
# Author:   Magdy Abdel / Nico Henriques da Silva
# Created:  13.10.2022
# Version:  v0.1.0
#
#                                 Changelog
#   v0.1.0 Initial version 
#   v0.2.0 Added RX calibration
#   v0.3.0 Added save and load features
#   v0.4.0 Added 6 additional TX calibration points
#   v0.5.0 Changed to 11 TX calibration points with different DAC values
#
##
APP_NAME = 'MCU Calibration'
APP_DESC = 'A simple to use MCU calibration tool'
VERS = '0.5.0'
YEAR = 2022

import logging
import sys
import os
import time
import socket
import json
import datetime
import shutil

import signal
from pathlib import Path

from PySide2.QtCore import Qt, QCommandLineOption, QCommandLineParser, Slot, QTimer
from PySide2.QtGui import QIntValidator, QDoubleValidator, QCloseEvent
from PySide2.QtWidgets import QApplication, QMainWindow, QLineEdit, QMessageBox, QLabel, QWidget, QSizePolicy, QFileDialog

from ui_wnd_main import Ui_MainWindow as ui_main

# Logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

# Exit code
app_exitcode = 0

# Handler for SIGINT signal
def sigint_handler(signum, frame):
    logger.info(f"Killing application.")
    QApplication.quit()

# Enable/disable RX calibration
enable_rx = False

# GPFE port
GPFE_PORT = 12000
# GPFE user password
GPFE_PASS = 'mahakala'

# List of default DAC values to be calibrated (Globalstar -30 to 0 dBm)
DAC = [
    '0000',
    '07B0',
    '07D9',
    '07FF',
    '0829',
    '0860',
    '08A0',
    '08D0',
    '0912',
    '093A',
    '0FFF'
]

# Dictionary with MCU commands
MCU_CMD = {
    'version':      'version',
    'agc':          'agc {}',                                             # enable/disable
    'sn':           'sn{}',                                               # chain(1/2)
    'setdac':       'dac {} {} {} {}',                                    # chain(0/1) direct(tx/rx) DAC_ID(0,1,2) val(2Bhex)
    'getadc':       'adc {} {}',                                          # chain(0/1) direct(tx/rx)
    'readsetpt':    'setpoint {} {}',                                     # chain(0/1) direct(tx/rx)
    'setagcpar':    'agcpar {} {} {} {} {}',                              # chain(0/1) direct(tx/rx) rxoffset(2Bint) setpoint(2Buint) timeconstant(2Buint) 
    'cal':          'txcal {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {} {}',   # chain(0/1) 11x[lvl(2Bhex,cBm) adc(2Bhex)]
    'resetcal':     'resetcal {}',                                        # chain(0/1)
}
TX = 'tx'
RX = 'rx'

# HEX colors
HEX_RED = 'FF6961'
HEX_YLW = 'F8D66D'
HEX_GRN = '7ABD7E'

# Class that handles all GPFE communication
class GPFE():
    def __init__(self, instance:int, chain:int, user:str=None, password:str=GPFE_PASS):
        if chain % 2 == 1:
            chain = 0
        else:
            chain = 1
        self.configure(instance, chain, user, password)
        # GPFE sockets
        self.sock = {
            'mon': None,
            'ctl': None
        }

    # Configure GPFE connection
    def configure(self, instance:int, chain:int, user:str=None, password:str=GPFE_PASS):
        self.instance = instance
        self.chain = chain
        self.user = user if user else f'chain{self.chain + 2*self.instance-1}'
        self.password = password
        # GPFE address and port information
        self.addr = f'gpfe{self.instance}_itf'
        self.port = GPFE_PORT

    # Connect monitoring and control connection on GPFE
    def connect(self):
        result = True
        for s in self.sock.items():
            # Get session type (mon, ctl)
            session = s[0]
            # Setup the session
            logger.info(f"Setting up {session} session with gpfe ({self.addr}:{self.port})")
            self.sock[session] = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                self.sock[session].connect((self.addr, self.port))
            except socket.error as err:
                logger.error(f"Unable to connect with gpfe ({self.addr}:{self.port}) to set up {session} session")
                self.sock[session] = None
                result = False
                break
            # Log in
            if self.sock[session] != None:
                response = self.send_msg(self.sock[session], f'{self.user} {self.password} !{session}\n')
                if response is None:
                    logger.warning(f"Unable to log in {session} session with gpfe ({self.addr}:{self.port})")
                    result = False
        return result

    # Close monitoring and control connection to GPFE 
    def close(self):
        for s in self.sock.items():
            # Get session type (mon, ctl)
            session = s[0]
            # Close the session
            if self.sock[session] != None:
                logger.info(f"Closing {session} session with gpfe ({self.addr}:{self.port})")
                self.sock[session].close()
                self.sock[session] = None
            else:
                logger.warning(f"Unable to close {session} session with gpfe ({self.addr}:{self.port}): Already closed.")

    # Send a message to the GPFE socket and return the response
    def send_msg(self, sock:socket.socket, msg:str):
        response = None
        if sock != None:
            session = list(self.sock.keys())[list(self.sock.values()).index(sock)]
            logger.debug(f"Sending message to GPFE {session} session on ({sock.getsockname()}): {msg}")
            sock.send(f"{msg}\n".encode())
            response = self.get_response(sock)
        else:
            logger.error(f"Unable to send message to GPFE: Not connected.")
        return response
    
    # Simple function to send MCU commands through the GPFE sys0 module. We can
    # not keep the sys0 module because the moncon will not be able to use it if
    # we do. Which is why we only add it when we send an MCU command and remove
    # it again after.
    def send_mcu_msg(self, msg:str):
        # We do not return the responses and we ignore the mon sock messages
        self.send_msg(self.sock['ctl'], f"+mod sys0")
        self.get_response(self.sock['mon'])
        self.send_msg(self.sock['mon'], f"sys0 +disp")
        self.send_msg(self.sock['ctl'], f"sys0 fe {msg}")
        self.send_msg(self.sock['ctl'], f"-mod sys0")
        time.sleep(0.5)
        response = self.send_msg(self.sock['mon'], f"sys0 -disp")
        self.get_response(self.sock['mon'])
        return response

    # Get response from a socket
    def get_response(self, sock:socket.socket):
        response = None
        if sock != None:
            session = list(self.sock.keys())[list(self.sock.values()).index(sock)]
            # Set a time-out period of on all blocking operations on the socket, in 
            # particular the recv() call. Assumption is that the gpfe sends all if 
            # its answer(s) in this period.
            sock.settimeout(0.5)
            size = 4096 # read buffer size in bytes
            try:
                response = sock.recv(size)
            except socket.timeout:
                logger.warning(f"No answer received from GPFE {session} session on {sock.getsockname()}")
                response = None
            if response != None:
                logger.debug(f"Received message from GPFE {session} session on ({sock.getsockname()}): {response.decode()}")
        else:
            logger.error(f"Unable to receive message from GPFE: Not connected.")
        return response

    # Return list of values from first found response
    def get_values(self, msg:str, startswith:str, index:int=None):
        result = None
        if msg != None and startswith != None:
            lines = msg.splitlines()
            for line in lines:
                logger.debug(f"Searching for '{startswith}' in: {line}")
                s = line.split()
                if s[1] == startswith:
                    result = s[1:]
                    break
            if result != None and index != None: result = result[index]
        return result 


# Basic MCU information
class MCU():
    def __init__(self):
        self.version:str = None
        self.sn:str = None
        # The assumption is made that AGC is enabled by default. There is no 
        # command to know what the AGC status is. 
        self.agc:bool = True

    def reset(self):
        self.__init__()

# Status bar message
class Status():
    def __init__(self, ui:ui_main, mcu:MCU):
        self.ui = ui
        self.con_gpfe:bool = False
        self.con_mcu:bool = False
        self.mcu:MCU = mcu

    def tag(self, color:str, type:str='span'):
        return f"<{type} style='color:#{color}'>"

    def __str__(self):
        msg = f"GPFE {self.tag(HEX_RED, 'b') if not self.con_gpfe else self.tag(HEX_GRN, 'b')}\u25cf</b> | MCU {self.tag(HEX_RED, 'b') if not self.con_mcu else self.tag(HEX_GRN, 'b')}\u25cf</b>"
        if self.con_mcu: msg += f" (v{self.mcu.version}) | AGC {self.tag(HEX_GRN, 'b') if self.mcu.agc else self.tag(HEX_RED, 'b')}\u25cf</b>"
        return msg

    # Update statusbar message
    def update(self):
        lbl:QLabel = self.ui.statusbar.findChild(QLabel, 'lbl_status')
        if lbl != None:
            lbl.setText(str(self))

# Main window
class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        # Set up GUI
        self.ui = ui_main()
        self.ui.setupUi(self)

        # Enable/disable rx calibration
        if not enable_rx: 
            self.ui.tabs.removeTab(1)
            self.ui.tabs.setCurrentIndex(0)

        # GPFE and MCU objects
        self.gpfe = GPFE(int(self.ui.combo_gpfe.currentText()), int(self.ui.combo_chain.currentText()))
        self.mcu = MCU()
        
        # Status
        lbl_status = QLabel(self.ui.statusbar)
        lbl_status.setObjectName('lbl_status')
        lbl_status.setTextFormat(Qt.RichText)
        lbl_status.setContentsMargins(5, 3, 10, 3)
        self.ui.statusbar.addPermanentWidget(lbl_status)
        self.status = Status(self.ui, self.mcu)
        self.status.update()

        # Current row of calibration:
        #   -1      Calibration not active
        #   0-N     Current calibration row
        self.cal_row = -1

        # Lists of QLineEdits for displayed calibration values
        self.dac:list[QLineEdit] = []
        self.adc:list[QLineEdit] = []
        self.olvl:list[QLineEdit] = []

        # Validator for output level (in dBm): -90.0dBm .. 30.0dBm
        val_olvl = QDoubleValidator(-90.0, 30.0, 1, self)
        val_olvl.setNotation(QDoubleValidator.StandardNotation)
        # Validator for offsets (in dBm): -130.0dBm .. 130.0dBm
        val_offset = QDoubleValidator(-130.0, 130.0, 1, self)
        val_offset.setNotation(QDoubleValidator.StandardNotation)
        # Validator for timeconstant (in ms): 20ms .. 10000ms
        val_timeconst = QIntValidator(20, 10000, self)

        # Add 13 TX calibration rows
        for i in range(0, len(DAC), 1):
            # Create line edits
            line_dac = QLineEdit(self.ui.grp_table)
            line_adc = QLineEdit(self.ui.grp_table)
            line_olvl = QLineEdit(self.ui.grp_table)
            # Configure line edits
            line_dac.setDisabled(True)
            line_dac.setInputMask('HHHH')
            line_dac.setText(DAC[i])
            line_dac.sizePolicy().setVerticalPolicy(QSizePolicy.Fixed)
            line_dac.show()

            line_adc.setDisabled(True)
            line_adc.setInputMask('HHHH')
            line_adc.sizePolicy().setVerticalPolicy(QSizePolicy.Fixed)
            line_adc.show()

            line_olvl.setDisabled(True)
            line_olvl.setValidator(val_olvl)
            line_olvl.sizePolicy().setVerticalPolicy(QSizePolicy.Fixed)
            line_olvl.show()
            # Add line edits to table and lists
            self.ui.grd_table.addWidget(line_dac)
            self.dac.append(line_dac)
            self.ui.grd_table.addWidget(line_adc)
            self.adc.append(line_adc)
            self.ui.grd_table.addWidget(line_olvl)
            self.olvl.append(line_olvl)
        # Update TX geometry
        self.ui.grp_table.updateGeometry()

        # Set RX masks and validators
        self.ui.line_setpt.setInputMask('HHHH')
        self.ui.line_defoffset.setValidator(val_offset)
        self.ui.line_injsig1.setValidator(val_offset)
        self.ui.line_injsig2.setValidator(val_offset)
        self.ui.line_carlvl1.setValidator(val_offset)
        self.ui.line_carlvl2.setValidator(val_offset)
        self.ui.line_difoffset1.setValidator(val_offset)
        self.ui.line_difoffset2.setValidator(val_offset)
        self.ui.line_meandiff.setValidator(val_offset)
        self.ui.line_offset.setValidator(val_offset)
        # TODO fix, does not work for some reason
        self.ui.line_timeconst.setValidator(val_timeconst)

        # Setup button click signals and slots
        #Settings
        self.ui.btn_connect.clicked.connect(self.connect_to_gpfe)
        self.ui.btn_setagc.clicked.connect(self.set_agc)
        #Tx
        self.ui.btn_setdac.clicked.connect(self.set_tx_dac)
        self.ui.btn_prev.clicked.connect(self.goto_prev)
        self.ui.btn_next.clicked.connect(self.goto_next)
        self.ui.btn_cali.clicked.connect(self.calibrate_tx)
        self.ui.btn_reset.clicked.connect(self.reset_tx_cal)
        #Rx
        self.ui.btn_readsetpt.clicked.connect(self.get_rx_adc)
        self.ui.btn_clrsetpt.clicked.connect(self.clear_rx_adc)
        self.ui.btn_setpt.clicked.connect(self.set_default_rx_setpoint)
        self.ui.btn_rxcal.clicked.connect(self.calibrate_rx)

        # Setup field changed signals and slots
        #Rx
        # Update difference offset when carrier level is filled out
        self.ui.line_carlvl1.editingFinished.connect(self.update_offset_field)
        self.ui.line_carlvl2.editingFinished.connect(self.update_offset_field)
        # Update mean difference when difference offset is updated
        self.ui.line_difoffset1.textChanged.connect(self.update_offset_field)
        self.ui.line_difoffset2.textChanged.connect(self.update_offset_field)
        # Update offset when mean difference is updated
        self.ui.line_meandiff.textChanged.connect(self.update_offset_field)

        # Add action to save calibration values
        self.ui.actionSave.triggered.connect(self.open_savecal_window)
        # Add action to load calibration values
        self.ui.actionLoad.triggered.connect(self.open_loadcal_window)

        # Only activate row by row after clicking next (disable previous row).
        # Clicking 'Set DAC' sets the DAC and measures the ADC, output level is 
        # manually written, click next to move on to next row.
        # After all 11 points are measured, enable calibrate button.
        # Clicking calibrate will send the calibration table to the MCU.
        
        self.resize(self.sizeHint())

    # Callback that handles connecting to or disconencting from the GPFE and setting up GUI and environment
    def connect_to_gpfe(self):
        if self.ui.btn_connect.text() == 'Connect':
            self.gpfe.configure(int(self.ui.combo_gpfe.currentText()), int(self.ui.combo_chain.currentText()))
            self.status.con_gpfe = self.gpfe.connect()
        else:
            self.gpfe.close()
            self.status.con_gpfe = False

        version = None
        if self.status.con_gpfe:
            version = self.gpfe.send_mcu_msg(MCU_CMD['version'])
        if version != None:
            vers = self.gpfe.get_values(version.decode(), 'fe_version')
            self.mcu.version = f"{vers[1]}.{vers[2]}.{vers[3]}"
            self.status.con_mcu = True
            # Disable GPFE and chain fields
            self.ui.combo_gpfe.setEnabled(False)
            self.ui.combo_chain.setEnabled(False)
            self.ui.btn_connect.setText('Disconnect')
            # Enable AGC so we know what state the AGC is in since we cannot request it
            self.gpfe.send_mcu_msg(MCU_CMD['agc'].format('enable' if self.mcu.agc else 'disable'))
            # Enable RX calibration fields and buttons
            self.ui.line_carlvl1.setEnabled(True)
            self.ui.line_carlvl2.setEnabled(True)
            self.ui.line_timeconst.setEnabled(True)
            self.ui.btn_rxcal.setEnabled(True)
            # Enable agc button
            self.ui.btn_setagc.setEnabled(True)
        else:
            self.status.con_mcu = False
            self.mcu.reset()
            # Enable GPFE and chain fields and disable agc button
            self.ui.combo_gpfe.setEnabled(True)
            self.ui.combo_chain.setEnabled(True)
            self.ui.btn_connect.setText('Connect')
            self.ui.btn_setagc.setText('Disable AGC')
            self.ui.btn_setagc.setEnabled(False)
            # Disable calibration fields and reset everything
            #Handle TX calibration
            # Calibration not active
            self.cal_row = -1
            for i in range(0, len(DAC), 1):
                self.dac[i].setEnabled(False)
                # Uncomment next line if you want to reset DAC values to default ones
                # self.dac[i].setText(DAC[i])
                self.adc[i].clear()
                self.olvl[i].setEnabled(False)
                self.olvl[i].clear()
            self.ui.btn_prev.setEnabled(False)
            self.ui.btn_next.setEnabled(False)
            self.ui.btn_setdac.setEnabled(False)
            self.ui.btn_cali.setEnabled(False)
            self.ui.btn_reset.setEnabled(False)
            #Handle RX calibration
            # self.ui.line_setpt.setEnabled(False)
            # self.ui.line_setpt.clear()
            self.ui.btn_readsetpt.setEnabled(False)
            self.ui.btn_clrsetpt.setEnabled(False)
            self.ui.btn_setpt.setEnabled(False)
            self.ui.line_carlvl1.setEnabled(False)
            self.ui.line_carlvl2.setEnabled(False)
            self.ui.line_carlvl1.clear()
            self.ui.line_carlvl2.clear()
            self.ui.line_difoffset1.clear()
            self.ui.line_difoffset2.clear()
            self.ui.line_meandiff.clear()
            self.ui.line_offset.clear()
            self.ui.line_timeconst.setEnabled(False)
            self.ui.line_timeconst.setText('1000')
            self.ui.btn_rxcal.setEnabled(False)

        logger.info(f"GPFE:\t{self.gpfe.addr}:{self.gpfe.port} ({'Connected' if self.status.con_gpfe else 'Disconnected'})")
        logger.info(f"CHAIN:\t{self.gpfe.chain}")
        logger.info(f"MCU:\tv{self.mcu.version} ({'Connected' if self.status.con_mcu else 'Disconnected'})")

        self.status.update()

    # Callback that handles enabling/disabling the AGC
    def set_agc(self):
        if not self.status.con_gpfe or not self.status.con_mcu:
            logger.error(f"Unable to change AGC: Not connected to GPFE or no MCU connected.")
            return
        # Set AGC
        self.mcu.agc = not self.mcu.agc
        logger.info(f"{'En' if self.mcu.agc else 'Dis'}abling AGC")
        self.gpfe.send_mcu_msg(MCU_CMD['agc'].format('enable' if self.mcu.agc else 'disable'))
        self.status.update()
        # Change button text
        self.ui.btn_setagc.setText(f"{'Enable' if not self.mcu.agc else 'Disable'} AGC")
        # Enable calibration fields
        #TX
        if self.cal_row is -1: self.goto_next()
        self.ui.btn_setdac.setEnabled(True)
        self.ui.btn_next.setEnabled(True)
        self.ui.btn_reset.setEnabled(True)
        self.ui.grp_table.setEnabled(True)
        #RX
        # self.ui.line_setpt.setEnabled(True)
        if self.ui.line_setpt.text() == '' and not self.mcu.agc:
            self.ui.btn_readsetpt.setEnabled(True)
        self.ui.btn_clrsetpt.setEnabled(True)
        self.ui.btn_setpt.setEnabled(True)
        if self.ui.line_setpt.text() != '':
            self.ui.line_carlvl1.setEnabled(self.mcu.agc)
            self.ui.line_carlvl2.setEnabled(self.mcu.agc)
        self.ui.line_timeconst.setEnabled(True)
        self.ui.btn_rxcal.setEnabled(True)

    # Callback that handles sending TX DAC value to MCU
    def set_tx_dac(self):
        if self.cal_row is -1: return
        # Set DAC
        logger.info(f"Setting DAC to {self.dac[self.cal_row].text()}")
        self.gpfe.send_mcu_msg(MCU_CMD['setdac'].format(self.gpfe.chain, TX, 0, self.dac[self.cal_row].text()))
        self.gpfe.send_mcu_msg(MCU_CMD['setdac'].format(self.gpfe.chain, TX, 1, self.dac[self.cal_row].text()))
        self.gpfe.send_mcu_msg(MCU_CMD['setdac'].format(self.gpfe.chain, TX, 2, self.dac[self.cal_row].text()))
        # Get measured ADC
        adc = self.get_tx_adc()
        self.adc[self.cal_row].setText(adc)

    # Callback that handles getting TX ADC value 
    def get_tx_adc(self):
        adc = []
        for i in range(0, 5):
            # Read ADC value
            response = self.gpfe.send_mcu_msg(MCU_CMD['getadc'].format(self.gpfe.chain, TX))
            if response != None:
                adc.append(self.gpfe.get_values(response.decode(), 'fe_adc', 3))
                logger.debug(f"adc[{i}] {adc[i]}")
            time.sleep(0.1)
        # Calculate mean ADC from last three ADC values
        max_tries = 3
        current_try = 0
        while current_try < max_tries:
            current_try += 1
            no_problem = True
            try:
                adc_mean = sum([int.from_bytes(bytes.fromhex(a), byteorder='big', signed=True) for a in adc[-3:]])//len(adc[-3:]) # Convert to integers first
            except Exception as e:
                print(f"Problem getting adc_mean: {e}")
                no_problem = False
            if no_problem: break
            
        adc_mean = adc_mean.to_bytes(length=2, byteorder='big', signed=True).hex() # Convert back to hex
        logger.debug(f"adc[mean] {adc_mean}")
        return adc_mean

    # Callback that handles going to previous TX calibration row
    def goto_prev(self):
        logger.debug("To be implemented")

    # Callback that handles going to next TX calibration row
    def goto_next(self):
        if self.cal_row is not -1 and (self.adc[self.cal_row].text() == '' or self.olvl[self.cal_row].text() == ''): return
        # Increase row
        self.cal_row += 1
        # Disable fields
        self.dac[self.cal_row-1].setEnabled(False)
        self.olvl[self.cal_row-1].setEnabled(False)
        if self.cal_row is len(DAC): 
            # Disable next button
            self.ui.btn_next.setEnabled(False)
            # Disable set dac button
            self.ui.btn_setdac.setEnabled(False)
            # Enable calibrate button
            self.ui.btn_cali.setEnabled(True)
            # Enable agc button
            self.ui.btn_setagc.setEnabled(True)
        else:
            # Enable fields
            if self.cal_row == 0 or self.cal_row == len(DAC)-1: self.dac[self.cal_row].setEnabled(False)
            else: self.dac[self.cal_row].setEnabled(True)
            self.olvl[self.cal_row].setEnabled(True)

    # Callback that handles TX calibration
    def calibrate_tx(self):
        adc:list[str] = []
        olvl:list[str] = []
        for i in range(0, len(DAC), 1):
            adc.append(self.adc[i].text())
            olvl.append(int(float(self.olvl[i].text())*10).to_bytes(length=2, byteorder='big', signed=True).hex())
            if adc[i] is None or olvl[i] is None: 
                logger.error(f"Unable to calibrate TX: Missing values.")
                return
        cali_cmd = MCU_CMD['cal'].format(self.gpfe.chain, olvl[0], adc[0], olvl[1], adc[1], olvl[2], adc[2], olvl[3], adc[3], olvl[4], adc[4], olvl[5], adc[5], olvl[6], adc[6], olvl[7], adc[7], olvl[8], adc[8], olvl[9], adc[9], olvl[10], adc[10])
        logger.debug(f"Calibrating TX: {cali_cmd}")
        self.gpfe.send_mcu_msg(cali_cmd)
        logger.info(f"Calibrated TX!")
        # Disable calibrate button
        self.ui.btn_cali.setEnabled(False)

    # Callback that handles resetting TC calibration on MCU
    def reset_tx_cal(self):
        logger.warning("Are you sure you want to reset the TX calibration table in the MCU?")
        warnbox:QMessageBox.StandardButton = QMessageBox.warning(self.centralWidget(), "Confirm TX calibration reset", "Resetting TX calibrations in MCU.\nAre you sure?", buttons=QMessageBox.StandardButton.Ok | QMessageBox.StandardButton.Cancel, defaultButton=QMessageBox.StandardButton.Cancel)
        if warnbox == QMessageBox.StandardButton.Ok:
            self.gpfe.send_mcu_msg(MCU_CMD['resetcal'].format(self.gpfe.chain))
            logger.info("Calibration TX reset")
        else:
            logger.info("Calibration TX reset cancelled")

    # Callback that handles getting RX ADC value
    def get_rx_adc(self):
        setpts = []
        for i in range(0, 5):
            # Read setpoint value
            response = self.gpfe.send_mcu_msg(MCU_CMD['getadc'].format(self.gpfe.chain, RX))
            if response != None:
                setpts.append(self.gpfe.get_values(response.decode(), 'fe_adc', 3))
                logger.debug(f"adc[{i}] {setpts[i]}")
            time.sleep(0.1)
        # Calculate mean setpoint from last three setpoints
        setpt_mean = sum([int.from_bytes(bytes.fromhex(a), byteorder='big', signed=True) for a in setpts[-3:]])//len(setpts[-3:]) # Convert to integers first
        setpt_mean = setpt_mean.to_bytes(length=2, byteorder='big', signed=True).hex() # Convert back to hex
        logger.debug(f"adc[mean] {setpt_mean}")
        # Set setpoint field to mean value
        self.ui.line_setpt.setText(setpt_mean)
        self.ui.btn_readsetpt.setEnabled(False)
        return setpt_mean

    # Callback that handles clearing the RX ADC field
    def clear_rx_adc(self):
        self.ui.btn_readsetpt.setEnabled(True)
        self.ui.line_setpt.clear()

    # Callback that handles setting the default RX setpoint
    def set_default_rx_setpoint(self):
        # Exit if setpoint is empty
        if self.ui.line_setpt.text() == '': 
            logger.warning(f"Unable to set setpoint with default offset: Read setpoint first.")
            return
        # Set setpoint
        logger.info(f"Setting RX setpoint to {self.ui.line_setpt.text()} with default offset {self.ui.line_defoffset.text()}")
        self.gpfe.send_mcu_msg(MCU_CMD['setagcpar'].format(self.gpfe.chain, RX, str(int(float(self.ui.line_defoffset.text())*10)), str(int.from_bytes(bytes.fromhex(self.ui.line_setpt.text()), 'big', signed=False)), self.ui.line_timeconst.text()))
        logger.warning("RX setpoint changed: Restart the GPFE, start the chain and reconnect the calibration tool to proceed with the calibration!")
        # Enable AGC again if disabled
        if not self.mcu.agc:
            self.set_agc()
        # Close the GPFE connection
        self.connect_to_gpfe()
        # Show message box stating that the user should restart the GPFE and reconnect the calibration tool
        QMessageBox.warning(self.centralWidget(), "RX setpoint and offset changed", "Perform the following steps: \n1. Restart the GPFE.\n2. Start the chain GUI.\n3. Reconnect the calibration tool with the GPFE.\n4. Continue with next calibration step.")

    # Callback that updates another field when text changes of an offset field
    @Slot(QMainWindow)
    def update_offset_field(self):
        sender:QLineEdit = self.sender()
        logger.debug(f"{sender.objectName()} has changed to {sender.text()}")
        # Check who emitted the signal and check necessary fields before updating
        if sender == self.ui.line_carlvl1 and self.ui.line_carlvl1.text() != '' and self.ui.line_injsig1.text() != '':
            self.ui.line_difoffset1.setText(str(-float(self.ui.line_injsig1.text()) + float(self.ui.line_carlvl1.text())))
        elif sender == self.ui.line_carlvl2 and self.ui.line_carlvl2.text() != '' and self.ui.line_injsig2.text() != '':
            self.ui.line_difoffset2.setText(str(-float(self.ui.line_injsig2.text()) + float(self.ui.line_carlvl2.text())))
        if sender == self.ui.line_difoffset1 and self.ui.line_difoffset1.text() != '' and self.ui.line_difoffset2.text() != '':
            self.ui.line_meandiff.setText(str((float(self.ui.line_difoffset1.text()) + float(self.ui.line_difoffset2.text()))/2))
        elif sender == self.ui.line_difoffset2 and self.ui.line_difoffset1.text() != '' and self.ui.line_difoffset2.text() != '':
            self.ui.line_meandiff.setText(str((float(self.ui.line_difoffset1.text()) + float(self.ui.line_difoffset2.text()))/2))
        elif sender == self.ui.line_meandiff and self.ui.line_meandiff.text() != '':
            self.ui.line_offset.setText(str(float(self.ui.line_defoffset.text()) + float(self.ui.line_meandiff.text())))

    # Callback that handles RX calibration
    def calibrate_rx(self):
        # Exit if any of the fields are empty
        if self.ui.line_setpt.text() == '': 
            logger.warning(f"Unable to calibrate RX: Read ADC and set default offset first before calibration.")
            return
        if self.ui.line_offset.text() == '': 
            logger.warning(f"Unable to calibrate RX: Inject signals and read levels to calculate correct offset for calibration.")
            return
        cali_cmd = MCU_CMD['setagcpar'].format(self.gpfe.chain, RX, str(int(float(self.ui.line_offset.text())*10)), str(int.from_bytes(bytes.fromhex(self.ui.line_setpt.text()), 'big', signed=False)), self.ui.line_timeconst.text())
        logger.debug(f"Calibrating RX: {cali_cmd}")
        self.gpfe.send_mcu_msg(cali_cmd)
        logger.info(f"Calibrated RX!")

    # Open a file dialog to choose file name to save
    def open_savecal_window(self):
        filename, _ = QFileDialog.getSaveFileName(self, 'Save calibration values', f'{str(Path.home())}/calibrations_{datetime.datetime.now().strftime("%y%m%d%H%M")}.json', 'JSON (*.json)')
        if filename != '':
            self.save_calibration_values(filename=filename)
        else:
            logger.error(f"Cancelled saving calibration values.")

    # Save calibration values
    def save_calibration_values(self, filename:str):
        val_to_save:dict = dict()
        ## TX
        save_tx = True
        # Check whether all TX values are filled in
        dac:list[str] = []
        adc:list[str] = []
        olvl:list[str] = []
        for i in range(0, len(DAC), 1):
            dac.append(self.dac[i].text())
            adc.append(self.adc[i].text())
            olvl.append(self.olvl[i].text())
            if dac[i] is None or adc[i] is None or olvl[i] is None: 
                logger.error(f"Unable to save TX calibration values: Missing values.")
                save_tx = False
        if save_tx:
            # Create top level key-val pair for TX values
            val_to_save['TX'] = dict()
            val_to_save['TX']['dac_values'] = dac
            val_to_save['TX']['adc_values'] = adc
            val_to_save['TX']['output_levels'] = olvl
        ## RX
        # Check whether the RX values are filled in (only when RX calibration is enabled)
        if enable_rx:
            save_rx = True
            if self.ui.line_setpt.text() == '': 
                logger.warning(f"Unable to save RX calibration values: Read ADC and set default offset first.")
                save_rx = False
            if self.ui.line_offset.text() == '': 
                logger.warning(f"Unable to save RX calibration values: Inject signals and read levels to calculate correct offset.")
                save_rx = False
            if save_rx:
                # Create top level key-val pair for RX values
                val_to_save['RX'] = dict()
                val_to_save['RX']['adc'] = self.ui.line_setpt.text()
                val_to_save['RX']['timeconstant'] = self.ui.line_timeconst.text()
                val_to_save['RX']['measurement_1'] = dict()
                val_to_save['RX']['measurement_1']['injected_signal'] = self.ui.line_injsig1.text()
                val_to_save['RX']['measurement_1']['carrier_level'] = self.ui.line_carlvl1.text()
                val_to_save['RX']['measurement_2'] = dict()
                val_to_save['RX']['measurement_2']['injected_signal'] = self.ui.line_injsig2.text()
                val_to_save['RX']['measurement_2']['carrier_level'] = self.ui.line_carlvl2.text()
        # Save file, if file exists make a backup and override original
        if os.path.isfile(filename):
            shutil.copy2(filename, f'{filename}.backup{datetime.datetime.now().strftime("%y%m%d%H%M")}')
            os.remove(filename)
        with open(filename, 'x') as savefile:
            json.dump(val_to_save, savefile)
        logger.info(f"Saved calibration values to {filename}!")

    # Open a file dialog to choose file name to load
    def open_loadcal_window(self):
        filename, _ = QFileDialog.getOpenFileName(self, 'Load calibration values', f'{str(Path.home())}', 'JSON (*.json)')
        if filename != '':
            self.load_calibration_values(filename=filename)
        else:
            logger.error(f"Cancelled loading calibration values.")

    # Load calibration values
    def load_calibration_values(self, filename:str):
        val_to_load:dict = dict()
        # Load file
        with open(filename, 'r') as loadfile:
            val_to_load = json.load(loadfile)
        ## TX
        if val_to_load.get('TX'):
            logger.debug(f"Loading TX values..")
            for i in range(0, len(DAC), 1):
                self.dac[i].setText(val_to_load['TX']['dac_values'][i])
                self.adc[i].setText(val_to_load['TX']['adc_values'][i])
                self.olvl[i].setText(val_to_load['TX']['output_levels'][i])
            logger.debug(f"TX values loaded!")
        ## RX
        # Check whether the RX values are filled in (only when RX calibration is enabled)
        if enable_rx and val_to_load.get('RX'):
            # Enable signals and slots for fields
            self.ui.line_carlvl1.textChanged.connect(self.update_offset_field)
            self.ui.line_carlvl2.textChanged.connect(self.update_offset_field)
            logger.debug(f"Loading RX values..")
            self.ui.line_setpt.setText(val_to_load['RX']['adc'])
            self.ui.line_timeconst.setText(val_to_load['RX']['timeconstant'])
            self.ui.line_injsig1.setText(val_to_load['RX']['measurement_1']['injected_signal'])
            self.ui.line_carlvl1.setText(val_to_load['RX']['measurement_1']['carrier_level'])
            self.ui.line_injsig2.setText(val_to_load['RX']['measurement_2']['injected_signal'])
            self.ui.line_carlvl2.setText(val_to_load['RX']['measurement_2']['carrier_level'])
            # Disable signals and slots for fields
            self.ui.line_carlvl1.textChanged.disconnect()
            self.ui.line_carlvl2.textChanged.disconnect()

            logger.debug(f"RX values loaded!")
        logger.info(f"Loaded calibration values from {filename}!")

    # Override close event to do some checks before closing the window
    def closeEvent(self, event:QCloseEvent):
        # Re-enable the AGC before closing
        if self.status.con_gpfe and self.status.con_mcu and not self.status.mcu.agc:
            logger.warning("Closing without enabling the AGC. Do you want to re-enable the AGC?")
            warnbox:QMessageBox.StandardButton = QMessageBox.warning(self.centralWidget(), "Re-enable the AGC", "Do you want to re-enable the AGC?", buttons=QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No, defaultButton=QMessageBox.StandardButton.Yes)
            if warnbox == QMessageBox.StandardButton.Yes:
                self.gpfe.send_mcu_msg(MCU_CMD['agc'].format('enable'))
                logger.info("AGC enabled")
            else:
                logger.info("AGC disabled")
        return super(QMainWindow, self).closeEvent(event)

# Main
if __name__ == "__main__":
    # Catch the SIGINT signal
    signal.signal(signal.SIGINT, sigint_handler)
    # Initialise logger
    logFormatter = logging.Formatter("%(asctime)s [%(levelname)s]  \t%(message)s")
    logPath = os.path.join('logs', f'mcu_calibration_{time.strftime("%Y%m%d_%H%M%S")}.log')
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

    # Timer to run interpreter every 500ms, that way we can catch the exceptions (like CTRL + C)
    timer = QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    # Define and process command line arguments
    parser = QCommandLineParser()
    parser.setApplicationDescription(APP_DESC)
    parser.addHelpOption()
    parser.addVersionOption()
    opt_loglevel = QCommandLineOption(['loglevel'], 'Logging level [debug, info, warning, error, critical]', 'level', 'debug')
    opt_rxcal = QCommandLineOption(['r', 'rx'], 'Enable RX calibration')
    opt_rxcal.setHidden(True)
    parser.addOption(opt_loglevel)
    parser.addOption(opt_rxcal)
    parser.process(app)
    # Setting log level
    log_level = None
    if parser.isSet(opt_loglevel):
        log_level = parser.value(opt_loglevel).upper()
        if log_level not in ['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL']:
            print(f"{APP_NAME}: Invalid log level '{log_level}' defined in command line.")
            app_exitcode = 126  # Command invoked cannot execute
    if parser.isSet(opt_rxcal):
        enable_rx = True

    # If no exitcodes generated start mainwindow
    wnd_main = None
    if app_exitcode is 0:
        # Create and show mainwindow
        wnd_main = MainWindow()
        # Set log level
        if log_level != None:
            logger.setLevel(log_level)                    
            # Skip debug message for font manager
            logging.getLogger('matplotlib.font_manager').setLevel(logging.INFO)
            print(f"{APP_NAME}: Log level set to '{log_level}'.")
        wnd_main.setMinimumSize(wnd_main.width(), wnd_main.height())
        wnd_main.show()
        app_exitcode = app.exec_()

    # Exit application
    # Close the gpfe connections
    wnd_main.gpfe.close()
    logger.info(f"Shutting down (exitcode={app_exitcode}).")
    sys.exit(app_exitcode)
