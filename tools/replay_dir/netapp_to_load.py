#!/usr/bin/python
#Objective:
	#Take netapp inout trace format . 
	#convert it to .load format (blkreplay standard trace format)
#Usage :
#	./netapp_to_load.py netapp_trace_file_name

import sys

_in_list = []
_out_dict = {}
_error_count = 0
_processed_count = 0 
_lat_col = 19
_seqID_col = 7 
_time_col = 2 
_lba_col = _lat_col - 5
_reqSize_col = _lat_col - 4

def prepare_ds(trace_file_name):
	global _in_list
	global _out_dict
	global _error_count
	_in_list = []
	_out_dict = {}
	f = open(trace_file_name)
	for line in f: 
		if "_IN" in line: 
			_in_list.append(line)
		elif "_OUT" in line: 
			records = line.split(",")
			_out_dict[records[_seqID_col]] = records[_time_col]
		
def process_ds(trace_file_name):
	global _in_list
	global _out_dict
	global _error_count 
	global _processed_count
	

	out_trace = open(trace_file_name+".load","w")
	for inIO in _in_list: 
		in_records = inIO.split(",")
		time_str = in_records[_time_col]
		time_usec = float( time_str) 
		time_str = "%.9f" % (time_usec / 1000000)
		sector = in_records[ _lba_col ]
		size = in_records[ _reqSize_col ]
		rw = ''
		if "RD_IN" in inIO:
			rw = "R"
		elif "WR_IN" in inIO: 
			rw = "W"	

		out_record =  time_str +" ; "+ sector + " ; " + size + " ; " + rw + " ; 0.0 ; 0.0 \n"
		out_trace.write( out_record )
	out_trace.close()
		
			
def main(argv):
	if( len(argv) != 2  ):
		print("input error\n")
		exit(100)
	prepare_ds(argv[1]) 
	process_ds(argv[1])
	print("Parsing %s file is done, Processed IOs: %d , Errors: %d"%(argv[1],_processed_count,_error_count) )
	exit(0)
	

if __name__ == "__main__":
    main(sys.argv)
