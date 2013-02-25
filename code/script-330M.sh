#!/bin/bash

NB_ITERATIONS=4
DIR=/Users/paraita/src/var-monte-carlo-opencl/code
BIN=$DIR/bin/pfe
PORTEFEUILLE=$DIR/portefeuille.csv
TRACE=$DIR/trace-330M.csv
T=1
SEUIL=0.99
#N=9830400
N=98304

for ((i=1; i<=NB_ITERATIONS; i++))
do
    $BIN -c $SEUIL -n $N -p $PORTEFEUILLE -t $T >> $TRACE
done
