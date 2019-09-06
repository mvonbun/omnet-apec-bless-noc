#!/usr/bin/env bash

for b in $(seq -f "%02g" 1 16); do
    for c in $(seq -f "%02g" 1 16); do
	echo "${b}_${c}_dec.dat"
	cp dummcode.dat "single/${b}_${c}_dec.dat"
	cp dummcode.dat "burst/${b}_${c}_dec.dat"
	cp dummcode.dat "joint/${b}_${c}_dec.dat"
    done
done

    
