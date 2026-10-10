#!/bin/sh 

# rm *.png 

FILE_IN="$1"
FILE_OUT="$2"
FILE_PNG="$3"
TITLE_TXT="$4"

gnuplot << EOF
reset
set terminal postscript eps color enhanced
set term png size 2000,2000

set title "$TITLE_TXT"
set output "$FILE_PNG"
plot   "$FILE_IN" with lines ,"$FILE_OUT" with linespoints

#plot   "$FILE_OUT" with linespoints

EOF

rm "$FILE_IN" "$FILE_OUT"
