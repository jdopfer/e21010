#!/usr/bin/env bash

mkdir -p res/macros/tmp
mkdir -p data/foil_search

source /usr/local/bin/geant4.sh

X=$1
Y=$2

XX=`echo $(bc <<< "${X}*10") | cut -d. -f1`
YY=`echo $(bc <<< "${Y}*10") | cut -d. -f1`

if [ $XX -lt 0 ] ; then
  XX="neg$(bc <<< "${XX}*-1")"
fi
if [ $YY -lt 0 ] ; then
  YY="neg$(bc <<< "${YY}*-1")"
fi

sed -e "s/\/G4Sim\/outputFile .*/\/G4Sim\/outputFile ..\/data\/foil_search\/${XX}_${YY}.root/" \
    -e "s/\/G4Sim\/isotropic\/position .*/\/G4Sim\/isotropic\/position ${X} ${Y} 0. mm/" \
    res/macros/iso_proton.mac > res/macros/tmp/${XX}_${YY}.mac

../cmake-build-debug/sim/sim res/macros/tmp/${XX}_${YY}.mac
