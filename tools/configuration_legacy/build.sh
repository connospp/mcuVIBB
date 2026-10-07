#!/bin/bash
pyinstaller -F -y -n mcu_configuration_legacy main.py
rm -rf build/*
rm -f mcu_configuration_legacy.spec