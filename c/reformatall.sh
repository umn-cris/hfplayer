#!/bin/bash
for file in *.c
do
    ./reformat.sh $file
done
for file in *.h
do
    ./reformat.sh $file
done
for file in *.cc
do
    ./reformat.sh $file
done

