#!/usr/bin/python 


#0 DUMP_OFFSET.I,
#1 ELAPSED_USECS.D,
#2 ELAPSED_TICKS.I,
#3 CMD.S,
#4 INFLIGHT_IOS.I,
#5 TS.I,
#6 SEQID.I,
#7 LUN_SSID.I,
#8 OP.I,
#9 PHASE.I,
#10 LBA.I,
#11 NBLKS.I,
#12 LATENCY_TICKS.I,
#13 HOST_ID.I,
#14 HOST_LUN.I,
#15 LATENCY_USECS.D
#0,0.000,0,RD_IN,0,19133702411053028,139753163,3,0,0,29804264,32,11602240,7,3,5438.550

import sys
import csv
import subprocess
import sys
import re

def calc_iops_arr(timestamps , latencies):
	iops_per = 1000000 #usecs 
	pivot = 0 
	iops = []
	iops.append(0)
	#initialize iops array
	for i in range( len(timestamps) ) :
		if( pivot*iops_per <= timestamps[i] ) :
			if( timestamps[i] + latencies[i] < (pivot + 1)*iops_per ):
				iops[pivot]+=1
			else:
				new_pivot = int(timestamps[i]) / iops_per
				for j in range(new_pivot-pivot):
					iops.append(0)
				pivot = new_pivot
				iops[pivot]+=1		
		else:
			old_pivot = int(timestamps[i]) / iops_per
			if( timestamps[i] + latencies[i] < (old_pivot + 1)*iops_per ):
				iops[old_pivot]+=1
	total_io=0;
	for ios in iops:
		total_io+=ios
	return iops

def plot_iops(iops,filename):
	title="Percona Replay Workload on SSD Array"
	ylable="IOPS"
	xlable="Time (s)"
	max_y_tick = '20000'
	y_step = '2000'
	max_x_tick = '*'
	plotcmds=[	"set ylabel '%s'" % ylable,
		"set xlabel '%s'" % xlable,
		"set ytics nomirror tc lt 4",
		"set ytics 0,%s" % y_step,
		"set yrange [0:%s]" % max_y_tick,
		"set xrange [0:%s]" % max_x_tick,
		"set title '%s'" % title,
		"set terminal png font arial 20 size 800,600",
		"set output '%s.png' " % filename,
		"set grid x y",
		#"set lmargin 1",
		#"set rmargin 1",
		"set border lw 2",
		"set tics textcolor rgb 'black'",
		"set nokey",
		"plot '-' using 1 axes x1y1 with l lt 1 lw 3"
	]
	
	proc = subprocess.Popen(['gnuplot','--persist'],
							shell=False,stdin=subprocess.PIPE)
	for cmd in plotcmds:
		#print "running: %s" %cmd
		proc.stdin.write(cmd+'\n')
  #feed data to gnuplot
	for i in range(len(iops)):
		proc.stdin.write(str(iops[i])+'\n')
  #finish feeding gnuplot
	proc.stdin.write('e'+'\n')
  
dumps = sys.argv[1:]
exec_time = [0]
for fn in dumps : 
	f = open( fn ,'rb' )
	trace = csv.reader(f, delimiter=',')
	ts=[] #store timestamps
	lat=[] #store latency
	iterrow = iter(trace)
	next(iterrow)
	for row in iterrow: 
		ts.append( float(row[1]) )
		lat.append( float(row[15]) )
	f.close() #done reading this file 
	iops_arr = calc_iops_arr(ts,lat)
	plot_iops(iops_arr,fn)
	exec_time.append( ts[-1] - ts[0] )
	print fn + ": Exec Time:" + str(exec_time[-1])

