#!/bin/bash
# C3 Testing Script 
# Tests an already checked c file by running C3 and then attempting to compile

if [ -z $C3_DIR ]  
then
  echo "Please set \$C3_DIR to point to the C3 directory"
  exit 1
fi

if [ -z $1 ]
then
  echo Usage: $0 "<test_file>"
  exit 1
fi

# TARGET=$(realpath $1)
TARGET=$1

set -e
set -o pipefail

cd $C3_DIR
CC="clang -x c -std=c89 -fsyntax-only"
dune exec -- src/main.exe -f $TARGET | $CC -
