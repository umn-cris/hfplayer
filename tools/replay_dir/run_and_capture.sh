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

RESDIR='test'
SD=$1
QSIZE='2048'
delay=5
SKIP_IO=0
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
    sleep $delay
	time eval $@ >>run.log 2>&1
    sleep 30
	finish_capture 
    remove_small_inout_file 
}
function rename () {
	for i in `ls *$1` ; do 
		mv $i $i.$2
	done
}



# Initialize:
#--------------------------------------------------------#

echo "correct system"
./correct_system.sh $SD >>/dev/null 2>&1

rm tB* >>/dev/null 2>&1
rm tA* >>/dev/null 2>&1

mkdir $RESDIR 
rm -r /mnt/fs/*; sync
IO_CMD="./make_fs_mount.sh $SD"
SKIP_IO=100
run_and_capture $IO_CMD
rename inout mkfs.$SD
cp *.mkfs.$SD $RESDIR

free && sync && echo 3 > /proc/sys/vm/drop_caches && free

IO_CMD="filebench -f filebench_workloads/createfiles.f"
SKIP_IO=100
run_and_capture $IO_CMD
rename inout createfiles.$SD
cp *.createfiles.$SD $RESDIR

#rm -r /mnt/fs/*; sync
#free && sync && echo 3 > /proc/sys/vm/drop_caches && free
##./make_fs_mount.sh $SD
#IO_CMD="filebench -f filebench_workloads/mongo.f"
#SKIP_IO=100
#run_and_capture $IO_CMD
#rename inout mongo
#cp *.mongo $RESDIR
#
#
#rm -r /mnt/fs/*; sync
#free && sync && echo 3 > /proc/sys/vm/drop_caches && free
##./make_fs_mount.sh $SD
#IO_CMD="filebench -f filebench_workloads/netsfs.f"
#SKIP_IO=100
#run_and_capture $IO_CMD
#rename inout netsfs
#cp *.netsfs $RESDIR
