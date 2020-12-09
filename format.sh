#!/bin/bash
format_files=`find libs tests binary -type f -name "*.cc" -o -name "*.hpp"`

for file in $format_files
do
  clang-format -i "$file"
done