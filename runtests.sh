#!/bin/sh

rm testoutput.txt

./search < testinput.txt > testoutput.txt

diff correctoutput.txt testoutput.txt
