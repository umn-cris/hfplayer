#!/bin/bash
set -v
#should run as root 

#Blocklayer Queue Size List
BQSL="32 64 128 256 512 1024"
#Device Driver Queue Depth List
DQSL="32 64 128 256 512 1024"
#QLogic FC Queue Size List
QQSL="32 64 128 256"

ReplayTrace=Trace-single-thread.aio.fio-trace-inout.csv

function LoopBody(){
  newName=$(echo -e "B_${bqs}-D_${dqs}-Q_${qqs}")
  NamesList=$(echo -e "$NamesList $newName")
}

# start from here
echo ", AvgInFlight" >AvgFlightList
for qqs in $QQSL
do
  for dqs in $DQSL
  do
    for bqs in $BQSL
    do
      LoopBody
    done
  done
done
./stdDeviation $NamesList >temp
paste temp AvgFlightList >fioFidelityResults.csv

