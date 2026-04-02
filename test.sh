#!/bin/bash

trap 'rm bftest/*.out; exit' INT TERM

for f in bftest/*.b; do
    echo
    echo "---------------"
    echo "Compiling $f.."
    echo
    ./brainf -c $f
    base="${f%.b}"
    ./${base}.out
done

rm bftest/*.out
