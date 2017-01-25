#!/bin/bash

SD=$1
set -x 
umount /mnt/fs  2>/dev/null 
rm -rf /mn/fs   2>/dev/null 

cmd="wipefs /dev/$SD"
eval $cmd

cmd="mkfs.ext4 -F -E lazy_itable_init=0,lazy_journal_init=0,nodiscard /dev/$SD"
eval $cmd

rm -r /mnt/fs 2> /dev/null

mkdir /mnt/fs
mount /dev/$SD /mnt/fs 





