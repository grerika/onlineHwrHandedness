#! /bin/bash


for F in Data/*/*/*xml; do
	./detectHandedness.py "$F"
done 2>run.err | tee run.out 
