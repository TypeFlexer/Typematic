#!/bin/bash

C3_DIR=$(pwd)

set -e

all_passed=true

for test_case in $(ls ./tests/checked_cases/)
do
  echo testing $test_case
  full=./tests/checked_cases/$test_case
  LASTCASE=$full
  if ! C3_DIR=. ./tests/test_checked.sh $full; then
    echo Test $LASTCASE failed!
    all_passed=false
  fi
done

if $all_passed; then
  echo 'All Tests Pass!'
else
  echo 'At least one test failed (see above)!'
fi

# Set exit status.
$all_passed
