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
RESDIR='/root/fidelity_results'
# functions 

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
	rm *raw.csv >>/dev/null 2>&1
	start_capture
	eval $@ >>/dev/null 
	finish_capture 
        remove_small_inout_file 
}
function rename () {
	for i in `ls *$1` ; do 
		mv $i $i.$2
	done
}


function run_all_tests () {
#--------------------------------------------------------#
# A Run IO and capture workload
    SKIP_IO=100
    run_and_capture $@
    rename inout orig
    cp *.orig $RESDIR

    SKIP_IO=0
    echo "--------------------------------------"
    umount /mnt/fs 

#--------------------------------------#
# C  run btreplay  

    rm *.replay.* > /dev/null 2>&1
#Bunch Size
    BS="1"
#Bunch Time
    BT="200000000"

    ./btrecordG *.orig $SD.replay.0  $BS $BT  > /dev/null  2>&1
    cmd="./btreplay -W $SD"
    ./monitor_cpu.sh btreplay >> /dev/null  &
    run_and_capture $cmd
#we got *.inout now
    rename inout btreplay
    mv *.btreplay $RESDIR

    echo "--------------------------------------"
#--------------------------------------#
# B run blkreplay 

    rm *.load  >> /dev/null 2>&1
    ./netapp_to_load.py *.orig 
    BLKTR="1024"
    cmd="cat *.load | blkreplay.exe  --replay-start=0 --replay-end=0 --with-conflicts --threads=$BLKTR --no-overhead /dev/$SD "
    ./monitor_cpu.sh blkreplay.exe >> /dev/null  &
    run_and_capture $cmd
#we got *.inout now
    rename inout blkreplay
    mv *.blkreplay $RESDIR

    echo "--------------------------------------"
#--------------------------------------#
# D  run hfplayer
    cmd="./hfplayer -mode hf -nt 1 -cfg ReplayCfg.csv -q 2048 -b 5000 *.orig"
    ./monitor_cpu.sh hfplayer >> /dev/null  &
    run_and_capture $cmd 
    rename inout hfplayer 
    mv *.hfplayer $RESDIR 
}
# Initialize:
#--------------------------------------------------------#

echo "correct system"
./correct_system.sh $SD >>/dev/null 2>&1

rm tB* >>/dev/null 2>&1
rm tA* >>/dev/null 2>&1
mkdir final_results


#mkdir $RESDIR 
#IO_CMD='./fio fioJobs/four-thread.aio.fio' 
#run_all_tests $IO_CMD 
#mv $RESDIR final_results/fio

#--------------------------------------------------------#
mkdir $RESDIR 
./make_fs_mount.sh $SD
IO_CMD="filebench -f filebench_workloads/copyfiles.f"
run_all_tests $IO_CMD 
mv $RESDIR final_results/copyfiles
#--------------------------------------------------------#
mkdir $RESDIR 
./make_fs_mount.sh $SD
IO_CMD="filebench -f filebench_workloads/webserver.f"
run_all_tests $IO_CMD 
mv $RESDIR final_results/webserver

