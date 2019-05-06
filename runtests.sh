#!/bin/sh

make clean
make indexer
./indexer
make search

rm testfiles/testoutput.txt
./search < testfiles/testinput.txt > testfiles/testoutput.txt
diff testfiles/correctoutput.txt testfiles/testoutput.txt

valgrind -q ./search < testfiles/testinput.txt > /dev/null
