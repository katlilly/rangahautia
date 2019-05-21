#!/bin/sh


g++ -std=c++11 -fprofile-arcs -ftest-coverage vbyte_compress.cpp search.cpp -lm
g++ -std=c++11 -fprofile-arcs -ftest-coverage vbyte_compress.cpp growablearray.cpp tokeniser.cpp indexer.cpp -o indexer

./indexer testfiles/test.xml
./a.out < testfiles/testsearch.txt

gcov indexer.cpp vbyte_compress.cpp tokeniser.cpp growablearray.cpp search.cpp

