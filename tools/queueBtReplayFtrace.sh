#!/bin/bash
set -x
#should run as root 
TRACE=$1
#Blocklayer Queue Size List
BQSL="4096"
#Device Driver Queue Depth List
DQSL="4096"
#Bunch Size
BS="10"
#Bunch Time
BT="200000000"

#target device
SD=sdd

#rarget Trace device
SDTRACE=sdb

#tarfet Stat device
SDSTAT=sdc

ReplayTrace=$SD-t-A-trace-inout.csv

function LoopBody(){
  echo "Set quque sizes to $bqs , $dqs , $qqs"
  echo ${bqs} > /sys/block/$SD/queue/nr_requests
  echo ${dqs} > /sys/block/$SD/device/queue_depth
  
  ./runBtReplayFtrace $SD 
  

  if [ "$?" -ne "0" ]; then
    echo "Can not run runReplay"
    exit 100
  fi
  #Generate new name 
  newName=$(echo -e "BtReplay-B_${bqs}-D_${dqs}-BS_${bs}_BT_${bt}")
  NamesList=$(echo -e "$NamesList $newName")
  
  #process replayed trace for fidelity check 
  mv $ReplayTrace  $newName
  if [ "$?" -ne "0" ]; then
    echo "Can not mv $ReplayTrace to $newName"
    exit 100
  fi

  #read ftrace tracefile 
  mv /tmp/trace $newName.Ftrace
  if [ "$?" -ne "0" ]; then
    echo "Can not mv /tmp/trace to $newName.Ftrace"
    exit 100
  fi

  #read Hostload
  echo -e ", $(cat avg)" >>AvgFlightList
  mv avg.log ${newName}.HostLoad
  if [ "$?" -ne "0" ]; then
    echo "Can not mv avg.log"
    exit 100
  fi
  echo "---------------------------------------------------------------------------"
}


# start from here

if [ "$1" -ne "1" ] ;  then
    echo " please enter trace file as input" 
    exit 100
fi

echo ",    AvgBlockInFlight   , MaxBlockInFlight " >AvgFlightList
./linuxfixes.sh $SD
for bs in $BS
do 
    for bt in $BT
    do
        ./btrecordG $TRACE $SD.replay.0 $bs $bt > /dev/null  2>&1 
        for dqs in $DQSL
        do
            for bqs in $BQSL
            do
                    LoopBody
        done
    done
done
done
./stdDeviation $NamesList >temp
paste temp AvgFlightList >fioFidelityResults.csv
#./plot.sh $NamesList
./plot_inter_load.sh $NamesList
./plot_inter-arrival.sh $NamesList
./plot_order.sh $NamesList
##set queue sizes to default value 
#echo 128 > /sys/block/$SD/queue/nr_requests
#echo 32 > /sys/block/$SD/device/queue_depth
#modprobe -r qla2xxx
#modprobe qla2xxx ql2xmaxqdepth=32
