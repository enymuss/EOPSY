#!/bin/sh

#to create testing hierarchy
#touch new.c
#mkdir exampleFolder
#cd exampleFolder
#touch new2.c new3.c new4.c new5.c
#mkdir newExampleFolder
#cd newExampleFolder
#touch newnew.c newnew2.c


echo "Uppercase new.c"
./modify.sh -u new.c
ls N*

echo "Lowercase new.c"
./modify.sh -l NEW.C
ls n*

echo "Rename new.c into old.c"
./modify.sh "s/new/old/" new.c
ls o*

echo "Rename old.c into new.c"
./modify.sh "s/old/new/" old.c
ls n*

echo "Incorrect, modify operates only on files"
sh modify.sh -u exampleFolder
ls -d *FOLDER

echo "Incorrect, no -l, -u or sed pattern"
sh modify.sh -r ./exampleFolder

echo "Rename all new into old recursively"
./modify.sh -r "s/new/old/" ./exampleFolder
ls -R ./exampleFolder

echo "Rename all old into new recursively"
./modify.sh -r "s/old/new/" ./exampleFolder
ls -R ./exampleFolder

echo "Uppercase all files recursively"
./modify.sh -r -u ./exampleFolder
ls -R ./exampleFolder

echo "Lowercase all files recursively"
./modify.sh -r -l ./exampleFolder
ls -R ./exampleFolder
