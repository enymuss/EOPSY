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

echo "Uppercase only folder name"
sh modify.sh -u exampleFolder
ls -d EXAMPLE*

echo "Incorrect, no -l, -u or sed pattern"
sh modify.sh -r ./exampleFolder

echo "Rename first occurance of old into new recursively"
./modify.sh -r "s/old/new/" ./exampleFolder
ls -R ./exampleFnewer

echo "Rename first occurance of new into old recursively"
./modify.sh -r "s/new/old/" ./exampleFnewer
ls -R ./exampleFolder

echo "Uppercase all files recursively"
./modify.sh -r -u ./exampleFolder
ls -R ./exampleFolder

echo "Lowercase all files recursively"
./modify.sh -r -l ./exampleFolder
ls -R ./exampleFolder

echo "uppercase multiple files"
sh modify.sh -u examplefolder/old3.c examplefolder/old2.c examplefolder/old4.c
ls ./examplefolder/O*

echo "lowercase multiple files"
sh modify.sh -l examplefolder/OLD3.c examplefolder/OLD2.c examplefolder/OLD4.c
ls ./examplefolder/o*.c

echo ""
echo "uppercase multiple folders"
sh modify.sh -u examplefolder/ examplefolder2/ examplefolder3/
ls -d EX*/

echo ""
echo "lowercase multiple folders"
sh modify.sh -l EXAMPLEFOLDER/ EXAMPLEFOLDER2/ EXAMPLEFOLDER3/
ls -d ex*/

echo ""
echo "uppercase multiple folders recursively"
sh modify.sh -r -u examplefolder/ examplefolder2/ examplefolder3/
ls -d EX*/

echo ""
echo "lowercase multiple folders recursively"
sh modify.sh -r -l EXAMPLEFOLDER/ EXAMPLEFOLDER2/ EXAMPLEFOLDER3/
ls -d ex*/

echo "Rename first occurance of old into new recursively multiple folders"
./modify.sh -r "s/old/new/" ./exampleFolder ./examplefolder2 ./examplefolder3
ls -R ./example*/

echo "Rename first occurance of new into old recursively"
./modify.sh -r "s/new/old/" ./exampleFnewer ./examplefnewer2/ ./examplefnewer3/
ls -R ./
