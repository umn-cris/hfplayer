#!/bin/bash

APP=$1
PID=`pgrep $APP`
delay=5 

sleep $delay
sleep $delay
sleep $delay
sleep $delay
sleep $delay
sleep $delay
sleep $delay
sleep $delay
sleep $delay

#ps -p $PID -o %CPU  | tail -1 | grep  0.0
top -b -n 1 | head -20 | grep $APP
while [ $? -eq 0 ]; do
    sleep $delay
    #ps -p $PID -o %CPU  | tail -1 | grep  0.0
    top -b -n 1 | head -20 | grep $APP
done 

killall  -9 $APP
