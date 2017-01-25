#!/bin/bash
set -x
#should run as root 
Trace=$1
#Max in-flight IO in hfplayer
MAXIN="4096"
#Number of Thread
TNO="1 2 4 8"
#Blocklayer Queue Size List
BQSL="4096"
#Device Driver Queue Depth List
DQSL="4096"
#QLogic FC Queue Size List
QQSL="256"
#IAT time
IAT="0 5000 10000 10000"

#target device
SD=sdt

#rarget Trace device
SDTRACE=sdr

#tarfet Stat device
SDSTAT=sds

ReplayTrace=$1-t-A-trace-inout.csv

function LoopBody(){
  echo "Set quque sizes to $bqs , $dqs , $qqs"
  echo ${bqs} > /sys/block/$SD/queue/nr_requests
  echo ${dqs} > /sys/block/$SD/device/queue_depth
  
  if [ "$?" -ne "0" ]; then
    echo "Can not set queue depth to $dqs"
    exit 100
  fi

 #Generate new name 
  newName=$(echo -e "B_${bqs}-D_${dqs}-T_${tno}-I_${maxin}-IAT_${iat}")
  NamesList=$(echo -e "$NamesList $newName")
  rm -r $newName
  mkdir $newName
  cd $newName
  blktrace -d  /dev/$SD &
  cd ..
  echo "Run Replay Trace $Trace on $SD with $tno threads and $maxin in-flight"
  ./runReplay $tno $maxin $Trace $SD $SDTRACE $SDSTAT $iat
  

  if [ "$?" -ne "0" ]; then
    echo "Can not run runReplay"
    #kill blktrace
    kill -SIGINT %1
    exit 100
  fi
  #kill blktrace
  kill -SIGINT %1
  #make sure blktrace is dead
  killall -s SIGINT blktrace
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
  echo "---------------------------------------------------------------------------"
}


# start from here
if [ "$#" -ne "1" ] ; then
	echo "input original trace file name as an input"
	exit 100
fi

echo ",    AvgBlockInFlight   , MaxBlockInFlight " >AvgFlightList
for qqs in $QQSL
do
  
#modprobe -r qla2xxx
#  modprobe qla2xxx ql2xmaxqdepth=${qqs}
#  if [ "$?" -ne "0" ]; then
#    echo "Can not set qqs to $qqs"
#    exit 100
#  fi
#  sleep 2
  ./linuxfixes.sh $SD
  for dqs in $DQSL
  do
    for bqs in $BQSL
    do
		for tno in $TNO
		do 
			for maxin in $MAXIN
			do
                for iat in $IAT
                    do
                        LoopBody
                        sleep 40
                    done
			done
		done
    done
  done
done
#./stdDeviation $NamesList >temp
#paste temp AvgFlightList >fioFidelityResults.csv
##./plot.sh $NamesList
#./plot_inter_load.sh $NamesList
#./plot_inter-arrival.sh $NamesList
#./plot_order.sh $NamesList
##set queue sizes to default value 
#echo 128 > /sys/block/$SD/queue/nr_requests
#echo 32 > /sys/block/$SD/device/queue_depth
#modprobe -r qla2xxx
#modprobe qla2xxx ql2xmaxqdepth=32
