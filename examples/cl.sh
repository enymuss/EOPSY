#!/bin/sh
# command options and arguments interpreter example
# simple version

# the name of the script without a path
name=`basename $0`

# function for printing error messages to diagnostic output
error_msg()
{
        echo "$name: error: $1" 1>&2
}

# function for servicing -w option
with_arg()
{
        if test -z "$1"
        then
                error_msg "missing argument for -w"
                exit 1
        fi
        echo "-w with argument: $1"
}


# if no arguments given
if test -z "$1"
then
cat<<EOT 1>&2

usage:
  $name [-f|--first] [-s|--second] [-w arg] <names>

$name correct syntax examples:
  $name new.c
  $name -f --second file.c
  $name -f -w arg_w hello.c first.sh

$name incorrect syntax example:
  $name -d

EOT
fi


# do with command line arguments
f=n
s=n
while test "x$1" != "x"
do
        case "$1" in
                -f|--first) f=y;;
                -s|--second) s=y;;
                -w) with_arg "$2"; shift;;
                -*) error_msg "bad option $1"; exit 1 ;;
                *) echo "arg: $1"
        esac
        shift
done
if test $f = "y"
then
        echo "with option --first"
fi
if test $s = "y"
then
        echo "with option --second"
fi
