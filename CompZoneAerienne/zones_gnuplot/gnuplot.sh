#!/bin/sh 

#rm *.png 

FILE_IN="$1"_avc.txt
FILE_OUT="$1"_apc.txt
FILE_PNG="$1".png

gnuplot << EOF
reset
set terminal postscript eps color enhanced
set term png size 1900,1200

set output "$FILE_PNG"
plot   "$FILE_IN" with linespoints,"$FILE_OUT" with linespoints
#plot   "$FILE_OUT" with linespoints

EOF
