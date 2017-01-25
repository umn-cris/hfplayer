#!/bin/bash
x=$2
while [ $x -ne 0 ]
do
        cat /proc/interrupts | grep 'ata_piix\|mpt2sas\|qla2xxx\|em1-\|LOC:\|NMI:' >> $1
        x=$(( $x - 1 ))
        sleep 5
        echo "-----------------------------------------------------------------------------------" >> $1
done
