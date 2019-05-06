#!/bin/sh

rm testfiles/testoutput.txt

./search < testfiles/testinput.txt > testfiles/testoutput.txt

diff testfiles/correctoutput.txt testoutput.txt


valgrind -q ./search < testfiles/testinput.txt > /dev/null



