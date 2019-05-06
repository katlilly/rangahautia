#!/bin/sh

./search < testinput.txt > testoutput.txt

diff correctoutput.txt testoutput.txt
