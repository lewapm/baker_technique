#!/bin/bash
#g++ ./gen.cpp -o gen -std=c++11 -O2
a=1;
licznik=1;
while [ $a -le 20 ]; do
	echo $RANDOM $RANDOM > seed;
	./gen > in;
	./wz <in
	echo "udalo sie"
done;
