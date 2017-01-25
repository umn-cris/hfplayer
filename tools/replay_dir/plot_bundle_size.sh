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

set -o nounset # Treat unset 1s as an errorwdev_0
XLABLE="Bundle Offset"
YLABLE="Bundle Size"

for i in $@
	do
	
		gnuplot -e " 	set ylabel '$YLABLE';\
				set xlabel '$XLABLE';\
				set ytics nomirror tc lt 1;\
				set yrange [1:*];\
				set xrange [1:*];\
				set autoscale x;\
				set title '$i';\
				set terminal png size 1600,800;\
				set output '${i}.png' ;\
				set grid x y;\
				set lmargin 12;\
				set rmargin 24;\
				plot '$i' using 1 axes x1y1 with p lt 1 pt 6 ps 0.5 ;
			"
done 
#				set logscale y ;\
#				set xrange [1:*] ;\
#				set yrange [1:$YMAX] ;\
#				set terminal png size 800,1000;\
#				set format x '%g %%';\
