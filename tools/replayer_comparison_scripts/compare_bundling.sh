#!/bin/bash
#Objective:
#	A-run the IO command and capture original workload
#	B-run {blkreplay,btreplay,hfplayer} on the same volume and capture replay workload 
#	C-run {blkreplay,btreplay,hfplayer} on the faster volume and capture replay workload 
#	D-Compare the results 


#Usage :
#	/home/alireza/arh_replay_dir/compare_replayer.sh
set -x
#General config

QSIZE='2048'
delay=5
SKIP_IO=0
# functions 
RESDIR="/root/bundling_traces"

function connectWLC () {
    ip=$1
    tracefile=$2
    ./shellCmd 'ioLogger,"net disable"' $ip >>/dev/null 2>&1
    sleep $delay
    ./shellCmd 'ioLogger,"stop"' $ip >>/dev/null 2>&1
    sleep $delay
    ./shellCmd 'ioLogger,"net enable '10005'"' $ip >>/dev/null 2>&1 
    sleep $delay
    ./shellCmd 'ioLogger,"start"' $ip$ip >>/dev/null 2>&1
    sleep $delay
    ssh root@10.113.1.23 "wlcHostClient $ip 10005 $tracefile >> /tmp/mylogfile  2>&1 &"
    ssh root@10.113.1.23 "tail -2 /tmp/mylogfile"
    ssh root@10.113.1.23 "tail -2 /tmp/mylogfile" > mylogfile 
}

function startstop () {
    ip=$1
    tracefile=$2
    #./shellCmd 'ioLogger,"net enable '10005'"' $ip >>/dev/null 2>&1 
    #sleep $delay
    #./shellCmd 'ioLogger,"start"' $ip$ip >>/dev/null 2>&1
    #sleep $delay
    connectWLC $ip $tracefile
}


function start_capture {
    startstop 10.113.1.20 NetTrace1617A.raw
    while grep -qi failed mylogfile ; do
        sleep $delay
        connectWLC 10.113.1.20 NetTrace1617A.raw
    done

    startstop 10.113.1.21 NetTrace1617B.raw
    while grep -qi failed mylogfile ; do
        sleep $delay
        connectWLC 10.113.1.21 NetTrace1617B.raw
    done
}

function finish_capture {
    
	sleep 2
	./StartStopNetIP1IP2 -finish t s #>>/dev/null 2>&1
	sleep 2
	rm *.inout >>/dev/null 2>&1
	echo "Skipping $SKIP_IO io requests" 
	./raw_trace_parser.py tA-trace-raw.csv $SKIP_IO
	./raw_trace_parser.py tB-trace-raw.csv $SKIP_IO
}
function remove_small_inout_file () {
	set -- $(ls -S *.inout)
	echo -e "keeping $1"
	rm $2 >/dev/null 2>&1 
}
function run_and_capture () {
    rm tB* >>/dev/null 2>&1
    rm tA* >>/dev/null 2>&1
	rm *raw.csv >>/dev/null 2>&1
	start_capture
	eval $@ >>/dev/null 
    sleep 5 
	finish_capture 
    remove_small_inout_file 
}
function rename () {
	for i in `ls *$1` ; do 
		mv $i $i.$2
	done
}


function run_btreplay () {
    echo "--------------------------------------"
    rm *.replay.* > /dev/null 2>&1
    SD=$1
    TRACE=$2
#Bunch Size
    BS=$3
#Bunch Time
    BT=$4

    ./btrecordG origs/$TRACE $SD.replay.0  $BS $BT  > /dev/null  2>&1
    cmd="./btreplay -W $SD"
    #./monitor_cpu.sh btreplay >> /dev/null  &
    run_and_capture $cmd
#we got *.inout now
    rename inout btreplay
    mv *.btreplay $RESDIR/$TRACE.$SD.$BS.$BT.btreplay
}

#--------------------------------------#
function run_hfplayer () {
    SD=$1
    TRACE=$2
    BT=$3
    NT=$4
    cmd="./hfplayer -mode hf -nt $NT -cfg ReplayCfg.csv.$SD -q 2048 -b $BT origs/$TRACE"
    #./monitor_cpu.sh hfplayer >> /dev/null  &
    run_and_capture $cmd 
    rename inout hfplayer 
    mv *.hfplayer $RESDIR/$TRACE.$SD.$BT.$NT.hfplayer
}
# Initialize:
#--------------------------------------------------------#

SDs="sdi"
mkdir $RESDIR
HFbundles="0 5000 10000 15000 20000"
BTbs="1 5 10 15 20"

for SD in $SDs ; do 
    echo "correct system for $SD"
    ./correct_system.sh $SD >>/dev/null 2>&1
    for TRACE in `cd origs; ls ${SD}.*` ; do 
        #for hfb in $HFbundles ; do 
        #    echo -e "\n=====---- Start hfplayer $TRACE on $SD with $hfb bundle size  ----======"
        #    run_hfplayer $SD $TRACE $hfb 1 
        #done 
        for btb in $BTbs ; do
            echo -e "\n=====---- Start btreplay $TRACE on $SD with $btb max bundle size   ----======"
            run_btreplay $SD $TRACE 10000000 $btb
        done
    done
done 


