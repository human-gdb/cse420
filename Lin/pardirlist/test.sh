#!/bin/bash

rm *.txt
make clean
make

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
sed 's,replace,'"$DIR"',' files/correct_final-src.txt > correct_final-src.txt
sed 's,replace,'"$DIR"',' files/correct_linux-master.txt > correct_linux-master.txt

sudo sh -c 'sync && echo 3 > /proc/sys/vm/drop_caches'
start=$(date +%s%N)
./pardirlist "$DIR/files/final-src" int sout_seq_final-src.txt 0
end=$(date +%s%N)

if diff -w sout_seq_final-src.txt correct_final-src.txt; then
    echo Test 1 - Success--------------------seq-final-src----------------------------Success
else
    echo Test 1 - Fail-----------------------seq-final-src----------------------------Fail
fi
echo -e "Elapsed time (seq-final-src): $(($(($end-$start))/1000000)) ms\n"

sudo sh -c 'sync && echo 3 > /proc/sys/vm/drop_caches'
start=$(date +%s%N)
./pardirlist "$DIR/files/final-src" int sout_par_final-src.txt 1
end=$(date +%s%N)

if diff -w sout_par_final-src.txt correct_final-src.txt; then
    echo Test 2 - Success--------------------par-final-src----------------------------Success
else
    echo Test 2 - Fail-----------------------par-final-src----------------------------Fail
fi
echo -e "Elapsed time (par-final-src): $(($(($end-$start))/1000000)) ms\n"

sudo sh -c 'sync && echo 3 > /proc/sys/vm/drop_caches'
start=$(date +%s%N)
./pardirlist "$DIR/files/linux-master" int sout_seq_linux-master.txt 0
end=$(date +%s%N)

if diff -w sout_seq_linux-master.txt correct_linux-master.txt; then
    echo Test 3 - Success------------------seq-linux-master---------------------------Success
else
    echo Test 3 - Fail---------------------seq-linux-master---------------------------Fail
fi
echo -e "Elapsed time (seq-linux-master): $(($(($end-$start))/1000000)) ms\n"

sudo sh -c 'sync && echo 3 > /proc/sys/vm/drop_caches'
start=$(date +%s%N)
./pardirlist "$DIR/files/linux-master" int sout_par_linux-master.txt 1
end=$(date +%s%N)

if diff -w sout_par_linux-master.txt correct_linux-master.txt; then
    echo Test 4 - Success------------------par-linux-master---------------------------Success
else
    echo Test 4 - Fail---------------------par-linux-master---------------------------Fail
fi
echo -e "Elapsed time (par-linux-master): $(($(($end-$start))/1000000)) ms\n"
