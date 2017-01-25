#!/usr/bin/python
# metrics to claculate:
#   - Avg Latency : accumulate latency value, devide it by number of IOs at the end
#   - Execution Time: take start time, subtract it from last IO time at the end
#   - IOPS: devide number of IOs by execution time 
#   - Avg queue depth: accumulate queue depth for each request, devide it by number of IOs
#   - Type-P-Reordered: https://tools.ietf.org/html/rfc4737#section-4.1.1
#           1. read original trace file, create dic with <LBA> as key and <order> as value
#           2. read replay trace file, for each IO, maintain IO order number locally. read <LBA> and
#               then lookup in the dic for that LBA:
#               if (replay order >= expected order):
#                   package received in order
#               else: 
#                   reorder += 1 
#   - SequenceDiscontinuty: https://tools.ietf.org/html/rfc4737#section-4.1.1
#           if package received in order:
#               seqDinceDiscontinutySize += replay_order - expected_order 
    

import sys
import pdb, traceback
import copy
_lat_col = 1000
_seqID_col = 1000 
_time_col = 1000
_queue_col = 1000
_lba_col = 1000
_req_size_col = 1000 
_parentCount_col = 1000
_parentList_col = 1000

_seq_range = 0 # sequantility range 

_acc_latency = 0
_start_time = 0 
_end_time = 0 
_num_io = 0
_acc_queue = 0
_reorder = 0
_acc_seq_dist = 0 
_ooo_acc_seq_dist = 0 
_orig_order = {}
_compare_with_orig = False 
_seq_io_count = 0 
_num_read = 0 
_total_parent_counts = 0 

    
def initialize():
    global _acc_latency
    global _start_time  
    global _end_time 
    global _num_io 
    global _acc_queue 
    global _reorder 
    global _acc_seq_dist
    global _seq_io_count
    global _num_read
    global _ooo_acc_seq_dist
    global _total_parent_counts
    
    global _lat_col 
    global _seqID_col 
    global _time_col 
    global _queue_col 
    global _lba_col 
    global _req_size_col 
    global _parentCount_col 
    global _parentList_col 
    
    _acc_latency = 0
    _start_time = 0 
    _end_time = 0 
    _num_io = 0
    _acc_queue = 0
    _reorder = 0
    _acc_seq_dist = 0
    _seq_io_count = 0 
    _num_read = 0 
    _ooo_acc_seq_dist = 0
    _total_parent_counts = 0 
    
    _lat_col = 1000
    _seqID_col = 1000 
    _time_col = 1000
    _queue_col = 1000
    _lba_col = 1000
    _req_size_col = 1000 
    _parentCount_col = 1000
    _parentList_col = 1000

def prepare_orig( trace_file ) :
    global _orig_order
    f = open(trace_file)
    header = f.readline()
    set_schema(header)
    for line in f : 
        records = line.split(",")
        lba = int(records[_lba_col])
        seqID = int(records[_seqID_col])
        order_list = []
        order_list = _orig_order.get(lba,None)
        if( order_list == None ):
            order_list = [seqID]
        else:
            order_list.append(seqID)
        _orig_order[lba] = order_list

def set_schema(header):
    global _lat_col 
    global _seqID_col 
    global _time_col 
    global _queue_col 
    global _lba_col 
    global _req_size_col 
    global _parentCount_col 
    global _parentList_col 
    
    if("#" not in header ):
        print("Cannot read header to set schema for this trace file")
        raise ValueError('Cannot read header to set schema for this trace file')
    
    records = header.split(",")
    
    for i in xrange(0,len(records)) :
        if "LATENCY_USECS" in records[i] : 
            _lat_col = i 
        if "SEQID" in records[i] : 
            _seqID_col = i 
        if "ELAPSED_USECS" in records[i] : 
            _time_col = i 
        if "INFLIGHT_IOS" in records[i] : 
            _queue_col = i 
        if "LBA" in records[i] : 
            _lba_col = i 
        if "NBLKS" in records[i] : 
            _req_size_col = i 
        if "DEP_PARENT_COUNT" in records[i] : 
            _parentCount_col = i 
        if "DEP_PARENT_LIST" in records[i] : 
            _parentList_col = i 
            
def prepare_ds(trace_file, orig_orders):
    global _acc_latency
    global _start_time  
    global _end_time 
    global _num_io 
    global _acc_queue 
    global _reorder 
    global _acc_seq_dist
    global _seq_io_count
    global _seq_range
    global _compare_with_orig
    global _num_read
    global _ooo_acc_seq_dist
    global _total_parent_counts
    
    order = 1 
    last_touched_lba = 0 
    count_parents = False
    f = open(trace_file)
    header = f.readline()
    set_schema(header)
    
    firstline = f.readline()
    firstlineStrs = firstline.split(",")
    _start_time = float(firstlineStrs[_time_col])
    nextSeqID = int(firstlineStrs[_seqID_col])
    f.close()
    
    if("annot" in trace_file[-6:]):
        count_parents = True
    
    f = open(trace_file)
    header = f.readline()
    for line in f : 
        records = line.split(",")
        _acc_latency += float(records[_lat_col])
        _num_io += 1
        _acc_queue += int(records[_queue_col])
        curr_lba = int(records[_lba_col])
        curr_req_size = int(records[_req_size_col]) 

        if( "RD" in line ):
            _num_read += 1
        
        if( last_touched_lba <= curr_lba and last_touched_lba >= curr_lba -  _seq_range and curr_lba > _seq_range ):
            _seq_io_count += 1 
            
        last_touched_lba = curr_lba + curr_req_size 
	try:
		if( _compare_with_orig ):
			lba = int(records[_lba_col] )
			SeqIDlist= orig_orders[lba]
			currSeqID =  SeqIDlist[0]
			if(currSeqID < nextSeqID ):
				_reorder+=1
				_ooo_acc_seq_dist += (nextSeqID -  currSeqID)
			else: #inorder 
				_acc_seq_dist += (currSeqID - nextSeqID)
			
			if( len(SeqIDlist ) > 1 ): 
				orig_orders[lba] = SeqIDlist[1:]
			else:
				del orig_orders[lba]
				
			nextSeqID = currSeqID + 1 
	except:
		print("Error, cannot find lba %s in the orig trce" % records[_lba_col] )
		pass 

        io_time = float(records[_time_col])
        if(io_time < _end_time):
            print("Error, IO time is smaller than end_time, timing is wrong")
        else:
            _end_time = io_time
    if(count_parents):
        parents_list = records[_parentList_col].split("-")
        _total_parent_counts += int(records[_parentCount_col])
        
       # for parent in parents_list : 
            
        
        
            
        
def print_ds(trace_name):
    f = open("results.csv","a")
    #_acc_latency = 0
    #_start_time = 0 
    #_num_io = 0
    #_acc_queue = 0
    #_reorder = 0
    #_acc_seq_dist = 0
    if ( print_ds.header_written == False ) :
        header = "Trace Name, Execution Time (s), IO Counts, IOPS, Avg Latency (us), Avg Queue Depth, Sequantility%, Read%,  OoO IOs, OoO IO%, Avg inorder Seq Dist, Avg OoO Seq Dist, Avg Parent Count\n"
        f.write(header)
        print_ds.header_written = True 
        
    
    exec_time = "%.2f" % ( (_end_time - _start_time)/1000000 )
    num_io = str(_num_io)
    iops = "%d" % (_num_io / ((_end_time - _start_time)/1000000 ) ) 
    avg_latency = "%.2f" % ( _acc_latency / float(_num_io) )
    avg_queue = "%.2f" % ( float(_acc_queue) / float(_num_io) )
    sequantility = "%.4f" % (float(_seq_io_count)/float(_num_io))
    read = "%.4f" % ( float(_num_read)/ float(_num_io) )
    ooo_ios = str(_reorder)
    ooo_ios_prc = "%.4f" % ( float(_reorder) / float(_num_io) )
    avg_seq_dist = "%.2f" % ( float(_acc_seq_dist) / float(_num_io-_reorder) )
    avg_ooo_seq_dist =  "%.2f" % ( float(_ooo_acc_seq_dist) / float(_reorder) ) if _reorder != 0 else '0'
    avg_parent_count = "%.2f" % ( float(_total_parent_counts) / float(_num_io))
    f.write(    trace_name + "," 
                + exec_time + ","
                + num_io + ","
                + iops + ","
                + avg_latency + ","
                + avg_queue + ","
                + sequantility + ","
                + read + ","
                + ooo_ios + ","
                + ooo_ios_prc + ","
                + avg_seq_dist + ","
                + avg_ooo_seq_dist + ","
                + avg_parent_count
                +"\n" )
        
def pre_initialize():
    global _compare_with_orig 

    print_ds.header_written = False 
    _orig_order.clear()
    _compare_with_orig = False 
    
def main(argv):
    #import pdb; pdb.set_trace(); 
    global _compare_with_orig
    global _orig_order
    pre_initialize()

    start_argv = 1
    if( "-o" in argv[1]):
        trace_file = argv[2]
        prepare_orig(trace_file)
        _compare_with_orig = True 
        start_argv = 3

    for trace_file in argv[start_argv:] :
        try:
            initialize()
            orig_orders = copy.deepcopy(_orig_order)
            prepare_ds(trace_file, orig_orders) 
            print_ds(trace_file)
            print("Processing %s file is done, Processed IOs: %d"%(trace_file,_num_io) )
        except:
            print("Error, Processing %s filed after Processing IOs: %d"%(trace_file,_num_io) )
            #typeD, value, tb = sys.exc_info()
            #traceback.print_exc()
            #pdb.post_mortem(tb)
            pass
        
    exit(0)
    

if __name__ == "__main__":
        main(sys.argv)
