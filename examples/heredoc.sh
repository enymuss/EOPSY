#!/bin/sh

# example variable
X="value of X variable"

# instead of "cat < help.txt" here unnamed internal text file like
# help.txt built into script - to be an input to some command,
# here 'cat'. That idea in shell with << redirector is called
# 'here document'.
echo "-- some line --"
cat <<EOT
This is a long formatted text where I can use variables like
  $X

  some formatted output

next line of the message
  next line of the message
  next line of the message
next line of the message
  next line of the message
EOT
echo "-- other line --"
