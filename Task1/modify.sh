#!/bin/sh

# add script to path
# cp modify.sh modify
# mv modify /usr/local/bin/
# chmod 755 /usr/local/bin/modify

# the name of the script without a path
name=`basename $0`

# function for printing error messages to diagnostic output
error_msg()
{
        echo "$name: error: $1" 1>&2
}

# basic checks of input
check()
{
if test $l = "y" && test $u = "y"
then
  error_msg "Both lowercase and uppercase options selected."
  exit 1;
fi

if test $l = "n" && test $u = "n" && test $sedPattern = "n"
then
  error_msg "No l, u or sed pattern as arguments"
  exit 1;
fi

if test -z "$1"
then
  error_msg "File path empty"
  exit 1;
fi

}

#save arg as sedPattern or filePath
save_arg()
{
  argFilePath=$1
  if test -z "$sedPattern" && test $l = 'n' && test $u = 'n'
  then
    sedPattern=$argFilePath
  elif test -d $argFilePath || test -f $argFilePath
  then
    filePath=$argFilePath
    #modify the filePath passed
    doSomethingWithFilePath $argFilePath
  else
    error_msg "argument not accepted: $argFilePath"
  fi
}

doSomethingWithFilePath()
{
  argFilePath=$1
  check $argFilePath

  # if not recursive, find only in current dir
  if test $r = "n"
  then
    maxdepth="-maxdepth 0"
  fi

  #go recursively through each file and apply the modification depth first.
  find $argFilePath -d $maxdepth | while IFS=: read foundFilePath
  do
    if test $l = "y"
    then
      lowercase $foundFilePath
    elif test $u = "y"
    then
      uppercase $foundFilePath
    elif test -n "$sedPattern" && test -n "$foundFilePath"
    then
      sed_modify $sedPattern $foundFilePath
    else
      error_msg "Skipped all ifs for $foundFilePath"
    fi
  done
}

lowercase()
{
  #replace each big letter with small letter
  toLowercaseSed="y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/"
  string="$(sed "$toLowercaseSed" <<< "$1")"
  mv $1 $string
}

uppercase()
{
  #replace each small letter with big letter
  toUppercaseSed="y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/"
  string="$(sed "$toUppercaseSed" <<< "$1")"
  mv $1 $string
}

sed_modify() {
  #enter into dir and modify filename.
  #since find results are depth first, it changes the dir name last

  #$1 = $sedPattern
  #$2 = $filePath

  pwdString=$(pwd)
  cd $(dirname $2)

  string="$(sed "$1" <<< "$(basename $2)")"

  mv $(basename $2) $string
  cd $pwdString
}

# if no arguments given
if test -z "$1" || test "$1" = "-h"
then
cat<<EOT 1>&2

usage:
  $name [-r] [-l|-u] <dir/file names...>
  $name [-r] <sed pattern> <dir/file names...>
  $name [-h]

$name correct syntax examples:
  $name -l new.c new2.c new3.c
  $name -u examplefolder

$name incorrect syntax example:
  $name -l -u new.c
  $name new.c


EOT
exit 0;
fi

l=n
u=n
r=n
h=n
filePath=""
sedPattern=""
maxdepth=""

while test "x$1" != "x"
do
        case "$1" in
                -r|--recursive) r=y;;
                -l|--lower) l=y;;
                -u|--upper) u=y;;
                -h|--help) h=y;;
                -*) error_msg "bad option $1"; exit 1 ;;
                *) save_arg $1
        esac
        shift
done
# check again for any errors
check $filePath
exit 0;
