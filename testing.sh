#!/bin/sh

set -e
set -u

: ${SIMULATOR:=./build/WildlandFireSimulator}

echo "testing with landscape created by SaFiM landscape generator"
"$SIMULATOR" -l landscape.txt -b fixed_mediumNorthernWind.txt -d 80 -r 80 -c

echo "testing with landscape specified by esri grid files"
"$SIMULATOR" -a -b fixed_mediumNorthernWind.txt -d 80 -r 80 -c
