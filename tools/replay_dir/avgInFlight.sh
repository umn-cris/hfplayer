#!/bin/bash
# Calculate mean (average) of integer data
#set -x 
if [ "$#" -ne "1" ]; then
    echo "AvgInflight need input SD device name"
    exit 100
fi

# Initialise the variables
n=0     # n being the number of (valid) data provided
sum=0   # sum being the running total of all data
started=0
avg=0
max=0
zeroCount=0
zeroExit=2 #Auto kill threashould
delay=0.005
SD=$1
FPATH=/sys/block/$SD/inflight
echo -e "Collecting average in-flight IO request from $FPATH"
#infinite loop
rm avg.log 2>/dev/null
while :
do
  #x=$(cat $FPATH | awk '{print $1}' ) #Read
      x=$(cat $FPATH | awk '{print $2}' ) #Write
    if [ "$x" -ne "0" ] || [  "$started" -ne "0" ]; then
        started=1
        if [ "$x" -gt "$max" ]; then
        max=$x
        fi
        echo $x >>avg.log
        sum=`expr $sum + $x`
        n=`expr $n + 1`
        avg=`expr $sum / $n `
        echo -e "$avg , $max" >avg
    fi
    if [ "$x" -eq "0" ] && [  "$started" -eq "1" ]; then
        zeroCount=`expr $zeroCount + 1 `
        if [ "$zeroCount" -eq "$zeroExit" ]; then
            exit 0
        fi
    fi
      sleep $delay
done
