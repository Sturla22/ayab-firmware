#!/bin/bash
set -e

parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $parent_path/..

./build.sh -w

avrdude -p atmega328p -c arduino -P /dev/ttyACM0 -b115200 -D -Uflash:w:"build/ayab_HW_TEST_uno.hex":i

miniterm -e --raw /dev/ttyACM0 115200
