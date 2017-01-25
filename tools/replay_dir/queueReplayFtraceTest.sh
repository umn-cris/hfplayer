#!/bin/bash
set -x
#should run as root 
Trace=$1
#Max in-flight IO in hfplayer
MAXIN="8192"
#Number of Thread
TNO="4"
#Blocklayer Queue Size List
BQSL="2048"
#Device Driver Queue Depth List
DQSL="2048"
#QLogic FC Queue Size List
QQSL="256"

#target device
SD=sdf

#rarget Trace device
SDTRACE=sdd

#tarfet Stat device
SDSTAT=sde

ReplayTrace=Trace-${Trace}-trace-inout.csv

function LoopBody(){
  echo "Set quque sizes to $bqs , $dqs , $qqs"
  echo ${bqs} > /sys/block/$SD/queue/nr_requests
  echo ${dqs} > /sys/block/$SD/device/queue_depth
  
  if [ "$?" -ne "0" ]; then
    echo "Can not set queue depth to $dqs"
    exit 100
  fi
#remove old Ftrace
  rm /tmp/trace 
  echo "Run Replay Trace $Trace on $SD with $tno threads and $maxin in-flight"
  ./runReplayFtrace $tno $maxin $Trace $SD $SDTRACE $SDSTAT
  if [ "$?" -ne "0" ]; then
    echo "Can not run runReplay"
    exit 100
  fi

  # read inflight avg and kill avgInFlight script
  #Generate new name 
  newName=$(echo -e "B_${bqs}-D_${dqs}-Q_${qqs}-T_${tno}-I_${maxin}")
  NamesList=$(echo -e "$NamesList $newName")
  mv /tmp/trace $newName.ftrace 
  mv /tmp/stack_trace $newName.stack
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
      				LoopBody
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
##set queue sizes to default value 
#echo 128 > /sys/block/$SD/queue/nr_requests
#echo 32 > /sys/block/$SD/device/queue_depth
#modprobe -r qla2xxx
#modprobe qla2xxx ql2xmaxqdepth=32
