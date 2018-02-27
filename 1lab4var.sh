#!/bin/bash

(
dir1=$1
dir2=$2


#count number of equall files
checked=0
for var1 in $(find $dir1 -type f)
do
	echo $var1
	for var2 in $(find $dir2 -type f)
	do
		if (diff -q $var1 $var2)
		then
			echo  $var1 = $var2 
			let "checked=checked + 1"
		fi
	done
done 

find $dir1 $dir2 -type f | wc -l 
) 2>errors.txt | grep =

 


