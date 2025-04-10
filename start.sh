date "+%H:%M:%S"
echo Copying rpiscgms.srec to /dev/ttyUSB0
stty -F /dev/ttyUSB0 115200
cat build/rpiscgms.srec > /dev/ttyUSB0
date "+%H:%M:%S"
echo Finished, listening
while :
do
cat /dev/ttyUSB0
done
