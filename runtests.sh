#!/bin/sh

make clean
make indexer
./indexer testfiles/test.xml
make search

rm testfiles/testoutput.txt
./search < testfiles/testsearch.txt > testfiles/testoutput.txt
diff testfiles/correctresult.txt testfiles/testoutput.txt

valgrind -q ./search < testfiles/testsearch.txt > /dev/null
