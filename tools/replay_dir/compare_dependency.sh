#!/bin/bash
#Objective:
#	A-run the IO command and capture original workload
#	B-run {blkreplay,btreplay,hfplayer} on the same volume and capture replay workload 
#	C-run {blkreplay,btreplay,hfplayer} on the faster volume and capture replay workload 
#	D-Compare the results 


#Usage :
#	/home/alireza/arh_replay_dir/compare_replayer.sh
set -x
#set -e
#General config

QSIZE='2048'
delay=5
SKIP_IO=0
# functions 
RESDIR="/home/alireza/dep_traces"

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
    echo "Workload is done, Waiting for network capture to collect all IOs"
    sleep 30
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
    mv *.btreplay $RESDIR/$TRACE.$BT.btreplay
}

#--------------------------------------#
function run_hfplayer () {
    SD=$1
    TRACE=$2
    BT=$3
    NT=$4
    MODE=$5
    cmd="./hfplayer -mode $MODE -nt $NT -cfg ReplayCfg.csv.sd*$SD -q 2048 -b $BT origs/$TRACE"
    #./monitor_cpu.sh hfplayer >> /dev/null  &
    run_and_capture $cmd 
    rename inout hfplayer 
    mv *.hfplayer $RESDIR/$TRACE.$NT.$BT.hfplayer.$MODE
}
# Initialize:
#--------------------------------------------------------#

SDs="sdg sdc"
mkdir $RESDIR

for SD in $SDs ; do 
    echo "correct system for $SD"
    ./correct_system.sh $SD >>/dev/null 2>&1
done 
    #for TRACE in `cd origs; ls ${SD}.*` ; do 
#for TRACE in `cd origs; ls sdi.fio.*.annot` ; do 
#    for mode in $MODE ; do 
#        echo -e "\n=====---- Start hfplayer $TRACE on $SD with $hfb bundle size  ----======"
#        run_hfplayer sdd $TRACE 0 1 $mode
#    done 
#done
#
mode="dep"
for TRACE in `cd origs; ls sdd.createfiles*.annot` ; do 
        echo -e "\n=====---- Start hfplayer $TRACE on sdi with $mode mode  ----======"
        run_hfplayer sdi $TRACE 0 1 $mode
done
mode="hf"
        run_hfplayer sdi $TRACE 0 1 $mode

#--------------------------------------------------------#
echo "--------------------------------------------------------"
mode="dep"
for TRACE in `cd origs; ls sdd.mkfs*.annot` ; do 
        echo -e "\n=====---- Start hfplayer $TRACE on sdi with $mode mode  ----======"
        #run_hfplayer sdi $TRACE 0 1 $mode
done
mode="hf"
        run_hfplayer sdi $TRACE 0 1 $mode

#--------------------------------------------------------#
echo "--------------------------------------------------------"


mode="dep"
for TRACE in `cd origs; ls sdd.netsfs*.annot` ; do 
        echo -e "\n=====---- Start hfplayer $TRACE on sdi with $mode mode  ----======"
        #run_hfplayer sdi $TRACE 0 1 $mode
done
mode="hf"
        run_hfplayer sdi $TRACE 0 1 $mode

#--------------------------------------------------------#
echo "--------------------------------------------------------"


mode="dep"
for TRACE in `cd origs; ls sdi.copyfiles*.annot` ; do 
        echo -e "\n=====---- Start hfplayer $TRACE on sdd with $mode mode  ----======"
        run_hfplayer sdd $TRACE 0 1 $mode
done
mode="hf"
        run_hfplayer sdd $TRACE 0 1 $mode

#--------------------------------------------------------#
echo "--------------------------------------------------------"


mode="dep"
for TRACE in `cd origs; ls sdi.createfiles*.annot` ; do 
        echo -e "\n=====---- Start hfplayer $TRACE on sdd with $mode mode  ----======"
        run_hfplayer sdd $TRACE 0 1 $mode
done
mode="hf"
        run_hfplayer sdd $TRACE 0 1 $mode

#--------------------------------------------------------#
echo "--------------------------------------------------------"

mode="dep"
for TRACE in `cd origs; ls sdi.mkfs*.annot` ; do 
        echo -e "\n=====---- Start hfplayer $TRACE on sdd with $mode mode  ----======"
        #run_hfplayer sdd $TRACE 0 1 $mode
done
mode="hf"
        run_hfplayer sdd $TRACE 0 1 $mode

mode="load"
        run_hfplayer sdd $TRACE 0 1 $mode

#--------------------------------------------------------#
echo "--------------------------------------------------------"

mode="dep"
for TRACE in `cd origs; ls sdi.netsfs*.annot` ; do 
        echo -e "\n=====---- Start hfplayer $TRACE on sdd with $mode mode  ----======"
        run_hfplayer sdd $TRACE 0 1 $mode
done
mode="hf"
        run_hfplayer sdd $TRACE 0 1 $mode

mode="afap"
        run_hfplayer sdd $TRACE 0 1 $mode
mode="load"
        run_hfplayer sdd $TRACE 0 1 $mode
#--------------------------------------------------------#
echo "--------------------------------------------------------"
mode="dep"
for TRACE in `cd origs; ls sdd.copyfiles*10.100*.annot` ; do 
        echo -e "\n=====---- Start hfplayer $TRACE on sdi with $mode mode  ----======"
        run_hfplayer sdi $TRACE 0 1 $mode
done
mode="hf"
        run_hfplayer sdi $TRACE 0 1 $mode

#--------------------------------------------------------#
echo "--------------------------------------------------------"
