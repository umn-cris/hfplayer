#!/bin/bash

set -x

rm /mnt/fs/benchmark.db 

SQ=/home/alireza/sqlite_/bin
$SQ/sqlite3 /mnt/fs/benchmark.db "CREATE TABLE pts1 ('I' SMALLINT NOT NULL, 'DT' TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, 'F1' VARCHAR(4) NOT NULL, 'F2' VARCHAR(16) NOT NULL);"
cat $SQ/sqlite-2500-insertions.txt | $SQ/sqlite3 /mnt/fs/benchmark.db



