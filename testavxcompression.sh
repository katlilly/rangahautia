#!/bin/sh

make clean
make indexer
./indexer testfiles/test.xml
make testavx

rm testfiles/testoutput.txt
./testavx < testfiles/testsearch.txt > testfiles/testoutput.txt
diff testfiles/correctresult.txt testfiles/testoutput.txt
