#!/bin/bash

find . \( -name '*.c' -o -name '*.h' \) -exec printf "Indenting file {}\n" \; -exec astyle --style=java --unpad-paren --delete-empty-lines --add-brackets --convert-tabs --align-pointer=type --lineend=linux --suffix=none {} \;
