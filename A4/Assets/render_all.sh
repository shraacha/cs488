#!/usr/bin/env sh

for i in $(find . -name \*.lua); do # Not recommended, will break on whitespace
    ../A4 "$i"
done
