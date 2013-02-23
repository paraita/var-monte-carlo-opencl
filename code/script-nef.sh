#!/bin/bash

NB_ITERATIONS=10
DIR=/home/paittaha/src/var-monte-carlo-opencl/code
BIN=$DIR/bin/pfe
PORTEFEUILLE=$DIR/portefeuille.csv
TRACE=$DIR/trace-nef.csv
T=1
SEUIL=0.99
N=45875200

for ((i=1; i<=NB_ITERATIONS; i++))
do
    $BIN -c $SEUIL -n $N -p $PORTEFEUILLE -t $T >> $TRACE
done
