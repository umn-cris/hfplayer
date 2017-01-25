#!/usr/bin/python
import sys
import pdb, traceback
import copy
import matplotlib
matplotlib.use('Agg')
import matplotlib.pylab as pl 

pl.ioff()
_plot_col_name = "INFLIGHT_IOS"
_time_col_name = "ELAPSED_USECS"
_plot_time_unit_usec = 1000000

_plot_col = 1000
_time_col = 1000

def set_col_index(header):
    global _plot_col
    global _plot_col_name
    global _time_col
    global _time_col_name
    
    if("#" not in header ):
        print("Cannot read header to set schema for this trace file")
        raise ValueError('Cannot read header to set schema for this trace file')
    records = header.split(",")
    for i in xrange(0,len(records)) :
        if _plot_col_name in records[i] : 
            _plot_col = i
        elif _time_col_name in records[i]:
            _time_col = i 
    
    
def create_plot_date(trace_file):
    global _plot_col
    global _time_col
    
    f = open(trace_file)
    header = f.readline()
    set_col_index(header)
    x = []
    y = []
    curr_time = 0 
    agg_plot_y = []
    plot_x_value = 0 
    for line in f : 
        records = line.split(",")
        curr_time = float(records[_time_col])
        agg_plot_y.append( float(records[_plot_col]) )
        if ( int( curr_time / _plot_time_unit_usec ) > plot_x_value ) :
            x.append(plot_x_value)
            y.append( sum(agg_plot_y) / float(len(agg_plot_y) ) ) 
            agg_plot_y = []
            plot_x_value = int( curr_time / _plot_time_unit_usec )
    if(len(agg_plot_y) != 0 ):
        x.append(plot_x_value)
        y.append( sum(agg_plot_y) / float(len(agg_plot_y) ) ) 
        
    return  x,y
    
  
def main(argv):
    import pdb; pdb.set_trace()
    for trace_file in argv[1:] :
        try:
            x , y = create_plot_date(trace_file)
            pl.plot(x,y)
            pl.savefig(trace_file + ".load.png")
            pl.clf()
        except:
            print("Error, Plotting %s"%(trace_file) )
            #typeD, value, tb = sys.exc_info()
            #traceback.print_exc()
            #pdb.post_mortem(tb)
            pass
        
        
    exit(0)
    

if __name__ == "__main__":
        main(sys.argv)
