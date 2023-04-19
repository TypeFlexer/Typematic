#!/bin/bash 
#read the input path and file name
input_file=$1


#create a temporary copy of the original file to enable us to write our coverted code into
touch $input_file"_cpy.c"

eval $(opam env)
dune exec -- src/main.exe -f $input_file > $input_file"_cpy.c"

mv $input_file"_cpy.c" $input_file

