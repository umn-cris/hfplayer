#!/usr/bin/python
#Objective:
	#Take a raw trace file with IN and OUT IO entries. 
	#Subtract OUT timestamp from INs to calculate latency per each request. 
	#Print new trace file with INs only and fill the latency part 
#Usage :
#	./raw_trace_parser.py trace_file_name
#Algorithm:
	#Read input RAW trace file line by line:
	#Put all lines in a ordered dict, sorted by arrival time of request
	# read the ordered dict line by line
	#Line is IN request, add the line into a list ; increament queue depth and correct the In flight I/O value of the line 
	#Line is OUT request, add the line with Sequence ID as a key and the elapsed usec time as value in a key-value dictionary ; depcerement queue depth 
	#Process trace file:
	#read an IN request from the list 
	#pick the sequence ID of IN request
	#search sequence ID in the dictonary
	#get the elapsed used of corresponding out request from dic
	#calculate latency
	#fill latency in IN request line
	#Print the new line with latency info into consule 


import sys

_in_list = []
_out_dict = {}
_error_count = 0
_processed_count = 0 
_lat_col = 19
_seqID_col = 7 
_time_col = 2 
_queue_col = 5
_skip_io = 0

def prepare_ds(trace_file_name):
	global _in_list
	global _out_dict
	global _error_count
	_in_list = []
	_out_dict = {}
	ordered_dict = {}
	queueDepth = 0 
	#import pdb; pdb.set_trace()
	f = open(trace_file_name)
	for line in f: 
		if "_IN" in line or "_OUT" in line : 
			records = line.split(",")
			arrival_time = float( records[_time_col])
			ordered_dict[arrival_time] = line 

	for key in sorted(ordered_dict.iterkeys()):
		line = ordered_dict[key] 
		if "_IN" in line: 
			queueDepth += 1 
			records = line.split(",") 
			records[_queue_col] = str(queueDepth)
			new_line = ",".join(records)
			_in_list.append(new_line)
		elif "_OUT" in line: 
			if( queueDepth > 0 ):
				queueDepth -= 1 
			records = line.split(",")
			_out_dict[records[_seqID_col]] = records[_time_col]
		
def process_ds(trace_file_name):
	global _in_list
	global _out_dict
	global _error_count 
	global _processed_count
	global _skip_io

	f = open(trace_file_name)
	header = f.readline()
	#firstRecord = f.readline()
	f.close()
	
	#firstLineStrs = firstRecord.split(",")
	#startTime = float(firstLineStrs[_time_col])
	startTime = 0 
	out_trace = open(trace_file_name+".inout","w")
	out_trace.write(header)
	#import pdb; pdb.set_trace()
	for inIO in _in_list: 
		if( _skip_io > 0 ):
			_skip_io -= 1
			continue 
		elif( _skip_io == 0):
			in_records = inIO.split(",")
			startTime = float(in_records[_time_col])
			_skip_io -= 1 

		in_records = inIO.split(",")
		out_time_str = _out_dict.get(in_records[_seqID_col])
		if(out_time_str != None	):
			out_time = float(out_time_str)
			in_time = float(in_records[_time_col])
			latency = out_time - in_time 
			if(latency < 0 ):
				#print("negative latency recorded for seqID %s\n"% in_records[_seqID_col])
				_error_count += 1 
			elif(latency == 0 ):
				#print("zeor latency recorded for seqID %s\n"% in_records[_seqID_col])
				_error_count += 1 
			latency_str = "%.3f" % latency
			in_records[_lat_col] =  latency_str
			relative_arrival_time = float(in_records[_time_col]) - startTime 
			relative_arrival_time_str = "%.3f" % relative_arrival_time 
			in_records[_time_col] = relative_arrival_time_str
			new_line = ",".join(in_records)
			out_trace.write(new_line)
			_processed_count += 1
		else:
			#print("Out record for IN seqID %s does not exist\n" % in_records[7] )
			_error_count += 1
	out_trace.close()
		
			
def main(argv):
	#import pdb; pdb.set_trace(); 
	if( len(argv) != 3  ):
		print("input error\n")
		exit(100)
	global _skip_io
	_skip_io = int(argv[2])

	prepare_ds(argv[1]) 
	process_ds(argv[1])
	print("Parsing %s file is done, Processed IOs: %d , Errors: %d"%(argv[1],_processed_count,_error_count) )
	exit(0)
	

if __name__ == "__main__":
	try: 
		main(sys.argv)
	except:
		pass

