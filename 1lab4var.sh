#!/bin/bash

(
dir1=$1
dir2=$2


#count number of equall files
checked=0
for var1 in $(find $dir1 -type f)
do
	if [ -f "$var1" ]
	then
		echo $var1
		for var2 in $(find $dir2 -type f)
		do
			if [ -f "$var2" ]
			then
				if (diff -q $var1 $var2)
				then
					echo  $var1 = $var2 
					let "checked=checked + 1"
				fi
			fi
		done
	fi
done 

 
) 2>/tmp/errors.txt | grep =


sed -i "s/^/$(basename $0:" ")/g"  /tmp/errors.txt
#cat /tmp/errors.txt

echo $(find $dir1 $dir2 -type f | wc -l)
 


