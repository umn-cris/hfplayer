#!/usr/bin/python
# metrics to claculate:
#	- Avg Latency : accumulate latency value, devide it by number of IOs at the end
#	- Execution Time: take start time, subtract it from last IO time at the end
#	- IOPS: devide number of IOs by execution time 
#	- Avg queue depth: accumulate queue depth for each request, devide it by number of IOs
#	- Type-P-Reordered: https://tools.ietf.org/html/rfc4737#section-4.1.1
#			1. read original trace file, create dic with <LBA> as key and <order> as value
#			2. read replay trace file, for each IO, maintain IO order number locally. read <LBA> and
#				then lookup in the dic for that LBA:
#				if (replay order >= expected order):
#					package received in order
#				else: 
#					reorder += 1 
#	- SequenceDiscontinuty: https://tools.ietf.org/html/rfc4737#section-4.1.1
#			if package received in order:
#				seqDinceDiscontinutySize += replay_order - expected_order 
	

import sys

_lat_col = 19
_seqID_col = 7 
_time_col = 2 
_queue_col = 5
_lba_col = 14


_acc_latency = 0
_start_time = 0 
_end_time = 0 
_num_io = 0
_acc_queue = 0
_reorder = 0
_acc_seq_dist = 0
_orig_order = {}
_compare_with_orig = False 
	
def initialize():
	global _acc_latency
	global _start_time  
	global _end_time 
	global _num_io 
	global _acc_queue 
	global _reorder 
	global _acc_seq_dist
	global _orig_order
	global _compare_with_orig
	
	_acc_latency = 0
	_start_time = 0 
	_end_time = 0 
	_num_io = 0
	_acc_queue = 0
	_reorder = 0
	_acc_seq_dist = 0
	

def prepare_orig( trace_file ) :
	global _orig_order
	f = open(trace_file)
	header = f.readline()
	order = 1
	for line in f : 
		records = line.split(",")
		lba = int(records[_lba_col])
		_orig_order[_lba_col] = order
		order += 1
		
def prepare_ds(trace_file):
	global _acc_latency
	global _start_time  
	global _end_time 
	global _num_io 
	global _acc_queue 
	global _reorder 
	global _acc_seq_dist
	global _orig_order
	order = 1 
	#import pdb; pdb.set_trace()
	f = open(trace_file)
	header = f.readline()
	firstline = f.readline()
	firstlineStrs = firstline.split(",")
	_start_time = float(firstlineStrs[_time_col])
	f.close()
	
	f = open(trace_file)
	header = f.readline()
	
	for line in f : 
		records = line.split(",")
		_acc_latency += float(records[_lat_col])
		_num_io += 1
		_acc_queue += int(records[_queue_col])
		if( _compare_with_orig ):
			lba = int(records[_lba_col] )
			expected_order = _orig_order[lba]
			if(order < expected_order ):
				_reorder+=1
			else: #inorder 
				_acc_seq_dist += (order - expected_order)
			
		io_time = float(records[_time_col])
		if(io_time < _end_time):
			print("Error, IO time is smaller than end_time, timing is wrong")
		else:
			_end_time = io_time
			
		order += 1 
		
		
def print_ds(trace_name):
	f = open("results.csv","a")
	#_acc_latency = 0
	#_start_time = 0 
	#_num_io = 0
	#_acc_queue = 0
	#_reorder = 0
	#_acc_seq_dist = 0
	if ( print_ds.header_written == False ) :
		header = "Trace Name, Execution Time (s), IO Counts, Avg Latency (us), Avg Queue Depth, OoO IOs, OoO IO\%, Avg Seq Distance\n"
		f.write(header)
		print_ds.header_written = True 
		
	
	exec_time = "%.2f" % ( (_end_time - _start_time)/1000000 )
	num_io = str(_num_io)
	avg_latency = "%.2f" % ( _acc_latency / float(_num_io) )
	avg_queue = "%.2f" % ( float(_acc_queue) / float(_num_io) )
	ooo_ios = str(_reorder)
	ooo_ios_prc = "%.2f" % ( float(_reorder) / float(_num_io) )
	avg_seq_dist = "%.2f" % ( float(_acc_seq_dist) / float(_num_io) )
	f.write( 	trace_name + "," 
				+ exec_time + ","
				+ num_io + ","
				+ avg_latency + ","
				+ avg_queue + ","
				+ ooo_ios + ","
				+ ooo_ios_prc + ","
				+ avg_seq_dist 
				+"\n" )
		
def pre_initialize():
	print_ds.header_written = False 
	_orig_order.clear()
	_compare_with_orig = False 
	
def main(argv):
	#import pdb; pdb.set_trace(); 
	global compare_with_orig
	pre_initialize()
	for trace_file in argv[1:]:
		if "orig" in trace_file:
			prepare_orig(trace_file)
			compare_with_orig = True 
	
	for trace_file in argv[1:] :
		initialize()
		prepare_ds(trace_file) 
		print_ds(trace_file)
		
		print("Processing %s file is done, Processed IOs: %d"%(trace_file,_num_io) )
		
		
	exit(0)
	

if __name__ == "__main__":
	main(sys.argv)
