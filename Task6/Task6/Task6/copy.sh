#!/bin/sh

gcc main.c -o copy

chmod +x copy

./copy old.txt new.txt

cat new.txt

rm new.txt

