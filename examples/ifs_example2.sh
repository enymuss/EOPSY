#!/bin/sh
cat /etc/passwd | while read line
do
        IFS=: set $line
# or:   if test "$1" = "bin"
        if [ "$1" = "bin" ]
        then
                echo $3
        fi
done 
