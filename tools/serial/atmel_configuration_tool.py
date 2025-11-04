#!/usr/bin/python3
#__author__ = "Arne De Brabanter"
#__email__ = "arne.de_brabanter@celestia-antwerp.be"

import serial
import re

def ishex(s):
    return not re.search(r"[^a-f0-9]", s.lower())



ser = serial.Serial(
    port='/dev/ttyUSB0', 
    baudrate=38400, 
    bytesize=serial.EIGHTBITS, 
    parity=serial.PARITY_NONE, 
    stopbits=serial.STOPBITS_ONE,
    timeout=2
    )

if not(ser.isOpen()):
    ser.open()
    
print ("port is opened!\r\n")
print ('----------------------------------------------------------------------\r\n')
print ('Enter your commands below.\r\nInsert "exit" to leave the application.\r\n')
print ('----------------------------------------------------------------------\r\n')
print('-    type "1": Turn on LEDs\r\n')
print('-    type "2": Turn off all LEDs\r\n')
print('-    type "3": Change IP parameters\r\n')
print('-    type "4": Change TCP parameters\r\n')
print('-    type "5": Change MAC/HW address\r\n')
print('-    type "6": Read Ethernet parameters\r\n')
print('-    type "7": Read serial number 1\r\n')
print('-    type "8": Read serial number 2\r\n')
print('-    type "9": Write serial number 1\r\n')
print('-    type "10": Write serial number 2\r\n')
print ('----------------------------------------------------------------------\r\n')


while(1):
    cmd = input("Command >> ")
    ser.flush()
    if cmd == 'exit':
        ser.close()
        print('exit application')
        exit()
    else:
        if cmd == '1': # Turn on all LEDs
            print('Turn on all LEDs')
            ser.write('l1\r\n'.encode())
            ser.flush()
        elif cmd == '2': # Turn off all LEDs
            print('Turn off all LEDss')
            ser.write('l2\r\n'.encode())
            ser.flush()
        elif cmd == '3': #set the IP parameters of MCU
            # User has to chose an IP address
            print('What IP address needs to be set? E.g. 192.168.50.10')
            ipaddress = input('>> ')
            # User has to chose an subnet address
            print('What subnetmask needs to be set? E.g. 255.255.255.0')
            subnetmask = input('>> ')
            # User has to chose an default gateway address
            print('What default gateway needs to be set? E.g. 192.168.50.1')
            defaultgateway = input('>> ')
            ipparam = [ipaddress, subnetmask, defaultgateway]; 
            #check if all characters are numeric
            ipaddress = ipaddress.replace(".","")
            subnetmask = subnetmask.replace(".","")
            defaultgateway = defaultgateway.replace(".","")
            if ipaddress.isnumeric() and subnetmask.isnumeric() and defaultgateway.isnumeric():
                #double check by user if data is correct
                print('Are you sure the parameters are correct? (Y/N)')
                print('-----------------------------------------------------------')
                print('IP address      |     Subnetaddress     |    Default Gateway')
                print(ipparam[0] + '         ' + ipparam[1]+ '          ' + ipparam[2])
                print('-----------------------------------------------------------')
                doublecheck = input('>> ')
                if doublecheck == 'Y': #confirmation given by user
                    command_send = 'IP '+ ipparam[0] + ' ' + ipparam[1] + ' ' + ipparam[2] + '\r\n'
                    print(command_send)
                    ser.write(command_send.encode())
                    ser.flush()
                    print('Command send')
                else: #confirmation cancelled by user
                    print('Command cancelled')
            else:
                print('only numeric characters to be used! E.g. 192.168.50.10')
        elif cmd == '4': # set TCP port of MCU
            print('What TCP port needs to be set (4 numeric characters)? E.g. 5000')
            tcpport = input('>> ')
            if len(tcpport) == 4 and tcpport.isnumeric() == True: #check if all characters are numeric & characters in total 
                #double check by user if data is correct
                print('Are you sure the TCP port is correct? (Y/N)')
                print('-------------')
                print('TCP port')
                print(tcpport)
                print('-------------')
                doublecheck = input('>> ')
                if doublecheck == 'Y': #confirmation by user
                    command_tcp = 'TCP ' + tcpport + '\r\n'
                    ser.write(command_tcp.encode())
                    ser.flush()
                    print('Command send')
                else: #confirmation cancelled by user
                    print('Command cancelled')
            else: #no 4 numeric characters detected 
                print('4 numeric characters needed!')
        elif cmd == '5': #Set MAC address of MCU
            print('What MAC address needs to be set? E.g. 10:20:30:40:50:60')
            input_MACaddress = input('>> ')
            input_MACaddress = input_MACaddress.replace(":","") #remove the : from the input
            length_MAC = len(input_MACaddress) #get length of MAC address given by user (without ':')
            if length_MAC == 12 and ishex(input_MACaddress): #length of all hexadecimal characters should be 12
                command_MAC = 'MAC ' + input_MACaddress + '\r\n'
                print(command_MAC)
                ser.write(command_MAC.encode())
                ser.flush()
            else: #more or less than 12 hexadecimal characters (without the ':')
                print('Wrong size of MAC address! (12 hexadecimal characters)') 
        elif cmd == '6': #Print all IP TCP parameters
            ser.write('ETH\r\n'.encode('utf-8'))
            ser.flush()
            for i in range(10):
                printeth = ser.readline().decode() #read what MCU is sending
                print(printeth)
        elif cmd == '7': #Read serial number of chain 1 saved in EEPROM
            print('Serial Number of Chain 1:\r\n')
            ser.write('SN1 R\r\n'.encode('utf-8'))
            ser.flush()
            readuart = ser.readline().decode('utf-8')
            print(readuart)
        elif cmd == '8': #Read serial number of chain 2 saved in EEPROM
            print('Serial Number of Chain 2:\r\n')
            ser.write('SN2 R\r\n'.encode('utf-8'))
            ser.flush()
            readuart = ser.readline().decode('utf-8')
            print(readuart)
        elif cmd == '9': #Write serial number of chain 1 which will be saved in EEPROM
            print('Give in a Serial number of Chain 1 of 8 numbers (e.g. 21250001):\r\n')
            SN1 = input('> ')
            if len(SN1) == 8 and SN1.isnumeric(): #check if the serial number is consisting 8 numbers
                command_SN_write = 'SN1 W '+ SN1 +'\r\n'
                ser.write(command_SN_write.encode('utf-8'))
                ser.flush()
                readuart = ser.readline().decode('utf-8')
                print(readuart)
            else:
                print('Serial number should be an 8 numeric value')
        elif cmd == '10': #Write serial number of chain 2 which will be saved in EEPROM
            print('Give in a Serial number of Chain 2 of 8 numbers (e.g. 21250001):\r\n')
            SN2 = input('> ')
            if len(SN2) == 8 and SN2.isnumeric(): #check if the serial number is consisting 8 numbers
                command_SN_write = 'SN2 W '+ SN2 +'\r\n'
                ser.write(command_SN_write.encode('utf-8'))
                ser.flush()
                readuart = ser.readline().decode('utf-8')
                print(readuart)
            else:
                print('Serial number should be an 8 numeric value')
        else:
            print('Unknown Command\r\n')
            print('\r\n')
            print('\r\n')
            print('\r\n')
            print('\r\n')
            print ('----------------------------------------------------------------------\r\n')
            print ('Enter your commands below.\r\nInsert "exit" to leave the application.\r\n')
            print ('----------------------------------------------------------------------\r\n')
            print('-    type "1": Turn on LEDs\r\n')
            print('-    type "2": Turn off all LEDs\r\n')
            print('-    type "3": Change IP parameters\r\n')
            print('-    type "4": Change TCP parameters\r\n')
            print('-    type "5": Change MAC/HW address\r\n')
            print('-    type "6": Read Ethernet parameters\r\n')
            print('-    type "7": Read serial number 1\r\n')
            print('-    type "8": Read serial number 2\r\n')
            print('-    type "9": Write serial number 1\r\n')
            print('-    type "10": Write serial number 2\r\n')
            print ('----------------------------------------------------------------------\r\n')
        print('\r\n')



