#!/bin/bash
set -v
#should run as root 

#Blocklayer Queue Size List
BQSL="32 64 128"
#Device Driver Queue Depth List
DQSL="32 64 128"
#QLogic FC Queue Size List
QQSL="32 64 128"

ReplayTrace=Trace-scrub-trace-200K-50us.csv-trace-inout.csv

function LoopBody(){
  echo "Set quque sizes to $bqs , $dqs , $qqs"
  ./linuxfixes.sh
  echo ${bqs} > /sys/block/sdp/queue/nr_requests
  echo ${dqs} > /sys/block/sdp/device/queue_depth
  
  ./avgInFlight.sh &
  ./runReplay scrub-trace-200K-50us.csv
  # read inflight avg and kill avgInFlight script
  kill -9 %1  
  #Generate new name 
  newName=$(echo -e "B_${bqs}-D_${dqs}-Q_${qqs}")
  NamesList=$(echo -e "$NamesList $newName")
  
  #process replayed trace for fidelity check 
  lines=200001
  head -n $lines $ReplayTrace > $newName
  echo -e "$newName, $(cat avg)" >>AvgFlightList
  echo "-----"
}

# start from here
echo "TraceName, AvgInFlight" >AvgFlightList
for qqs in $QQSL
do

  modprobe -r qla2xxx
  modprobe qla2xxx ql2xmaxqdepth=${qqs}
  if [ "$?" -ne "0" ]; then
    echo "Can not set qqs to $qqs"
    exit
  fi
  
  for dqs in $DQSL
  do
    for bqs in $BQSL
    do
      LoopBody
    done
  done
done
./stdDeviation -o scrub-trace-200K-50us.csv $NamesList >fioFidelityResults.csv


#set queue sizes to default value 
echo 128 > /sys/block/sdp/queue/nr_requests
echo 32 > /sys/block/sdp/device/queue_depth
modprobe -r qla2xxx
modprobe qla2xxx ql2xmaxqdepth=32
