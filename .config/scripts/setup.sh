#!/bin/sh
# run this script once from root PIO project's folder
echo 'PIO workspace setup..'
ln -s lib/esp32-toolkit-lib/.config/platformio.ini platformio.ini && \
ln -s lib/esp32-toolkit-lib/.config/.gitmodules .gitmodules && \
ln -s lib/esp32-toolkit-lib/.config/scripts/replace_fs.py replace_fs.py
# echo 'create src dir and link to main source file'
mkdir src && ln -s ../lib/esp32-toolkit-lib/examples/firmware-main.cpp src/main.cpp
echo 'Done!'
