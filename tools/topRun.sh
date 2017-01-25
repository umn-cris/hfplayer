#!/bin/bash
set -x

mkdir results
mkdir results/runComp-1-26
TAR=results/runComp-1-26
rm -rf B_*

logsave $TAR/write.log ./queueTest.sh
mv fioFidelityResults.csv $TAR/write.csv
mkdir $TAR/write
mv B_* $TAR/write

sed -i 's/write/read/g' single-thread.aio.fio 
logsave $TAR/read.log ./queueTest.sh
mv fioFidelityResults.csv $TAR/read.csv
mkdir $TAR/read
mv B_* $TAR/read

sed -i 's/read/randwrite/g' single-thread.aio.fio 
logsave $TAR/randwrite.log ./queueTest.sh
mv fioFidelityResults.csv $TAR/randwrite.csv
mkdir $TAR/randwrite
mv B_* $TAR/randwrite


sed -i 's/randwrite/randread/g' single-thread.aio.fio 
logsave $TAR/randread.log ./queueTest.sh
mv fioFidelityResults.csv $TAR/randread.csv
mkdir $TAR/randread
mv B_* $TAR/randread
