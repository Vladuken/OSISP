#!/bin/bash

dir1=$1
dir2=$2
num=$(find $dir1 $dir2 -type f 2>/dev/null | wc -l)
(


#count number of equall files
#checked=0

IFS=$'\n'

set1=$(find $dir1 -type f)
set2=$(find $dir2 -type f)

#num=${set1}${set2}



for var1 in $set1
do
	if [ -f "$var1" ]
	then
		#echo $var1
		for var2 in $set2
		do
			#if [ "$var1" != "$var2" ]
			#then

				if [ -f "$var2" ]
				then
					if (diff -q $var1 $var2)
					then
						echo  $var1 = $var2 
						#let "checked=checked + 1"
					fi
				fi			
	
			#fi			
						
		done
	fi
done 

 
) 2>/tmp/errors.txt | grep =


sed -i "s/^/$(basename $0:" ")/g"  /tmp/errors.txt 
cat /tmp/errors.txt >&2

echo $num

 

