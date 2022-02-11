#!/bin/bash

printf "\x04"
echo $(basename $1)
cat $1
printf "\x00"
