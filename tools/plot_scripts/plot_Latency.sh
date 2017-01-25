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
XLABLE="Offset"
YLABLE="Latency"

for i in $@
	do
	
		gnuplot -e " 	set ylabel '$YLABLE';\
				set xlabel '$XLABLE';\
				set title '$i';\
				set terminal png;\
				set output '$i.png' ;\
				set grid ytics;\
				set lmargin 10;\
				set rmargin 3;\
				set datafile separator ',' ;\
				plot '$i' using 16 title 'Latency' with points lt 1 pt 6 ps variable;\
			"
done 
#				set logscale y ;\
#				set xrange [1:*] ;\
#				set yrange [1:$YMAX] ;\
#				set terminal png size 800,1000;\