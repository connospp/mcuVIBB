#!/usr/bin/python3
###
#
#                   C E L E S T I A    A N T W E R P   (c)
#
# MCU upgrade tool
#
# Author:   Magdy Abdel
# Created:  25.10.2022
# Version:  v0.1.0
#
#                                 Changelog
#   v0.1.0 Initial version 
#
##
APP_NAME = 'MCU upgrade tool'
APP_DESC = 'Upgrade MCU firmware'
VERS = '0.1.0'

import os
import sys
import logging
import argparse
import time
import subprocess  # For executing a shell command

RPM_NAME = 'ibb_mcu'

logger = logging.getLogger()
logger.setLevel(logging.INFO)

if __name__ == "__main__":
    # Initialise logger
    logFormatter = logging.Formatter("%(asctime)s [%(levelname)s]  \t%(message)s")
    logPath = os.path.join('/tmp',f'mcu_upgrade_{time.strftime("%Y%m%d_%H%M%S")}.log')
    fileHandler = logging.FileHandler(logPath, mode='w')
    fileHandler.setFormatter(logFormatter)
    logger.addHandler(fileHandler)
    consoleHandler = logging.StreamHandler()
    consoleHandler.setFormatter(logFormatter)
    logger.addHandler(consoleHandler)

    print(f"== {APP_NAME} v{VERS} ==")

    # Instantiate the command line argument parser
    parser = argparse.ArgumentParser(description=APP_DESC)
    # Required positional argument
    parser.add_argument('host', help='MCU IP address')
    parser.add_argument('port', help='MCU port', type=int)
    # Parse command line arguments
    args = parser.parse_args()
    # Print some information
    for k, v in vars(args).items(): logger.debug(f" {k}={v}")
    # Used for typing
    host:str = args.host
    port:int = args.port

    # Command to get RPM version
    rpm_vers_cmd = ['rpm', '-q', '--queryformat', '%{VERSION}', RPM_NAME]
    logger.debug(f"Command: {' '.join(rpm_vers_cmd)}")
    # Get RPM version
    rpm_vers = subprocess.check_output(rpm_vers_cmd).decode().split('.')
    logger.debug(f"Version: {rpm_vers} (cmd: {' '.join(rpm_vers_cmd)})")
    # Command to get RPM release (changeset number, with +unreleased if uncommitted changes)
    rpm_rel_cmd = ['rpm', '-q', '--queryformat', '%{RELEASE}', RPM_NAME]
    logger.debug(f"Command: {' '.join(rpm_rel_cmd)}")
    # Get RPM release
    rpm_rel = subprocess.check_output(rpm_rel_cmd).decode().split('+')
    logger.debug(f"Release: {rpm_rel} (cmd: {' '.join(rpm_rel_cmd)})")

    # Exit and display message when using an unreleased RPM
    if len(rpm_rel) > 1:
        logger.error(f"Using unreleased rpm {''.join(rpm_rel)}. Exiting.")
        # Uncomment following line if you want to exit for unreleased RPMs instead
        # sys.exit(1)

    # Get major version from version
    rpm_vers_major = int(rpm_vers[0])
    # Get changeset from release
    rpm_changeset = int(rpm_rel[0])

    # fwupgr command with version extracted from current rpm
    command = ['fwupgr', '-i', host, '-p', str(port), '-v', f'{rpm_vers_major}.{rpm_changeset}', '-f', f'/usr/local/{RPM_NAME}/IBBE.srec']
    logger.debug(f"Upgrading firmware: {' '.join(command)}")
    # Run fwupgr and return exitcode
    proc = subprocess.run(command)
    if proc.returncode != 0:
        logger.error(f"Failed to upgrade firmware: {proc.stderr}")
    sys.exit(proc.returncode)