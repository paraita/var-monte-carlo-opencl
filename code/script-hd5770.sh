#!/bin/bash

NB_ITERATIONS=3
#DIR=/home/pwohler/src/var-monte-carlo-opencl/code
DIR=/home/paittaha/var-monte-carlo-opencl/code
BIN=$DIR/bin/pfe
PORTEFEUILLE=$DIR/portefeuille.csv
TRACE=$DIR/trace-hd5770.csv
T=1
SEUIL=0.99
N=10485760

for ((i=1; i<=NB_ITERATIONS; i++))
do
    $BIN -c $SEUIL -n $N -p $PORTEFEUILLE -t $T >> $TRACE
done
