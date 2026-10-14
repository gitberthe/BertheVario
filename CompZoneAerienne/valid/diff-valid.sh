#!/bin/sh

clear 
diff valid/zonvalou.txt SDCardAllVario/valid/zonvalou.ref.txt
echo ""
echo "mini (indifferent des noms):"
echo ""

cat valid/zonvalou.txt                    | tr ':' ';' | cut -d';' -f 1,6,7 - > ./tmp/zonvalou.mini.txt 
cat SDCardAllVario/valid/zonvalou.ref.txt | tr ':' ';' | cut -d';' -f 1,6,7 - > ./tmp/zonvalou.ref.mini.txt

diff ./tmp/zonvalou.mini.txt ./tmp/zonvalou.ref.mini.txt

rm ./tmp/zonvalou.mini.txt ./tmp/zonvalou.ref.mini.txt
