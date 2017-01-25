#!/bin/bash 
#===============================================================================
#
#          FILE:  draw_all.sh
# 
#         USAGE:  ./draw_all.sh (gnuplot data files)
# 
#   DESCRIPTION:  draw all files with gnuplot
# 
#        AUTHOR: Alireza Haghdoost (arh), haghdoost@gmail.com
#       CREATED: 05/07/2011 06:59:45 PM IRDT
#      REVISION:  1.0
#	reference: http://sharats.me/the-ever-useful-and-neat-subprocess-module.html
#===============================================================================

set -o nounset # Treat unset variables as an errorwdev_0
XLABLE="Trace Line Number"
YLABLE="LBA Jump (log)"

for i in $@
	do
		echo "Plotting $i"
		awk 'BEGIN { FS = "," ; OFS = "," }; {print $11-i; i=$11}' $i | sed '1,2d' >$i.LbaDiff
	#	awk 'BEGIN { FS = "," ; OFS = "," }; {print $11}' $i | sed '1,2d' >$i.LbaDiff
		gnuplot -e " 	set ylabel '$YLABLE';\
				set logscale y;\
                set xlabel '$XLABLE';\
				set ytics nomirror tc lt 1;\
				set autoscale y;\
				set xrange [1:*];\
                set yrange [1:*];\
				set title 'Request LBA Order for $i';\
				set terminal png size 1600,800 font 'Verdana,20';\
				set output '${i}.LBAOrder.png' ;\
				set grid x y;\
				set key off;\
				set lmargin 12;\
				set datafile separator ',' ;\
				plot '${i}.LbaDiff' using 1  with points lt 1 pt 6 ps 0.7 ; "
				#rm ${i}.LbaDiff
done 
#				plot '${i}.LbaDiff' using 1  with points lt 1 pt 6 ps 0.2 ;
				#plot '${i}' using 11  with l ; "
#				set logscale y ;\
#				set xrange [1:*] ;\
#				set yrange [1:$YMAX] ;\
#				set terminal png size 800,1000;\
#				set format x '%g %%';\
#				plot '$i' using 16 axes x1y1 title 'Response Time' with points lt 1 pt 6 ps 0.5 , '$i' using 5 axes x1y2 title 'Controller Load' with points lt 2 pt 6 ps 0.5 ,'${i}.HostLoad' using 1 axes x2y2 title 'Host Load' with points lt 3 pt 6 ps 0.5;
