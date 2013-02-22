#!/bin/bash

NB=45875200
DIR=/home/pwohler/src/var-monte-carlo-opencl/code
BIN=$DIR/bin/pfe
PORTEFEUILLE=$DIR/portefeuille.csv
TRACE=$DIR/trace.csv
T=1
SEUIL=0.99

$BIN -c $SEUIL -n $NB -p $PORTEFEUILLE -t $T -b >> $TRACE
