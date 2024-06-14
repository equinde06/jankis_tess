#!/bin/bash
cd /home/easymetering/esp-serial/ && sudo ./script_tess.sh
echo "Inicio de Mergue "
source raspi/bin/activate
cd examples/raspberry_example/
mkdir build && cd build
cmake .. && cmake --build .
cd ../../binaries/app/
sudo chmod u+rw merged-flash.bin
echo "Inicio de Flash"
cd ../../raspberry_example/build/
sudo ./raspberry_flasher
cd ..
sudo rm -r build
cd ../binaries/app/
sudo rm merged-flash.bin
deactivate

