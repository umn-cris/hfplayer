#!/bin/bash
set -x
#should run as root 
TRACE=$1
#target device
SD=$2
#Blocklayer Queue Size List
BQSL="4096"
#Device Driver Queue Depth List
DQSL="4096"
#Bunch Size
BS="1"
#Bunch Time
BT="200000000"
#replayer postfix
REPLAYER="OTHER"
#nice value
NICENESS="0"
#Replay core
REPCORE="4"
#IRQ core
IRQCORE="2"


ReplayTrace=$SD-t-A-trace-inout.csv

function LoopBody(){
  rm $ReplayTrace
  echo "Set quque sizes to $bqs , $dqs , $qqs"
  echo ${bqs} > /sys/block/$SD/queue/nr_requests
  echo ${dqs} > /sys/block/$SD/device/queue_depth

  #set IRQ
  echo $irqcore > /proc/irq/81/smp_affinity
  echo $irqcore > /proc/irq/80/smp_affinity
  #set bunch size
  rm $SD.replay.* > /dev/null  2>&1 
  ./btrecordG $TRACE $SD.replay.$repcore $bs $bt > /dev/null  2>&1 

  ./runBtReplay $SD  $replayer $niceness

  if [ "$?" -ne "0" ]; then
    echo "Can not run runReplay"
    kill -SIGINT %1
    exit 100
  fi
    kill -SIGINT %1
  echo "---------------------------------------------------------------------------"
}


# start from here

if [ "$1" -ne "1" ] ;  then
    echo " please enter trace file as input" 
    exit 100
fi
LoopBody


