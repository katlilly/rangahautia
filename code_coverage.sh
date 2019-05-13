#!/bin/sh


g++ -std=c++11 -fprofile-arcs -ftest-coverage vbyte_compress.cpp search.cpp -lm
./a.out < testfiles/testsearch.txt
gcov search.cpp


