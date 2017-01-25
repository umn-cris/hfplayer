#!/bin/bash
set -x
#should run as root 

#Blocklayer Queue Size List
BQSL="2048"
#Device Driver Queue Depth List
DQSL="1024"
#QLogic FC Queue Size List
QQSL="256"

#target device
SD=sdd

ReplayTrace=Trace-single-thread.aio.fio-trace-inout.csv

function LoopBody(){
  echo "Set quque sizes to $bqs , $dqs , $qqs"
  echo ${bqs} > /sys/block/$SD/queue/nr_requests
  echo ${dqs} > /sys/block/$SD/device/queue_depth
  
  if [ "$?" -ne "0" ]; then
    echo "Can not set queue depth to $dqs"
    exit 100
  fi
  ./runfio single-thread.aio.fio $SD
  # read inflight avg and kill avgInFlight script
  #Generate new name 
  newName=$(echo -e "B_${bqs}-D_${dqs}-Q_${qqs}")
  NamesList=$(echo -e "$NamesList $newName")
  
  #process replayed trace for fidelity check 
#lines=`expr $(cat $ReplayTrace | wc -l) - 1000`
  lines=`expr $(cat $ReplayTrace | wc -l) - 0`
  head -n $lines $ReplayTrace > $newName
  echo -e ", $(cat avg)" >>AvgFlightList
  mv avg.log ${newName}.HostLoad
  if [ "$?" -ne "0" ]; then
    echo "Can not mv avg.log"
    exit 100
  fi
  echo "---------------------------------------------------------------------------"
}

# start from here
echo ",    AvgBlockInFlight   , MaxBlockInFlight " >AvgFlightList
for qqs in $QQSL
do
  
  ./linuxfixes.sh $SD
#modprobe -r qla2xxx
#  modprobe qla2xxx ql2xmaxqdepth=${qqs}
#  if [ "$?" -ne "0" ]; then
#    echo "Can not set qqs to $qqs"
#    exit 100
#  fi
#  sleep 2
  for dqs in $DQSL
  do
    for bqs in $BQSL
    do
      LoopBody
    done
  done
done


#./stdDeviation $NamesList >temp
#paste temp AvgFlightList >fioFidelityResults.csv
./plot.sh $NamesList
./plot_inter_load.sh $NamesList
./plot_inter-arrival.sh $NamesList


##set queue sizes to default value 
#echo 128 > /sys/block/$SD/queue/nr_requests
#echo 32 > /sys/block/$SD/device/queue_depth
#modprobe -r qla2xxx
#modprobe qla2xxx ql2xmaxqdepth=32
