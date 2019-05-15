#!/bin/sh

make clean
make indexer
./indexer ../431searchengine/wsj.xml
make search

rm testfiles/wsjtestoutput.txt
./search < testfiles/wsjtestinput.txt > testfiles/wsjtestoutput.txt
diff testfiles/correctwsjresult.txt testfiles/wsjtestoutput.txt

valgrind -q ./search < testfiles/wsjtestinput.txt > /dev/null
