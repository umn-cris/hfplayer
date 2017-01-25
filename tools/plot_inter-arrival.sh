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
XLABLE="IO Offset"
X2LABLE="Time (10ms)"
Y2LABLE="Response Time (us)"
YLABLE="Inter-Arrival Time (us)"

for i in $@
	do
		awk 'BEGIN { FS = "," ; OFS = "," }; {print $1,$2-i,$5,$16; i=$2}' $i | sed '1,2d' >$i.temp
		lines=`expr $(cat $i.temp | wc -l) - 1000`
		head -n $lines $i.temp > $i.arr
		gnuplot -e " 	set ylabel '$YLABLE';\
				set y2label '$Y2LABLE';\
				set xlabel '$XLABLE';\
				set ytics nomirror tc lt 4;\
				set y2tics nomirror tc lt 1;\
				set yrange [0:600];\
				set y2range [1:10000];\
				set xrange [1:*];\
				set title '$i';\
				set terminal png size 1600,800;\
				set output '${i}.png' ;\
				set grid x y2;\
				set key top right outside;\
				set lmargin 12;\
				set rmargin 24;\
				set datafile separator ',' ;\
				plot '${i}.arr' using 4 axes x1y2 title 'Response Time' with points lt 1 pt 6 ps 0.5 , '${i}.arr' using 2 axes x1y1 title 'Inter-Arrival Time' with points lt 4 pt 6 ps 0.5;"
done 
#				set logscale y ;\
#				set xrange [1:*] ;\
#				set yrange [1:$YMAX] ;\
#				set terminal png size 800,1000;\
#				set format x '%g %%';\
#				plot '$i' using 16 axes x1y1 title 'Response Time' with points lt 1 pt 6 ps 0.5 , '$i' using 5 axes x1y2 title 'Controller Load' with points lt 2 pt 6 ps 0.5 ,'${i}.HostLoad' using 1 axes x2y2 title 'Host Load' with points lt 3 pt 6 ps 0.5;
