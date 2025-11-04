#!/bin/bash
python3 -m PyInstaller -F -y -n mcu_calibration main.py
rm -rf build/*
rm -f mcu_calibration.spec