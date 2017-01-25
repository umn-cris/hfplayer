#!/bin/bash
#This script generates an IO pattern, captures a trace of it, and then
#replays it an captures the replay trace also.

# Clean up any old files
set -x #show executed command in stdout
rm -f fioTrace*.*
rm -f ReplayfioTrace$1*.*
	
./stop7273
sleep 5
./start7273
sleep 5
fio four-thread.fio
sleep 5
./stop7273
sleep 5
./copydump.pl /dev/sdk fioTrace$1.raw
./copydump.pl /dev/sdl fioStats$1.raw
./parsedump   fioTrace$1.raw  


#./start7273
##sleep 5
#
#./hfplayer -nt 1 -cfg ScaleCfg2-sdp.csv 50uShortReadTrace.csv
##sleep 5
#
#./stop7273
#
#./copydump.pl /dev/sdk Test50Trace$1.raw
#./copydump.pl /dev/sdl Test50Stats$1.raw
#./parsedump   Test50Trace$1.raw  
#
	exit

