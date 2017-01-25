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
XLABLE="IO Offset"
YLABLE="Time (us)"

for i in $@
	do	
	awk 'BEGIN {OFS = "," }; {print $8,$4*1000000}' ${i}.Q >Q
	awk 'BEGIN {OFS = "," }; {print $4*1000000}' ${i}.I >I
	awk 'BEGIN {OFS = "," }; {print $4*1000000}' ${i}.D >D
	awk 'BEGIN {OFS = "," }; {print $4*1000000}' ${i}.C >C
	paste -d , Q I D C > ${i}.Timing
	rm Q I D C
	# time diff : offset, Q2I, I2D, D2C, 
	awk 'BEGIN {FS = "," ; OFS = "," }; {print $1,$3-$2,$4-$3,$5-$4}' ${i}.Timing >${i}.TimeDiff 
	rm ${i}.Timing
		gnuplot -e " 	set ylabel '$YLABLE';\
				set xlabel '$XLABLE';\
				set ytics nomirror tc lt 2;\
				set xrange [0:*];\
				set yrange[0:*];\
				set title '$i';\
				set terminal png size 10000,800;\
				set output '${i}_TimeDiff.png' ;\
				set grid x y;\
				set key top right outside;\
				set lmargin 12;\
				set rmargin 24;\
				set datafile separator ',' ;\
				plot \
				'${i}.TimeDiff' using 2 title 'Q2I Time' with points lt 2 pt 6 ps 0.5 ,\
				'${i}.TimeDiff' using 3 title 'I2D Time' with points lt 3 pt 6 ps 0.5 ,\
				'${i}.TimeDiff' using 4 title 'D2C Time' with points lt 4 pt 6 ps 0.5 ,\
				'$i' 	     using 16 title 'H2L Time' with points lt 1 pt 6 ps 0.5 ;\
			"
done 
#				set logscale y ;\
#				set xrange [1:*] ;\
#				set yrange [1:$YMAX] ;\
#				set terminal png size 800,1000;\
#				set format x '%g %%';\
