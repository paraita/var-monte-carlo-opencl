#!/bin/bash

NB_ITERATIONS=10
DIR=/home/paittaha/src/var-monte-carlo-opencl/code
BIN=$DIR/bin/pfe
PORTEFEUILLE=$DIR/portefeuille.csv
TRACE=$DIR/trace.csv
T=1
SEUIL=0.99
N=45875200

for i in {1..10}
do
    $BIN -c $SEUIL -n $N -p $PORTEFEUILLE -t $T >> $TRACE
done
