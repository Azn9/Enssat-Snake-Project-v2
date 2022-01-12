#!/bin/bash

if [ "$#" -eq "0" ] || [ "$#" -lt 3 ]; then
  echo "./run.sh <0|time> <speed> <debug: on|off> [map]"
  exit 0
fi

if [ "$1" -eq "0" ]; then
  sudo ntpdate 129.6.15.28
  date +"%s"
else
  DATE=$(date --date="@$1")
#  echo $DATE
  sudo date -s "$DATE"
fi

if [ "$#" -eq "3" ]; then
  MAP="level-20x10.map"
else
  MAP="$4"
fi

./snake -debug $3 -display color -delay $2 $MAP

#sudo ntpdate 129.6.15.28
