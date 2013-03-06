#!/bin/bash

for basedir in "src" "tests"
do
    for file in `ls $basedir/*.{c,h}`
    do
        echo "##### Indenting file $file"
        # I don't like the -cs option (space after casts)
        # but indent doesn't recognise user types so we 
        # should be consistent.
        indent -cs -br -ce -brf -i4 -nsai -nsaw -nsaf -npcs -nut -sob -npsl -nss $file -o $file
    done
done
