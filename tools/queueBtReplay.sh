#!/bin/bash
#set -x
#should run as root 
TRACE=$1
#Blocklayer Queue Size List
BQSL="4096"
#Device Driver Queue Depth List
DQSL="4096"
#Bunch Size
BS="4"
#Bunch Time
BT="2000000000"
#replayer postfix
REPLAYER="OTHER"
#nice value
NICENESS="00"
#Replay core
REPCORE="4"
#IRQ core
IRQCORE="2"


#target device
SD=sdp

#rarget Trace device
SDTRACE=sdq

#tarfet Stat device
SDSTAT=sdq

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
    exit 100
  fi
  #Generate new name 
  newName=$(echo -e "BtReplay-B_${bqs}-D_${dqs}-BS_${bs}-BT_${bt}-$replayer-NI_$niceness-RC_$repcore-IC_$irqcore")
  NamesList=$(echo -e "$NamesList $newName")
  
  #process replayed trace for fidelity check 
  mv $ReplayTrace  $newName
  if [ "$?" -ne "0" ]; then
    echo "Can not mv $ReplayTrace to $newName"
    exit 100
  fi
  echo -e ", $(cat avg)" >>AvgFlightList
  mv avg.log ${newName}.HostLoad
  if [ "$?" -ne "0" ]; then
    echo "Can not mv avg.log"
    exit 100
  fi
  mv sched ${newName}.sched
  mv schedstat ${newName}.schedstat
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
                                    sleep 6
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
