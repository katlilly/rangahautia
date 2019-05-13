#!/bin/sh


g++ -fprofile-arcs -ftest-coverage vbyte_compress.cpp search.cpp -lm
./a.out < testfiles/testsearch.txt
gcov search.cpp


