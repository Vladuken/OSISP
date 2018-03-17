#!/bin/bash


path=$1
errfiles=$2
(
echo -n $$
wc -wc $path 
) 2>/tmp/errors.txt | grep =


sed -i "s/^/$(basename $1:" ")/g"  $errfiles 
cat  $errfiles>&2

echo $num

 

