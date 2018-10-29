#!/bin/sh

set -e
set -u

: ${SIMULATOR:=./build/WildlandFireSimulator}

echo "testing with landscape created by SaFiM landscape generator"
"$SIMULATOR" -l landscape.txt -b fixed_mediumNorthEasternWind.txt -d 80 -r 1 -c
mkdir test1
mv burn_map0.asc burndata0.csv test1

echo "testing with landscape specified by esri grid files"
"$SIMULATOR" -a -b fixed_mediumNorthEasternWind.txt -d 80 -r 1 -c
mkdir test2
mv burn_map0.asc burndata0.csv test1