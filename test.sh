#!/bin/bash

for f in bftest/*.b; do
    echo
    echo "---------------"
    echo "Compiling $f.."
    echo
    ./brainf -c $f
    ./b
done
