#!/bin/bash
set -x
#should run as root 
TRACE=$1
#Blocklayer Queue Size List
BQSL="4096"
#Device Driver Queue Depth List
DQSL="4096"
#Bunch Size
BS="4"
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


#target device
SD=sdd

#rarget Trace device
SDTRACE=sdb

#tarfet Stat device
SDSTAT=sdc

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
  #Generate new name 
  newName=$(echo -e "BtReplay-B_${bqs}-D_${dqs}-BS_${bs}-BT_${bt}-$replayer-NI_$niceness-RC_$repcore-IC_$irqcore")
  NamesList=$(echo -e "$NamesList $newName")
  mkdir $newName
  cd $newName
  blktrace -d /dev/$SD &
  cd ..

  ./runBtReplay $SD  $replayer $niceness

  if [ "$?" -ne "0" ]; then
    echo "Can not run runReplay"
    kill -SIGINT %1
    exit 100
  fi
    kill -SIGINT %1
  #process replayed trace for fidelity check 
  mv $ReplayTrace  $newName/$newName
  if [ "$?" -ne "0" ]; then
    echo "Can not mv $ReplayTrace to $newName"
    exit 100
  fi
  echo -e ", $(cat avg)" >>AvgFlightList
  mv avg.log $newName/${newName}.HostLoad
  if [ "$?" -ne "0" ]; then
    echo "Can not mv avg.log"
    exit 100
  fi
  mv sched $newName/${newName}.sched
  mv schedstat $newName/${newName}.schedstat
  if [ "$?" -ne "0" ]; then
    echo "Can not mv sched"
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
        for replayer in $REPLAYER
        do
            for niceness in $NICENESS
            do
                for irqcore in $IRQCORE
                do
                    for repcore in $REPCORE
                    do
                        for dqs in $DQSL
                        do
                            for bqs in $BQSL
                            do
                                    LoopBody
                                    sleep 60
                            done
                        done
                    done
                done
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


