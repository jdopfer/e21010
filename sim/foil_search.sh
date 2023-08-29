#!/usr/bin/env bash

mkdir -p res/macros/tmp
mkdir -p data/foil_search

source /usr/local/bin/geant4.sh

X=`echo $(bc <<< "$1*1")`
Y=`echo $(bc <<< "$2*1")`
Z=`echo $(bc <<< "$3*1")`

XX=`echo $(bc <<< "${X}*10") | cut -d. -f1`
YY=`echo $(bc <<< "${Y}*10") | cut -d. -f1`
ZZ=`echo $(bc <<< "${Z}*10") | cut -d. -f1`

if [ $XX -lt 0 ] ; then
  XX="neg$(bc <<< "${XX}*-1")"
fi
if [ $YY -lt 0 ] ; then
  YY="neg$(bc <<< "${YY}*-1")"
fi
if [ $ZZ -lt 0 ] ; then
  ZZ="neg$(bc <<< "${ZZ}*-1")"
fi

gsed -e "s/\/G4Sim\/outputFile .*/\/G4Sim\/outputFile ..\/data\/foil_search\/${XX}_${YY}_${ZZ}.root/" \
          -e "s/\/G4Sim\/target\/position .*/\/G4Sim\/target\/position 0. 0. ${Z} mm/" \
          -e "s/\/G4Sim\/isotropic\/position .*/\/G4Sim\/isotropic\/position ${X} ${Y} 0. mm/" \
          res/macros/iso_proton.mac > res/macros/tmp/${XX}_${YY}_${ZZ}.mac

../build/sim/sim res/macros/tmp/${XX}_${YY}_${ZZ}.mac

#echo "${X} ${Y} ${Z} ${XX} ${YY} ${ZZ}"

#printf "(\"${XX}\", \"${YY}\"), " >> combinations_xy.dat
#printf "(${X}, ${Y}), " >> combinations_num_xy.dat

#printf "(\"${YY}\", \"${ZZ}\"), " >> combinations_yz.dat
#printf "(${Y}, ${Z}), " >> combinations_num_yz.dat
