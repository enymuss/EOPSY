#!/bin/sh
# how to find user id for some login name, here login name: bin
cat /etc/passwd | while IFS=: read login x id line
do
        if test "$login" = "bin"
        then
                echo $id
        fi
done 
