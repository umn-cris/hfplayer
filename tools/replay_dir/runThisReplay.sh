#!/bin/bash
#This script generates an IO pattern, captures a trace of it, and then
#replays it an captures the replay trace also.

# Clean up any old files
set -x #show executed command in stdout

	
./StartStopNetIP1IP2 -stop # >>/dev/null 2>&1
#sleep 20
./StartStopNetIP1IP2 -start # >>/dev/null 2>&1
#./hfplayer -dep  -nt 1  -cfg ReplayCfg.csv TPCC-Dep-Test/TPCC-150vu-200q-SSD-2.inout.100K.annot
./hfplayer  -nt 1  -cfg ReplayCfg.csv TPCC-Dep-Test/TPCC-150vu-200q-NS-2.inout.100K.annot
sleep 2
./StartStopNetIP1IP2 -finish t- s- #>>/dev/null 2>&1

exit 0

