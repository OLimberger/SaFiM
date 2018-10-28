
echo "testing with landscape created by SaFiM landscape generator"
./WildlandFireSimulator -l landscape.txt -b fixed_mediumNorthernWind.txt -d 80 -r 80 -c

echo "testing with landscape specified by esri grid files"
./WildlandFireSimulator -a -b fixed_mediumNorthernWind.txt -d 80 -r 80 -c
