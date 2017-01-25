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
XLABLE="Bundle Size"
YLABLE="Bundle Frequency"

for i in $@
	do
	
		gnuplot -e " 	set ylabel '$YLABLE';\
				set xlabel '$XLABLE';\
				set ytics nomirror tc lt 1;\
				set yrange [1:*];\
				set xrange [1:*];\
				set autoscale x;\
				set title '$i';\
				set terminal png ;\
				set output '${i}.png' ;\
				set grid x y;\
                unset key;
				plot '$i' using 1:2 axes x1y1 with l ;
			"
done 
#				set logscale y ;\
#				set xrange [1:*] ;\
#				set yrange [1:$YMAX] ;\
#				set terminal png size 800,1000;\
#				set format x '%g %%';\
