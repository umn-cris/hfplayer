#!/bin/bash
#This script generates an IO pattern, captures a trace of it, and then
#replays it an captures the replay trace also.

# Clean up any old files
set -x #show executed command in stdout
	
./stop7273
sleep 5

./start7273
sleep 5

./hfplayer -nt 1 -cfg ScaleCfg2-sdp.csv fioTrace-trace-inout.csv
sleep 5

./stop7273

./copydump.pl /dev/sdk fioReplay$1.raw
./copydump.pl /dev/sdl fioReplay$1.raw
./parsedump   fioReplay$1.raw  

	exit

