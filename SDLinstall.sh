#!/bin/bash
# Script to download and install SDL library
mkdir ~/SDL
cd ~/SDL
wget www.libsdl.org/release/SDL2-2.0.4.tar.gz
tar -xvf SDL2-2.0.4.tar.gz
cd SDL2-2.0.4
./configure
make
make install
exit 