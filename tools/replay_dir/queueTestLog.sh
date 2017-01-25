#!/bin/bash
set -x
#should run as root 

#Blocklayer Queue Size List
BQSL="128 2048 8192"
#Device Driver Queue Depth List
DQSL="128 2048 8192"
#QLogic FC Queue Size List
QQSL="256"

#target device
SD=sdg

ReplayTrace=Trace-single-thread.aio.fio-trace-inout.csv

function LoopBody(){
  echo "Set quque sizes to $bqs , $dqs , $qqs"
  echo ${bqs} > /sys/block/$SD/queue/nr_requests
  echo ${dqs} > /sys/block/$SD/device/queue_depth
  
  if [ "$?" -ne "0" ]; then
    echo "Can not set queue depth to $dqs"
    exit 100
  fi

  #Generate new name 
  newName=$(echo -e "B_${bqs}-D_${dqs}-Q_${qqs}")
  NamesList=$(echo -e "$NamesList $newName")
  dmesg -c
  
  fio single-thread.aio.fio

  dmesg -d -k -l 7 -c > $newName.Log
  
  echo "---------------------------------------------------------------------------"
}

# start from here
echo ",    AvgBlockInFlight   , MaxBlockInFlight " >AvgFlightList
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

