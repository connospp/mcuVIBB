#!/bin/sh
##########################################
#
# Celestia Antwerp
# Project IBB MCU
#
# Copyright (c) Celestia Antwerp
#
# Created on: 27/10/2022
# By: Magdy Abdel
#
##########################################

RPM_TOPDIR="${HOME}/rpm"

# TODO Add check if tools were already build for this changeset, because it takes really long
# (do that by creating a version file in the tool dirs or so)

# Build calibration tool
$(cd tools/calibration ; ./build.sh)
# Build configuration tool
$(cd tools/configuration ; ./build.sh)

# Build rpm with spec file
rpmbuild  --define "_topdir ${RPM_TOPDIR}" -bb install/ibb_mcu.spec