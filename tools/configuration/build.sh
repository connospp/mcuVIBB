#!/bin/bash
python3 -m PyInstaller -F -y -n mcu_configuration main.py
rm -rf build/*
rm -f mcu_configuration.spec