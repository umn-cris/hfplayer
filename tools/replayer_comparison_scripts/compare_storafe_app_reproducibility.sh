#!/bin/bash

set -x


SDs="sdi sdb sdd"
RESDIR="/root/Orig_Comparison_final"
mkdir $RESDIR 

for TRY in `seq 6 10`; do 
    for SD in $SDs ; do 
        echo -e "\n\n==== Running test on $SD Try $TRY ====\n"
        umount /mnt/fs 
        mount  /dev/$SD /mnt/fs
        ./run_and_capture.sh $SD
        mv test $RESDIR/${SD}_$TRY
    done 
done 


    
