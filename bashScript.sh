#!/bin/bash

active=$(mktemp)
totalActive=0
inactive=$(mktemp)
totalInactive=0
cd $1

for fl in $(ls)
do
	if [ ! -d $fl ]; then
		continue
	fi
    pidname="./$fl/${fl}_pid"
    if [ -r $pidname ]; then
		pid=$(cat $pidname)
		exist=0
		for t in $(ps -p $pid)
		do
			if [ "$pid" == "$t" ]; then
				exist=1
				totalActive=$(($totalActive+1))
				echo $(pwd)/$fl
			fi
		done >>"$active"
		
		if [ "0" -eq "$exist" ]; then
			echo $(pwd)/$fl >> "$inactive"
			totalInactive=$(($totalInactive+1))
		fi
	fi
done

echo "$totalActive Boards Active:"
cat $active
echo "$totalInactive inactive Board (on disk but no server running):"
cat $inactive
rm $active
rm $inactive
