****************************************************************************
* This utility tool convert NetApp WLC trace into btreplay trace!
****************************************************************************

1.package content:
    -btrecordG.c
    -btrecord.h
    -readme.txt

2.compile
    compile by "gcc btrecordG.c -o btrecord"

3.run
    ./btrecord <input file> <output file> <bunch size> <bunch time>
    note:
        -<input file> is target WLC trace
        -<output file> should be named something like "sda.replay.0"
            sda is device name
            0   is cpu id
        -<bunch size> is max number of io allowd in a single bunch
        -<bunch time> is max time duration of single bunch, in nanosecond
    example:
        ./btrecord  input.trace sda.replay.0  8  50000

4.output
    sda.btreplay.0      is the real btreplay trace for btreplay input
    sda.btreplay.0.txt  is the btreplay trace in text format 
                        (automatically generated)

5.notes:
   - I'm using WLC trace format
   - I'm assuming the first io has time stamp 0
