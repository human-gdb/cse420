#!/bin/bash

./clean_os.sh
rm *.txt
rm *~
make clean
make

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
sed 's,replace,'"$DIR"',' inputFile_t2_1.dat > inputFile_t2_1.txt
sed 's,replace,'"$DIR"',' inputFile_t2_2.dat > inputFile_t2_2.txt
sed 's,replace,'"$DIR"',' inputFile_t2_3.dat > inputFile_t2_3.txt
sed 's,replace,'"$DIR"',' inputFile_t2_4.dat > inputFile_t2_4.txt
sed 's,replace,'"$DIR"',' inputFile_t2_5.dat > inputFile_t2_5.txt
./clean_os.sh
rm output.txt
echo "Starting 10, 10 test 2"
./run_t2_1
sleep 10
sort output.txt > student_out_t2_1.txt
./clean_os.sh

if diff -w student_out_t2_1.txt correct_output/correct_output_t2.txt; then
    echo Success---------------------Test 2-1--------------------------Success
else
    echo Fail------------------------Test 2-1--------------------------Fail
fi

rm output.txt

echo "Starting 5, 10 test 2"
./run_t2_2
sleep 10
sort output.txt > student_out_t2_2.txt
./clean_os.sh

if diff -w student_out_t2_2.txt correct_output/correct_output_t2.txt; then
    echo Success---------------------Test 2-2--------------------------Success
else
    echo Fail------------------------Test 2-2--------------------------Fail
fi

rm output.txt
echo "Starting 1, 10 test 2"
./run_t2_3
sleep 10
sort output.txt > student_out_t2_3.txt
./clean_os.sh

if diff -w student_out_t2_3.txt correct_output/correct_output_t2.txt; then
    echo Success---------------------Test 2-3--------------------------Success
else
    echo Fail------------------------Test 2-3--------------------------Fail
fi

rm output.txt
echo "Starting 20, 1 test 2"
./run_t2_4
sleep 10
sort output.txt > student_out_t2_4.txt
./clean_os.sh

if diff -w student_out_t2_4.txt correct_output/correct_output_t2.txt; then
    echo Success---------------------Test 2-4--------------------------Success
else
    echo Fail------------------------Test 2-4--------------------------Fail
fi


