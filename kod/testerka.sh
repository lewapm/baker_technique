#!/bin/bash
g++ ./brut.cpp -o brut -O2
g++ ./gen.cpp -o gen -std=c++11 -O2
a=1;
licznik=1;
while [ $a -le 20 ]; do
	./gen >in;
	./wz <in >outwz;
	./brut <in >outbrut;
	if diff outwz outbrut -w >/dev/null; then
		echo $licznik;
		licznik=$((licznik+1));
	else
		echo "ERROR ";
		echo "in:"; cat in; echo "wz:"; cat outwz; echo "brut:"; cat outbrut;
		exit;
	fi;
done;
