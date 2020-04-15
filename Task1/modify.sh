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

save_arg()
{
  if test -z "$sedPattern" && test $l = 'n' && test $u = 'n'
  then
    sedPattern=$1
  elif test -z "$filePath"
  then
    filePath=$1
  else
    error_msg "too many arguments: $1"
    exit 1;
  fi
}

lowercase()
{
  toLowercaseSed="y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/"
  # sed_modify $toLowercaseSed $1
  string="$(sed "$toLowercaseSed" <<< "$1")"
  mv $1 $string
}

uppercase()
{
  # sed_modify $toUppercaseSed $1
  toUppercaseSed="y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/"
  string="$(sed "$toUppercaseSed" <<< "$1")"
  mv $1 $string
}

sed_modify() {
  #$1 = $sedPattern
  #$2 = $filePath

  fileNameString=$(basename $2)
  dirPath=$(dirname $2)
  newfileNameString="$(sed "$1" <<< "$fileNameString")"
  string="$dirPath/$newfileNameString"

  mv $2 $string
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
  [TODO]

$name incorrect syntax example:
  $name -l -u new.c
  $name -l new.c new2.c new3.c
  $name -u examplefolder

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
                #-w) with_arg "$2"; shift;;
                -*) error_msg "bad option $1"; exit 1 ;;
                *) save_arg $1
        esac
        shift
done

if test $l = "y" && test $u = "y"
then
  error_msg "Both lowercase and uppercase options selected."
  exit 1;
fi

if test -z "$filePath"
then
  error_msg "File path empty"
  exit 1;
fi

if test -d $filePath && test $r = "n"
then
  error_msg "modify works only on files"
  exit 1;
fi

if test $r = "n"
then
  maxdepth="-maxdepth 1"
fi

find "$filePath" -type f $maxdepth | while IFS=: read foundFilePath
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
exit 0;
