#!/usr/bin/python3
###
#
#                   C E L E S T I A    A N T W E R P   (c)
#
# A test server for the MCU configuration tool
#
# Author:   Magdy Abdel
# Created:  20.10.2022
# Version:  v0.1.0
#
#                                 Changelog
#   v0.1.0 Initial version 
#
##
APP_NAME = 'MCU Tool Test Server'
APP_DESC = 'A test server for the MCU configuration tool'
VERS = '0.1.0'

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('localhost', 5000))
s.listen(1)
client = None
try:
    while True:
        client, addr = s.accept()
        while True:
            client.settimeout(0.1)
            try:
                received = client.recv(4096)
            except socket.error as err:
                continue
            if not received: break
            message = received.hex().upper()
            print(f"Message: {message}")
            if      message == '0100000510':
                client.sendall(bytes.fromhex('01000007001000'))
                client.sendall(bytes.fromhex('010000081001002E'))
                # client.sendall(b''.join([bytes.fromhex('01000007001000'), bytes.fromhex('010000081001002E')]))
            elif    message == '010000050A':
                client.sendall(bytes.fromhex('01000007000A00'))
                client.sendall(bytes.fromhex('0100000D0A0102030405060708'))
                # client.sendall(b''.join([bytes.fromhex('01000007000A00'), bytes.fromhex('0100000D0A0102030405060708')]))
            elif    message == '010000050B':
                client.sendall(bytes.fromhex('01000007000B00'))
                client.sendall(bytes.fromhex('0100000D0B0102030405060708'))
                # client.sendall(b''.join([bytes.fromhex('01000007000B00'), bytes.fromhex('0100000D0B0102030405060708')]))
            elif    message == '0100001107C0A8320AFFFFFF00C0A83201':
                client.sendall(bytes.fromhex('01000007000700'))
            elif    message == '01000007081388':
                client.sendall(bytes.fromhex('01000007000800'))
            else:
                client.sendall(bytes.fromhex(f'0100000700{message[8:10]}02'))
        client.close()
except KeyboardInterrupt as err:
    if client != None: client.close()
    s.close()